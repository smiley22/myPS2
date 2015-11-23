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

void GUI_Ctrl_Slider_Draw( const GUIControl_t *pCtrl )
{
	const GUIMenuImage_t *pBkgImg, *pNibImg;
	int nIdx, nNibPosX, nNibPosY;
	GUICtrl_Slider_t *pSlider = pCtrl->pCtrl;

	if( pSlider->nTextureNib == -1 || pSlider->nTextureNibFocus == -1 )
		return;

	if( pSlider->nTextureBg != -1 )
	{
		pBkgImg = GUI_MenuGetImage( pCtrl->pParent, pSlider->nTextureBg );

		if( pBkgImg )
		{
			gsLib_prim_sprite_texture(	pBkgImg->gsTexture, pCtrl->nPosX, pCtrl->nPosY,
										pSlider->nWidth, pSlider->nHeight, 0, 0,
										pBkgImg->gsTexture->Width, pBkgImg->gsTexture->Height,
										colorTex );
		}
	}

	nIdx =	pCtrl->pParent->iSelID == pCtrl->nID ? pSlider->nTextureNibFocus :
			pSlider->nTextureNib;

	pNibImg = GUI_MenuGetImage( pCtrl->pParent, nIdx );

	if( !pNibImg )
		return;

	nNibPosX =	pCtrl->nPosX + 
				((float) fabsf( pSlider->nPos - pSlider->nMins ) / (pSlider->nMaxs -
				pSlider->nMins)) * pSlider->nWidth -
				(pNibImg->gsTexture->Width / 2);

	nNibPosY =	pCtrl->nPosY +
				(pSlider->nHeight - pNibImg->gsTexture->Height) / 2;

	gsLib_prim_sprite_texture(	pNibImg->gsTexture, nNibPosX, nNibPosY,
								pNibImg->gsTexture->Width, pNibImg->gsTexture->Height,
								0, 0, pNibImg->gsTexture->Width, pNibImg->gsTexture->Height,
								colorTex );
}

void GUI_Ctrl_Slider_SetPos( GUIControl_t *pCtrl, s64 nPos )
{
	GUICtrl_Slider_t *pSlider = pCtrl->pCtrl;

	if( nPos < pSlider->nMins )
		nPos = pSlider->nMins;
	
	if( nPos > pSlider->nMaxs )
		nPos = pSlider->nMaxs;

	pSlider->nPos = nPos;
}

s64 GUI_Ctrl_Slider_GetPos( const GUIControl_t *pCtrl )
{
	GUICtrl_Slider_t *pSlider = pCtrl->pCtrl;

	return pSlider->nPos;
}

void GUI_Ctrl_Slider_SetBounds( GUIControl_t *pCtrl, s64 nMins, s64 nMaxs )
{
	GUICtrl_Slider_t *pSlider = pCtrl->pCtrl;

	pSlider->nMins	= nMins;
	pSlider->nMaxs	= nMaxs;
}

void GUI_Ctrl_Slider_SetStep( GUIControl_t *pCtrl, u64 nStepSize )
{
	GUICtrl_Slider_t *pSlider = pCtrl->pCtrl;

	if( nStepSize > pSlider->nMaxs )
		nStepSize = pSlider->nMaxs;

	pSlider->nStepSize = nStepSize;
}

u64 GUI_Ctrl_Slider_GetStep( const GUIControl_t *pCtrl )
{
	GUICtrl_Slider_t *pSlider = pCtrl->pCtrl;

	return pSlider->nStepSize;
}

int GUI_Ctrl_Slider_Input( GUIControl_t *pCtrl, unsigned int nPadBtns  )
{
	GUICtrl_Slider_t *pSlider = pCtrl->pCtrl;

	static	u64 nDelayTimer;
	static	u64 nLastInput;
	s64		nPos;

	// if no input for over 250 msecs reset press mode
	if( ps2time_time_msec(NULL) > nLastInput + 250 )
		GP_SetPressMode(0);

	nLastInput = ps2time_time_msec(NULL);

	// L1 and R1 set the indicator to the min or max value
	if( nPadBtns & PAD_L1 )
	{
		if( GUI_Ctrl_Slider_GetPos(pCtrl) > pSlider->nMins )
		{
			GUI_Ctrl_Slider_SetPos(pCtrl, pSlider->nMins);
			GUI_Render();

			pCtrl->pParent->pfnCallback(	pCtrl->pParent, GUI_MSG_CONTROL,
											MAKEPARAM( pCtrl->nID, GUI_NOT_SL_POS_CHANGED ), 0 );
			return 1;
		}
	}
	else if( nPadBtns & PAD_R1 )
	{
		if( GUI_Ctrl_Slider_GetPos(pCtrl) < pSlider->nMaxs )
		{
			GUI_Ctrl_Slider_SetPos(pCtrl, pSlider->nMaxs);
			GUI_Render();

			pCtrl->pParent->pfnCallback(	pCtrl->pParent, GUI_MSG_CONTROL,
											MAKEPARAM( pCtrl->nID, GUI_NOT_SL_POS_CHANGED ), 0 );
			return 1;
		}
	}

	// delay input a little so user can finetune slider value
	if( (nDelayTimer > ps2time_time_msec(NULL)) && (GP_GetPressMode()) )
		return 1;

	if( GP_GetPressMode() == 0 )
	{
		nDelayTimer = ps2time_time_msec(NULL) + 500;
		GP_SetPressMode(1);
	}

	if( nPadBtns & PAD_RIGHT )
	{
		nPos = GUI_Ctrl_Slider_GetPos(pCtrl);

		if( nPos < pSlider->nMaxs )
		{
			nPos += GUI_Ctrl_Slider_GetStep(pCtrl);

			GUI_Ctrl_Slider_SetPos( pCtrl, nPos );
			GUI_Render();

			pCtrl->pParent->pfnCallback(	pCtrl->pParent, GUI_MSG_CONTROL,
											MAKEPARAM( pCtrl->nID, GUI_NOT_SL_POS_CHANGED ), 0 );
		}
		return 1;
	}
	else if( nPadBtns & PAD_LEFT )
	{
		nPos = GUI_Ctrl_Slider_GetPos(pCtrl);

		if( nPos > pSlider->nMins )
		{
			nPos -= GUI_Ctrl_Slider_GetStep(pCtrl);

			GUI_Ctrl_Slider_SetPos( pCtrl, nPos );
			GUI_Render();

			pCtrl->pParent->pfnCallback(	pCtrl->pParent, GUI_MSG_CONTROL,
											MAKEPARAM( pCtrl->nID, GUI_NOT_SL_POS_CHANGED ), 0 );
		}
		return 1;
	}

	return 0;
}
