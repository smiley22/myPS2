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

#include <GUI.h>
#include <GUI_Ctrl.h>
#include <Imgscale.h>

static GUIMenu_t		*lpMenu;
static char				szImagePath[MAX_PATH + 1];
static GSTEXTURE		*gsTexture;
static float			fZoomStep;
static float			fOffsetX, fOffsetY;
static unsigned int		nImagePosX, nImagePosY;
static unsigned int		nImageWidth, nImageHeight;

void GUI_OpenViewMenu( const char *lpImageName )
{
	strncpy( szImagePath, lpImageName, MAX_PATH );
	szImagePath[MAX_PATH] = 0;

	GUI_OpenMenu( GUI_MENU_VIEW );
}

GSTEXTURE *LoadImage( const char *lpImageName )
{
	FHANDLE		fHandle;
	int			nSize, nRet;
	u8			*pBuf, *pImgData, *pResData;
	jpgData		*pJpg;
	int			nWidth, nHeight;
	float		fRatio;

	if( gsTexture )
	{
		gsLib_texture_free(gsTexture);
		gsTexture = NULL;
	}

	fHandle = FileOpen( lpImageName, O_RDONLY );
	if( fHandle.fh < 0 )
		return NULL;

	nSize = FileSeek( fHandle, 0, SEEK_END );
	FileSeek( fHandle, 0, SEEK_SET );

	if( !nSize )
		return NULL;

	if( !(pBuf = malloc( nSize )) )
		return NULL;

	FileRead( fHandle, pBuf, nSize );
	FileClose(fHandle);

	if( !(pJpg = jpgOpenRAW( pBuf, nSize, JPG_WIDTH_FIX )) )
	{
		free(pBuf);
		return NULL;
	}

	pImgData = malloc( ((3 * pJpg->width + 3) & ~3) * pJpg->height );
	if( !pImgData )
	{
		jpgClose(pJpg);
		free(pBuf);
		return NULL;
	}

	if( -1 == jpgReadImage( pJpg, pImgData ) )
	{
		jpgClose(pJpg);
		free(pBuf);
		free(pImgData);
		return NULL;
	}

	// resize image
	if( pJpg->width > pJpg->height )
	{
		fRatio	= (float) pJpg->height / pJpg->width;

		// make sure width is a multiple of 4
		nWidth = pJpg->width - (pJpg->width % 4);

		if( nWidth > 400 )
			nWidth = 400;

		nHeight	= (int) (nWidth * fRatio);
		nHeight -= (nHeight %2);

	} 
	else {
		fRatio	= (float) pJpg->width / pJpg->height;
	
		nHeight = pJpg->height - (pJpg->height % 2);

		if( nHeight > 300 )
			nHeight = 300;

		nWidth	= (int) (nHeight * fRatio);
		nWidth -= (nWidth % 4);
	}

	nRet = ScaleBitmap( pImgData, pJpg->width, pJpg->height, &pResData, nWidth, nHeight );
	if( 0 == nRet )
	{
		jpgClose(pJpg);
		free(pBuf);
		free(pImgData);
		return NULL;
	}

	gsTexture = gsLib_texture_raw( nWidth, nHeight, GS_PSM_CT24, pResData, GS_CLUT_NONE, NULL );

	jpgClose(pJpg);
	free(pBuf);
	free(pImgData);
	free(pResData);

	return gsTexture;
}

static void GUI_View_Render( void )
{
	unsigned int nOffsetX = 0;
	unsigned int nOffsetY = 0;
	unsigned int nScrWidth, nScrHeight;
	float fMul;

	nScrWidth	= gsLib_get_width();
	nScrHeight	= gsLib_get_height();

	// 640 / 400 = 1.6
	fMul = 1.6f;

	nImagePosX		= (nScrWidth - gsTexture->Width * fMul * fZoomStep) / 2;
	nImagePosY		= (nScrHeight - gsTexture->Height * fMul * fZoomStep) / 2;

	nImageWidth		= gsTexture->Width * fMul * fZoomStep;
	nImageHeight	= gsTexture->Height * fMul * fZoomStep;

	if( nImageWidth > nScrWidth )
		nOffsetX = fOffsetX * ( nImageWidth - nScrWidth ) / 2;

	if( nImageHeight > nScrHeight )
		nOffsetY = fOffsetY * ( nImageHeight - nScrHeight ) / 2;

	nImagePosX += nOffsetX;
	nImagePosY += nOffsetY;

	gsLib_clear(colorBlack);
	gsLib_prim_sprite_texture(	gsTexture, nImagePosX, nImagePosY, nImageWidth, nImageHeight,
								0, 0, gsTexture->Width, gsTexture->Height, colorTex );
	gsLib_swap();
}

static void GUI_View_Input( unsigned int nPadBtns )
{
	if( nPadBtns & PAD_TRIANGLE )
	{
		if( gsTexture )
		{
			gsLib_texture_free(gsTexture);
			gsTexture = NULL;
		}

		GUI_OpenMenu( GUI_MENU_MYPICTURES );
		return;
	}
	else if( (nPadBtns & PAD_CIRCLE) || (nPadBtns & PAD_R1) )
	{
		if( fZoomStep < 2.0f )
		{
			fZoomStep += 0.01f;
			GUI_Render();
		}
	}
	else if( (nPadBtns & PAD_SQUARE) || (nPadBtns & PAD_L1) )
	{
		if( fZoomStep > 1.0f )
		{
			fZoomStep -= 0.01f;
			GUI_Render();
		}
	}
	else if( nPadBtns & PAD_LEFT )
	{
		// move image left if it does not fit on the screen
		if( nImageWidth > gsLib_get_width() )
		{
			if( fOffsetX < 1.0f )
			{
				fOffsetX += 0.05f;
				GUI_Render();
			}
		}
	}
	else if( nPadBtns & PAD_RIGHT )
	{
		// move image right if it does not fit on the screen
		if( nImageWidth > gsLib_get_width() )
		{
			if( fOffsetX > -1.0f )
			{
				fOffsetX -= 0.05f;
				GUI_Render();
			}
		}
	}
	else if( nPadBtns & PAD_DOWN )
	{
		// move image down if it does not fit on the screen
		if( nImageHeight > gsLib_get_height() )
		{
			if( fOffsetY > -1.0f )
			{
				fOffsetY -= 0.05;
				GUI_Render();
			}
		}

	}
	else if( nPadBtns & PAD_UP )
	{
		// move image up if it does not fit on the screen
		if( nImageHeight > gsLib_get_height() )
		{
			if( fOffsetY < 1.0f )
			{
				fOffsetY += 0.05f;
				GUI_Render();
			}
		}
	}
}

unsigned int GUI_CB_View( GUIMenu_t *lpGUIMenu, unsigned int nGUIMsg,
						  unsigned int nCtrlParam, unsigned int nOther )
{
	switch( nGUIMsg )
	{
		case GUI_MSG_OPEN:
			lpGUIMenu->pfnInput		= GUI_View_Input;
			lpGUIMenu->pfnRender	= GUI_View_Render;

			lpMenu		= lpGUIMenu;
			fZoomStep	= 1.0f;
			fOffsetX	= 0.0f;
			fOffsetY	= 0.0f;

			LoadImage( szImagePath );

			GP_SetPressMode(1);
			break;

		case GUI_MSG_CLOSE:
			GP_SetPressMode(0);
			break;
	}
	return 0;
}
