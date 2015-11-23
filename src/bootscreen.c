/*
=================================================================
Copyright (C) 2005-2006 Torben Koenke

This program is free software; you can redistribute it and/or
modify it under the terms of the GNU General Public License
as published by the Free Software Foundation; either version 2
of the License, or (at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program; if not, write to the Free Software
Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston,
MA  02110-1301, USA.
=================================================================
*/

/*
# myPS2
# Author: tk
# Date: 11-14-2005
#
# File: Boot/Loading screen displayed while program
#		is loading.
#
*/

#include <misc.h>
#include <file.h>
#include <../lib/gsLib/gsLib.h>
#include <../lib/libpng/libpng.h>

#define BUILD_STRING		"(Build date "__DATE__", "__TIME__")"

#define MARGIN_Y_TOP		120
#define MARGIN_X_LEFT		50
#define MARGIN_X_RIGHT		640 - 50
//#define MARGIN_Y_BOTTOM		480 - 75
static int MARGIN_Y_BOTTOM;

// should be gsLib_font_height really
#define LINE_HEIGHT			15

#define COLOR_ESCAPE		'^'
#define COLOR_WHITE			'0'
#define COLOR_GREEN			'1'
#define COLOR_RED			'2'

static u32 RGBColorTable[] =
{
	colorWhite,		// COLOR_WHITE
	colorGreen,		// COLOR_GREEN
	colorRed		// COLOR_RED
};

static u32				nTableEntries	= sizeof(RGBColorTable) / sizeof(u32);

static int				bootscreen_x	= MARGIN_X_LEFT;
static int				bootscreen_y	= MARGIN_Y_TOP;
static GSFONT			*gsFont			= NULL;
static GSTEXTURE		*gsTexBkg		= NULL;

// compiled into ELF
extern unsigned int		size_boot_bkg;
extern unsigned char	boot_bkg[];
extern unsigned int		size_boot_font;
extern unsigned char	boot_font[];
extern unsigned int		size_boot_dat;
extern unsigned char	boot_dat[];

//
// Bootscreen_Clear - Clears bootscreen
//

void Bootscreen_Clear( void )
{
	char strVersion[128];
	int nHeight;

	gsLib_clear( colorBlack );

	if( gsTexBkg ) {
		nHeight = gsLib_get_mode() == GS_MODE_PAL ? 512 : 448;

		gsLib_prim_sprite_texture(	gsTexBkg, 0, 0, 640, nHeight, 0, 0,
									gsTexBkg->Width, gsTexBkg->Height, colorTex );
	}

	snprintf( strVersion, sizeof(strVersion), "myPS2 %s %s", MYPS2_VERSION, BUILD_STRING );

	gsLib_font_print( gsFont, 40, 40, RGB(0, 255, 0), strVersion );

	gsLib_font_print( gsFont, 40, 60, RGB(255, 255, 255), "Welcome to myPS2!" );
	gsLib_font_print( gsFont, 40, 75, RGB(255, 255, 255),
					  "Please wait while the program is loading" );
}

//
// Bootscreen_printf - Prints text to bootscreen
//

void Bootscreen_printf( const char *fmt, ... )
{
	va_list	args;
	u8 strBuffer[2048];
	
	int len;
	int i, nWidth, color;
	char c;
	u64 fontColor = RGB(255, 255, 255);

	va_start( args, fmt );
	len = vsnprintf( strBuffer, sizeof(strBuffer), fmt, args );

	for( i = 0; i < len; i++ ) {
		c = strBuffer[i];

		switch(c) {
			case '\n':
				bootscreen_x = MARGIN_X_LEFT;
				bootscreen_y = bootscreen_y + LINE_HEIGHT;

				if( bootscreen_y > MARGIN_Y_BOTTOM ) {
					bootscreen_y = MARGIN_Y_TOP;

					Bootscreen_Clear();
				}
				break;

			case '\t':
				bootscreen_x += gsLib_font_width( gsFont, " " ) * 5;

				if( bootscreen_x > MARGIN_X_RIGHT ) {
					bootscreen_x = MARGIN_X_LEFT;

					bootscreen_y = bootscreen_y + LINE_HEIGHT;

					if( bootscreen_y > MARGIN_Y_BOTTOM ) {
						bootscreen_y = MARGIN_Y_TOP;

						Bootscreen_Clear();
					}
				}
				break;

			case COLOR_ESCAPE:
				if( (i + 1) < len ) {
					i++;

					color = atoi(&strBuffer[i]);

					if( color < nTableEntries )
						fontColor = RGBColorTable[ color ];
				}
				break;

			default:
				nWidth = gsLib_font_print_char( gsFont, bootscreen_x, bootscreen_y,
												fontColor, c );

				bootscreen_x += nWidth;
				if( bootscreen_x > MARGIN_X_RIGHT ) {
					bootscreen_x = MARGIN_X_LEFT;
					bootscreen_y = bootscreen_y + LINE_HEIGHT;

					if( bootscreen_y > MARGIN_Y_BOTTOM ) {
						bootscreen_y = MARGIN_Y_TOP;

						gsLib_clear( colorBlack );
					}
				}
				break;
		}
	}
}

