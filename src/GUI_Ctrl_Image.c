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

void GUI_Ctrl_Image_Draw( const GUIControl_t *pCtrl )
{
	const GUIMenuImage_t	*pImgHandle;
	GUICtrl_Image_t			*pImg = pCtrl->pCtrl;
	u64						uTexColor;
	u32						nPosX, nPosY, nWidth, nHeight;

	if( pImg->nTexture == -1 )
		return;

	pImgHandle = GUI_MenuGetImage( pCtrl->pParent, pImg->nTexture );

	if( !pImgHandle )
		return;

	uTexColor = pImg->nColorDiffuse ? pImg->nColorDiffuse : colorTex;

	nPosX	= pCtrl->nPosX;
	nPosY	= pCtrl->nPosY;
	nWidth	= pImg->nWidth;
	nHeight	= pImg->nHeight;

	if( pImg->nAspectRatio )
	{
		if( pImgHandle->gsTexture->Width > pImgHandle->gsTexture->Height )
		{
			nHeight = nWidth * ((float) pImgHandle->gsTexture->Height /
								(float) pImgHandle->gsTexture->Width );

			nPosY += (pImg->nHeight - nHeight) / 2;
		}
		else
		{
			nWidth = nHeight * ((float) pImgHandle->gsTexture->Width /
								(float) pImgHandle->gsTexture->Height );

			nPosX += (pImg->nWidth - nWidth) / 2;
		}
	}

	gsLib_prim_sprite_texture(	pImgHandle->gsTexture, nPosX, nPosY,
								nWidth, nHeight, 0, 0, pImgHandle->gsTexture->Width,
								pImgHandle->gsTexture->Height, uTexColor );
}
