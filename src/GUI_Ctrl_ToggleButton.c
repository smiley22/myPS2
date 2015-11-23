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

// pretty much the same as in GUI_Ctrl_Button
#include <GUI.h>
#include <GUI_Ctrl.h>

void GUI_Ctrl_ToggleButton_Draw( const GUIControl_t *pCtrl )
{
	const GUIMenuImage_t	*pImgHandle;
	const GUIMenuFont_t		*pFont;
	unsigned int			nIdx;
	GUICtrl_ToggleButton_t	*pButton = pCtrl->pCtrl;
	u64						uTexColor;
	u32						nTexPosX, nTexPosY, nTexWidth, nTexHeight;
	const char				*pLangStr;
	char					szDisplayStr[MAX_PATH + 1];
	int						nStrIndex;
	u32						nLabelPosX, nLabelPosY;
	u32						nLabelWidth, nLabelHeight;
	u64						uLabelColor;

	nLabelPosX		= pCtrl->nPosX;
	nLabelPosY		= pCtrl->nPosY;
	nLabelWidth		= pButton->nWidth;
	nLabelHeight	= pButton->nHeight;

	// button has images assigned
	if( pButton->nTexture    != -1 && pButton->nTextureFocus    != -1 &&
		pButton->nAltTexture != -1 && pButton->nAltTextureFocus != -1 )
	{
		if( pButton->nState == GUI_BUTTON_ON )
		{
			nIdx =	pCtrl->pParent->iSelID == pCtrl->nID ?
					pButton->nAltTextureFocus : pButton->nAltTexture;
		}
		else
		{
			nIdx =	pCtrl->pParent->iSelID == pCtrl->nID ?
					pButton->nTextureFocus : pButton->nTexture;
		}

		pImgHandle = GUI_MenuGetImage(	pCtrl->pParent, nIdx );

		if( !pImgHandle )
			return;

		uTexColor = pButton->nColorDiffuse ? pButton->nColorDiffuse : colorTex;

		nTexPosX	= pCtrl->nPosX;
		nTexPosY	= pCtrl->nPosY;
		nTexWidth	= pButton->nWidth;
		nTexHeight	= pButton->nHeight;

		if( pButton->nTexWidth && pButton->nTexHeight )
		{
			nTexWidth	= pButton->nTexWidth;
			nTexHeight	= pButton->nTexHeight;

			if( pButton->nTexAlign & GUI_BUTTON_TEX_RIGHT )
			{
				nTexPosX += (pButton->nWidth - nTexWidth);
				nLabelWidth -= nTexWidth;
			}
			else if( pButton->nTexAlign & GUI_BUTTON_TEX_LEFT )
			{
				nLabelPosX	+= nTexWidth;
				nLabelWidth	-= nTexWidth;
			}
			else
			{
				// GUI_BUTTON_TEX_CENTER is default
				nTexPosX += (pButton->nWidth - nTexWidth) / 2;
			}

			nTexPosY += (pButton->nHeight - nTexHeight) / 2;
		}


		gsLib_prim_sprite_texture(	pImgHandle->gsTexture, nTexPosX, nTexPosY,
									nTexWidth, nTexHeight, 0, 0, pImgHandle->gsTexture->Width,
									pImgHandle->gsTexture->Height, uTexColor );
	}

	// button has a string label assigned
	if( pButton->lpStr )
	{
		if( pButton->nFontIdx == -1 )
			return;

		if( !(pFont = GUI_MenuGetFont( pCtrl->pParent, pButton->nFontIdx )) )
			return;

		pLangStr = pButton->lpStr;
		CharsetConvert_UTF8ToCharset( szDisplayStr, pLangStr, sizeof(szDisplayStr) );

		nStrIndex = strlen(szDisplayStr) - 1;

		while( gsLib_font_width( pFont->gsFont, szDisplayStr ) > (	nLabelWidth -
																	pButton->nTextOffsetX) )
		{
			szDisplayStr[ nStrIndex ] = 0;

			if( nStrIndex > 1 )
			{
				szDisplayStr[ nStrIndex - 1 ] = '.';
				szDisplayStr[ nStrIndex - 2 ] = '.';
			}

			nStrIndex--;
		}

		uLabelColor =	pCtrl->nDisabled ? pButton->nDisabledColor :
						pButton->nTextColor;

		if( pCtrl->pParent->iSelID == pCtrl->nID )
			uLabelColor = pButton->nSelectedColor;

		// left align by default
		if( (!pButton->nAlign) || (pButton->nAlign & GUI_LABEL_LEFT) )
		{
			nLabelPosX += pButton->nTextOffsetX;
		}
		else if( pButton->nAlign & GUI_LABEL_RIGHT )
		{
			nLabelPosX += nLabelWidth - gsLib_font_width( pFont->gsFont, szDisplayStr ) -
						  pButton->nTextOffsetX;	
		}
		else if( pButton->nAlign & GUI_LABEL_CENTER )
		{
			nLabelPosX += (nLabelWidth - gsLib_font_width( pFont->gsFont, szDisplayStr )) / 2;
		}

		if( !(pButton->nAlignY & GUI_LABEL_VALIGN_TOP) )
		{
			// align middle vertically
			nLabelPosY += (nLabelHeight - gsLib_font_height(pFont->gsFont)) / 2;
		}
		else
		{
			nLabelPosY += pButton->nTextOffsetY;
		}

		gsLib_font_print( pFont->gsFont, nLabelPosX, nLabelPosY, uLabelColor, szDisplayStr );
	}
}

int GUI_Ctrl_ToggleButton_Input( GUIControl_t *pCtrl, unsigned int nPadBtns )
{
	GUICtrl_ToggleButton_t *pButton = pCtrl->pCtrl;

	if( nPadBtns & PAD_CROSS )
	{
		if( pButton->nState == GUI_BUTTON_ON )
			pButton->nState = GUI_BUTTON_OFF;
		else
			pButton->nState = GUI_BUTTON_ON;

		pCtrl->pParent->pfnCallback(	pCtrl->pParent, GUI_MSG_CONTROL,
										MAKEPARAM( pCtrl->nID, GUI_NOT_BUTTON_TOGGLED ),
										pButton->nState );

		GUI_Render();
		return 1;
	}

	return 0;
}

void GUI_Ctrl_ToggleButton_SetState( GUIControl_t *pCtrl, unsigned int bState )
{
	GUICtrl_ToggleButton_t *pButton = pCtrl->pCtrl;

	pButton->nState = bState;
}

int GUI_Ctrl_ToggleButton_GetState( const GUIControl_t *pCtrl )
{
	GUICtrl_ToggleButton_t *pButton = pCtrl->pCtrl;

	return pButton->nState;
}