//
// Bootscreen_Init - Initializes bootscreen
//

void Bootscreen_Init( void )
{
	FHANDLE	fHandle;
	int		size;
	char	strPath[256];
	u8		*data, *pPNGFile, *pDATFile;
	u32		nPNGSize, nDATSize;
	pngData *pPng;
	u8		*pRGB;
	int		nPSM;

	gsLib_init( GS_MODE_AUTO, 0 );
	MARGIN_Y_BOTTOM =	gsLib_get_mode() == GS_MODE_PAL ? (512 - 75) :
						(448 - 75);

	strcpy( strPath, GetElfPath() );
	strcat( strPath, "boot/font.png" );

	pPNGFile = pDATFile = NULL;
	nPNGSize = nDATSize = 0;

	fHandle = FileOpen( strPath, O_RDONLY );
	if( fHandle.fh >= 0 ) {
		
		nPNGSize = FileSeek( fHandle, 0, SEEK_END );
		FileSeek( fHandle, 0, SEEK_SET );

		pPNGFile = malloc(nPNGSize);
		FileRead( fHandle, pPNGFile, nPNGSize );
		FileClose(fHandle);
	}

	strcpy( strPath, GetElfPath() );
	strcat( strPath, "boot/font.dat" );

	fHandle = FileOpen( strPath, O_RDONLY );
	if( fHandle.fh >= 0 ) {
		
		nDATSize = FileSeek( fHandle, 0, SEEK_END );
		FileSeek( fHandle, 0, SEEK_SET );

		pDATFile = malloc(nDATSize);
		FileRead( fHandle, pDATFile, nDATSize );
		FileClose(fHandle);
	}

	if( pPNGFile && pDATFile )
		gsFont = gsLib_font_create( pPNGFile, nPNGSize, pDATFile, nDATSize );

	if( pPNGFile )
		free(pPNGFile);

	if( pDATFile )
		free(pDATFile);

	// if failed to load font file use font compiled into elf
	if( !gsFont ) {
		if( !(gsFont = gsLib_font_create( boot_font, size_boot_font, boot_dat, size_boot_dat )) )
		{
			printf("Bootscreen_Init: Couldn't create font\n");
			SleepThread();
		}
	}

	strcpy( strPath, GetElfPath() );
	strcat( strPath, "boot/bkg.png" );

	fHandle = FileOpen( strPath, O_RDONLY );
	if( fHandle.fh >= 0 )
	{
		pngData *pPng;
		u8		*pRGB;
		int		nPSM;

		size = FileSeek( fHandle, 0, SEEK_END );
		FileSeek( fHandle, 0, SEEK_SET );

		data = malloc(size);
		FileRead( fHandle, data, size );
		FileClose(fHandle);

		if( (pPng = pngOpenRAW( data, size )) ) {
			pRGB = malloc( pPng->width * pPng->height * (pPng->bpp >> 3) );

			if( pngReadImage( pPng, pRGB ) != -1 ) {
				nPSM = pPng->bpp == 32 ? GS_PSM_CT32 : GS_PSM_CT24;

				gsTexBkg = gsLib_texture_raw( pPng->width, pPng->height, nPSM, pRGB, GS_CLUT_NONE,
											  NULL );
			}

			pngClose(pPng);
			free( pRGB );
		}
	}

	// if failed to load background use png compiled into elf
	if( !gsTexBkg ) {
		if( (pPng = pngOpenRAW( boot_bkg, size_boot_bkg )) ) {
			pRGB = malloc( pPng->width * pPng->height * (pPng->bpp >> 3) );

			if( pngReadImage( pPng, pRGB ) != -1 ) {
				nPSM = pPng->bpp == 32 ? GS_PSM_CT32 : GS_PSM_CT24;

				gsTexBkg = gsLib_texture_raw( pPng->width, pPng->height, nPSM, pRGB, GS_CLUT_NONE,
											  NULL );
			}

			pngClose(pPng);
			free( pRGB );
		}
	}

	Bootscreen_Clear();
}

//
// Bootscreen_Shutdown
//

void Bootscreen_Shutdown( void )
{
	gsLib_font_destroy( gsFont );
}

//
// Bootscreen_GetX
//

int Bootscreen_GetX( void )
{
	return bootscreen_x;
}

//
// Bootscreen_GetY
//

int Bootscreen_GetY( void )
{
	return bootscreen_y;
}

//
// Bootscreen_SetX
//

void Bootscreen_SetX( int x )
{
	if( x < MARGIN_X_LEFT || x >= MARGIN_X_RIGHT )
		return;

	bootscreen_x = x;
}

//
// Bootscreen_SetY
//

void Bootscreen_SetY( int y )
{
	if( y < MARGIN_Y_TOP || y >= MARGIN_Y_BOTTOM )
		return;

	bootscreen_y = y;
}
