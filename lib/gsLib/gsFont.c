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

#include "gsLib.h"

// not used at the moment
void gsLib_font_init( void )
{
	u8	*Clut;
	u32 i;

	Clut = memalign( 128, 16 * 4 );

	for( i = 0; i < 16; i++ )
	{
		Clut[ i * 4 + 0 ] = 0xFF;
		Clut[ i * 4 + 1 ] = 0xFF;
		Clut[ i * 4 + 2 ] = 0xFF;
		Clut[ i * 4 + 3 ] = (u8)i * 8;
	}

	gsGlobal.FontClut = gsLib_texture_raw( 8, 2, GS_PSM_CT32, Clut, GS_CLUT_PALLETE, NULL );

	free(Clut);
}

GSTEXTURE *gsLib_font_clut( void )
{
	u8	*Clut;
	u32 i;
	GSTEXTURE *t;

	Clut = memalign( 128, 16 * 4 );

	for( i = 0; i < 16; i++ )
	{
		Clut[ i * 4 + 0 ] = 0xFF;
		Clut[ i * 4 + 1 ] = 0xFF;
		Clut[ i * 4 + 2 ] = 0xFF;
		Clut[ i * 4 + 3 ] = (u8)i * 8;
	}

	t = gsLib_texture_raw( 8, 2, GS_PSM_CT32, Clut, GS_CLUT_PALLETE, NULL );

	free(Clut);

	return t;
}

GSFONT *gsLib_font_create( u8 *png_file, u32 png_size, u8 *dat_file, u32 dat_size )
{
	GSFONT		*gsFont;
	u8			*rgba, CharWidth, CharHeight, *alpha;
	int			i, c;
	pngData		*pPng;
	u32			nImageWidth, nImageHeight;
	int			x0, y0, x1, y1;
	u32			*charWidths;
	u8			val1, val2;
	GSTEXTURE	*clut;

	if( !(pPng = pngOpenRAW( png_file, png_size )) )
		return NULL;

	if( pPng->bpp != 32 )
		return NULL;

	if( !(rgba = malloc( pPng->width * pPng->height * (pPng->bpp >> 3) )) )
		return NULL;

	if( pngReadImage( pPng, rgba ) < 0 )
		return NULL;

	nImageWidth		= pPng->width;
	nImageHeight	= pPng->height;

	pngClose(pPng);

	alpha		= memalign( 128, nImageWidth * nImageHeight / 2 );

	// 2 clut indices per byte
	for( i = 0, c = 0; i < nImageWidth * nImageHeight; i += 2, c++ )
	{
		val1 = rgba[ (i + 0) * 4 + 3 ] / 9;
		val2 = rgba[ (i + 1) * 4 + 3 ] / 9;

		alpha[c] = ((val2 & 15) << 4) | (val1 & 15);
	}

	CharWidth	= nImageWidth  / 16;
	CharHeight	= nImageHeight / 16;

	gsFont		= calloc(1, sizeof(GSFONT) );
	clut		= gsLib_font_clut();

	gsFont->Texture	= gsLib_texture_raw( nImageWidth, nImageHeight, GS_PSM_T4, alpha,
										 GS_CLUT_TEXTURE, /*gsGlobal.FontClut*/clut );
	if( !gsFont->Texture )
		return NULL;

	free(alpha);
	free(rgba);

	// charwidths auslesen
	charWidths = malloc( sizeof(u32) * 256 );
	for( i = 0; i < 256; i++ )
		charWidths[i] = dat_file[ i * 2 ];

	gsFont->TexCoords = calloc( 1, sizeof(u32) * 4 * 256 );

	x0 = x1 = y1 = 0;
	y0 = -(nImageWidth / 16);

	for( i = 0; i < 256; i++ )
	{
		x0 %= nImageWidth;
		x1 = x0 + charWidths[i];

		if( (i % 16) == 0 )
		{
			y0 += (nImageWidth / 16);
			y1 = y0 + (nImageWidth / 16) - 1;
		}

		gsFont->TexCoords[ i * 4 + 0 ] = x0;
		gsFont->TexCoords[ i * 4 + 1 ] = y0;
		gsFont->TexCoords[ i * 4 + 2 ] = x1;
		gsFont->TexCoords[ i * 4 + 3 ] = y1;

		x0 += (nImageWidth / 16);
	}

	free( charWidths );

	return gsFont;
}

int gsLib_font_destroy( GSFONT *gsFont )
{
	gsLib_texture_free(gsFont->Texture->Clut);
	gsLib_texture_free(gsFont->Texture);

	free(gsFont->TexCoords);
	free(gsFont);

	return 1;
}

void gsLib_font_print( GSFONT *gsFont, u32 x, u32 y, u64 color, const char *string )
{
	int l, i, h, cx, cy;
	int c;

	cx	= x;
	cy	= y;
	l	= strlen(string);

	for( i = 0; i < l; i++ )
	{
		c = (unsigned char) string[i];

		if( c == '\n' )
		{
			h	=	gsFont->TexCoords[ '0' * 4 + 3 ] -
					gsFont->TexCoords[ '0' * 4 + 1 ] + 1;

			cx	= x;
			cy	= cy + h;
		}
		else
		{
			cx += gsLib_font_print_char( gsFont, cx, cy, color, c );
		}
	}
}

int gsLib_font_print_char( GSFONT *gsFont, u32 x, u32 y, u64 color, u32 c )
{
	u32 *tc;
	int x0, x1, y0, y1, w, h;

	w	= 0;
	tc	= &gsFont->TexCoords[ c * 4 ];

	x0	= *tc++;
	y0	= *tc++;
	x1	= *tc++;
	y1	= *tc++;
	w	= (x1 - x0 + 1);
	h	= (y1 - y0 + 1);

	gsLib_prim_sprite_texture(	gsFont->Texture, x, y, w, h, x0, y0,
								x1 + 1, y1 + 1, color );

	return w;
}

int gsLib_font_width( GSFONT *gsFont, const char *string )
{
	u32 *tc;
	int l, i, x0, x1, y0, y1, w, ret;
	int c;

	l	= strlen(string);
	ret	= 0;

	for( i = 0; i < l; i++ )
	{
		c = (unsigned char)string[i];

		tc	= &gsFont->TexCoords[ c * 4 ];
		x0	= *tc++;
		y0	= *tc++;
		x1	= *tc++;
		y1	= *tc++;
		w	= x1 - x0 + 1;

		ret += w;
	}

	return ret;
}

int gsLib_font_height( GSFONT *gsFont )
{
	int h;

	h = gsFont->TexCoords[ '0' * 4 + 3 ] - gsFont->TexCoords[ '0' * 4 + 1 ] + 1;
	return h;
}
