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

void GUI_Ctrl_Combo_Draw( const GUIControl_t *pCtrl )
{
	const GUIMenuImage_t	*pImgHandle;
	const GUIMenuFont_t		*pFont;
	unsigned int			nIdx;
	GUICtrl_Combo_t			*pButton = pCtrl->pCtrl;
	GUIComboItem_t			*pItem;
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
	if( pButton->nTexture != -1 && pButton->nTextureFocus != -1 )
	{
		nIdx =	pCtrl->pParent->iSelID == pCtrl->nID ?
				pButton->nTextureFocus : pButton->nTexture;

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

	if( pButton->nNumItems > 0 )
	{
		pItem = &pButton->pItems[ pButton->nSelectedItem ];

		if( pButton->nFontIdx == -1 )
			return;

		if( !(pFont = GUI_MenuGetFont( pCtrl->pParent, pButton->nFontIdx )) )
			return;

		if( !pItem->pStr )
			return;

		pLangStr = pItem->pStr;
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

int GUI_Ctrl_Combo_Input( GUIControl_t *pCtrl, unsigned int nPadBtns )
{
	GUICtrl_Combo_t *pButton = pCtrl->pCtrl;

	if( nPadBtns & PAD_CROSS )
	{
		pButton->nSelectedItem++;

		if( pButton->nSelectedItem >= pButton->nNumItems )
			pButton->nSelectedItem = 0;

		pCtrl->pParent->pfnCallback(	pCtrl->pParent, GUI_MSG_CONTROL,
										MAKEPARAM( pCtrl->nID, GUI_NOT_CB_SEL_CHANGED ),
										pButton->nSelectedItem );

		GUI_Render();
		return 1;
	}

	return 0;
}

void GUI_Ctrl_Combo_Add( GUIControl_t *pCtrl, const char *pStr, unsigned int nValue )
{
	GUICtrl_Combo_t *pButton = pCtrl->pCtrl;
	GUIComboItem_t *pItems;

	pItems = malloc( sizeof(GUIComboItem_t) * (pButton->nNumItems + 1) );

	if(!pItems)
	{
		pButton->nNumItems = 0;
		return;
	}

	if( pButton->pItems )
	{
		memcpy( pItems, pButton->pItems, pButton->nNumItems * sizeof(GUIComboItem_t) );
		free( pButton->pItems );
	}

	pButton->pItems	= pItems;
	pItems			= &pButton->pItems[ pButton->nNumItems ];

	pItems->nValue	= nValue;
	pItems->pStr	= malloc( strlen(pStr) + 1 );

	if( !pItems->pStr )
		return;

	strcpy( pItems->pStr, pStr );
	pButton->nNumItems++;
}

void GUI_Ctrl_Combo_Clean( GUIControl_t *pCtrl )
{
	GUICtrl_Combo_t *pButton = pCtrl->pCtrl;
	unsigned int i;

	for( i = 0; i < pButton->nNumItems; i++ )
	{
		if( pButton->pItems[i].pStr )
			free(pButton->pItems[i].pStr);
	}

	if( pButton->pItems )
		free(pButton->pItems);

	pButton->pItems			= NULL;
	pButton->nNumItems		= 0;
	pButton->nSelectedItem	= 0;
}

int GUI_Ctrl_Combo_Empty( const GUIControl_t *pCtrl )
{
	GUICtrl_Combo_t *pButton = pCtrl->pCtrl;

	return (pButton->nNumItems == 0);
}

void GUI_Ctrl_Combo_SetCurSel( GUIControl_t *pCtrl, unsigned int nIndex )
{
	GUICtrl_Combo_t *pButton = pCtrl->pCtrl;

	if( nIndex >= pButton->nNumItems )
		return;

	pButton->nSelectedItem = nIndex;
}
