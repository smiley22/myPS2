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

void GUI_Ctrl_Progress_Draw( const GUIControl_t *pCtrl )
{
	const GUIMenuImage_t	*pBkgImg, *pLeftImg, *pRightImg,
							*pMidImg;
	GUICtrl_Progress_t		*pProgress = pCtrl->pCtrl;
	double					f;
	unsigned int			nMidWidth, nBarWidth;

	if( pProgress->nTextureLeft == -1 || pProgress->nTextureMid == - 1 ||
		pProgress->nTextureRight == - 1 )
		return;

	if( pProgress->nTextureBg != -1 )
	{
		pBkgImg = GUI_MenuGetImage( pCtrl->pParent, pProgress->nTextureBg );

		if( pBkgImg )
		{
			gsLib_prim_sprite_texture(	pBkgImg->gsTexture, pCtrl->nPosX, pCtrl->nPosY,
										pProgress->nWidth, pProgress->nHeight, 0, 0,
										pBkgImg->gsTexture->Width, pBkgImg->gsTexture->Height,
										colorTex );
		}
	}

	f = (double) pProgress->nPos / (pProgress->nMaxs - pProgress->nMins);

	if( f > 0.0 )
	{
		pLeftImg = GUI_MenuGetImage( pCtrl->pParent, pProgress->nTextureLeft );

		if( !pLeftImg )
			return;

		gsLib_prim_sprite_texture(	pLeftImg->gsTexture, pCtrl->nPosX, pCtrl->nPosY,
									pLeftImg->gsTexture->Width, pProgress->nHeight,
									0, 0, pLeftImg->gsTexture->Width, pLeftImg->gsTexture->Height,
									colorTex );

		pRightImg = GUI_MenuGetImage( pCtrl->pParent, pProgress->nTextureRight );

		if( !pRightImg )
			return;
		
		nMidWidth = pProgress->nWidth - pLeftImg->gsTexture->Width -
					pRightImg->gsTexture->Width;

		nBarWidth = nMidWidth * f;

		pMidImg = GUI_MenuGetImage( pCtrl->pParent, pProgress->nTextureMid );

		if( !pMidImg )
			return;

		gsLib_prim_sprite_texture(	pMidImg->gsTexture,
									pCtrl->nPosX + pLeftImg->gsTexture->Width,
									pCtrl->nPosY, nBarWidth, pProgress->nHeight,
									0, 0, pMidImg->gsTexture->Width * f,
									pMidImg->gsTexture->Height, colorTex );

		if( f == 1.0 )
		{
			gsLib_prim_sprite_texture(	pRightImg->gsTexture,
										pCtrl->nPosX + pLeftImg->gsTexture->Width + nBarWidth,
										pCtrl->nPosY, pRightImg->gsTexture->Width,
										pProgress->nHeight, 0, 0, pRightImg->gsTexture->Width,
										pRightImg->gsTexture->Height, colorTex );
		}
	}
}

void GUI_Ctrl_Progress_SetBounds( GUIControl_t *pCtrl, u64 nMins, u64 nMaxs )
{
	GUICtrl_Progress_t *pProgress = pCtrl->pCtrl;

	pProgress->nMins = nMins;
	pProgress->nMaxs = nMaxs;
}

void GUI_Ctrl_Progress_GetBounds( const GUIControl_t *pCtrl, u64 *pMins, u64 *pMaxs )
{
	GUICtrl_Progress_t *pProgress = pCtrl->pCtrl;

	*pMins = pProgress->nMins;
	*pMaxs = pProgress->nMaxs;
}

void GUI_Ctrl_Progress_SetPos( GUIControl_t *pCtrl, u64 nPos )
{
	GUICtrl_Progress_t *pProgress = pCtrl->pCtrl;

	if( nPos < pProgress->nMins )
		nPos = pProgress->nMins;

	if( nPos > pProgress->nMaxs )
		nPos = pProgress->nMaxs;

	pProgress->nPos = nPos;
}

u64 GUI_Ctrl_Progress_GetPos( const GUIControl_t *pCtrl )
{
	GUICtrl_Progress_t *pProgress = pCtrl->pCtrl;

	return pProgress->nPos;
}
