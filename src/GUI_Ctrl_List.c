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

void GUI_Ctrl_List_Draw( const GUIControl_t *pCtrl )
{
	unsigned int nNumItems, bHasFocus, bItemFocus;
	unsigned int i, nItemIdx, nItemPosY;
	u64 uTextColor, uTexColor;
	unsigned int nItemHeight, nTextPosY, nTextPosX, nStrIndex;
	const GUIMenuImage_t *pTexNoFocus, *pTexFocus, *pTexMarked,
		*pTex, *pScrollTex;
	const GUIMenuFont_t *pFont;
	char szStr[ MAX_PATH + 1 ];
	GUIListItem_t *pItem;
	GUICtrl_List_t *pList = pCtrl->pCtrl;

	if( pList->nTexture == -1 || pList->nTextureFocus == -1 ||
		pList->nTextureMarked == -1 || pList->nFontIdx == -1 )
		return;

	pTexNoFocus = GUI_MenuGetImage( pCtrl->pParent, pList->nTexture );
	if( !pTexNoFocus )
		return;

	pTexFocus = GUI_MenuGetImage( pCtrl->pParent, pList->nTextureFocus );
	if( !pTexFocus )
		return;

	pTexMarked = GUI_MenuGetImage( pCtrl->pParent, pList->nTextureMarked );
	if( !pTexMarked )
		return;

	pFont = GUI_MenuGetFont( pCtrl->pParent, pList->nFontIdx );
	if( !pFont )
		return;

	nItemHeight = pList->nItemHeight ? pList->nItemHeight : gsLib_font_height(pFont->gsFont);
	nNumItems	= pList->nHeight / (nItemHeight + pList->nPadding);
	bHasFocus	= pCtrl->pParent->iSelID == pCtrl->nID;
	nItemPosY	= pCtrl->nPosY;
	uTexColor	= pList->nColorDiffuse ? pList->nColorDiffuse : colorTex;

	for( i = 0; i < nNumItems; i++ )
	{
		nItemIdx = pList->nStartItem + i;

		if( nItemIdx >= pList->nNumItems )
			break;

		pItem = &pList->pItems[nItemIdx];
		bItemFocus = (bHasFocus) && (nItemIdx == pList->nSelectedItem);

		pTex = (pItem->nFlags & FLAG_MARKED) ? pTexMarked : pTexNoFocus;

		if( bItemFocus )
			pTex = pTexFocus;

		gsLib_prim_sprite_texture(	pTex->gsTexture, pCtrl->nPosX, nItemPosY, pList->nWidth,
									nItemHeight, 0, 0, pTex->gsTexture->Width,
									pTex->gsTexture->Height, uTexColor );

		uTextColor	= (pItem->nFlags & FLAG_MARKED) ? pList->nMarkedColor : pList->nTextColor;

		if( bItemFocus )
			uTextColor = pList->nSelectedColor;

		nTextPosX	= pCtrl->nPosX + pList->nTextOffset;
		nTextPosY	= nItemPosY + (nItemHeight - gsLib_font_height(pFont->gsFont)) / 2;

		CharsetConvert_UTF8ToCharset( szStr, pItem->pStr, sizeof(szStr) );
		nStrIndex	= strlen(szStr) - 1;

		while( gsLib_font_width( pFont->gsFont, szStr ) > (pList->nWidth - pList->nTextOffset * 2) )
		{
			szStr[ nStrIndex ] = 0;

			if( nStrIndex > 1 )
			{
				szStr[ nStrIndex - 1 ] = '.';
				szStr[ nStrIndex - 2 ] = '.';
			}

			nStrIndex--;
		}

		gsLib_font_print( pFont->gsFont, nTextPosX, nTextPosY, uTextColor, szStr );
		nItemPosY += nItemHeight + pList->nPadding;
	}

	if( pList->nNumItems > nNumItems )
	{
		if( pList->nStartItem > 0 )
		{
			pScrollTex = GUI_MenuGetImage( pCtrl->pParent, pList->nUpTexture );

			if( pScrollTex )
			{
				gsLib_prim_sprite_texture(	pScrollTex->gsTexture, pCtrl->nPosX + pList->nUpTexPosX,
											pCtrl->nPosY + pList->nUpTexPosY,
											pScrollTex->gsTexture->Width,
											pScrollTex->gsTexture->Height, 0, 0,
											pScrollTex->gsTexture->Width,
											pScrollTex->gsTexture->Height, colorTex );
			}
		}

		if( (pList->nStartItem + nNumItems) < pList->nNumItems )
		{
			pScrollTex = GUI_MenuGetImage( pCtrl->pParent, pList->nDownTexture );

			if( pScrollTex )
			{
				gsLib_prim_sprite_texture(	pScrollTex->gsTexture, pCtrl->nPosX +
											pList->nDownTexPosX,
											pCtrl->nPosY + pList->nDownTexPosY,
											pScrollTex->gsTexture->Width,
											pScrollTex->gsTexture->Height,
											0, 0, pScrollTex->gsTexture->Width,
											pScrollTex->gsTexture->Height, colorTex );
			}
		}
	}
}

int GUI_Ctrl_List_AddItem( GUIControl_t *pCtrl, const char *pStr, unsigned int nValue )
{
	GUICtrl_List_t *pList = pCtrl->pCtrl;
	GUIListItem_t *pItems;

	pItems = malloc( sizeof(GUIListItem_t) * (pList->nNumItems + 1) );	

	if(!pItems)
	{
		pList->nNumItems = 0;
		return -1;
	}

	if( pList->pItems )
	{
		memcpy( pItems, pList->pItems, pList->nNumItems * sizeof(GUIListItem_t) );
		free( pList->pItems );
	}

	pList->pItems	= pItems;
	pItems			= &pList->pItems[ pList->nNumItems ];

	pItems->nFlags	= 0;
	pItems->nValue	= nValue;
	pItems->pStr	= malloc( strlen(pStr) + 1 );

	if( !pItems->pStr )
		return -1;

	strcpy( pItems->pStr, pStr );
	pList->nNumItems++;

	return (pList->nNumItems - 1);
}

const char *GUI_Ctrl_List_GetItem( GUIControl_t *pCtrl, unsigned int nIndex )
{
	GUICtrl_List_t *pList = pCtrl->pCtrl;

	if( nIndex >= pList->nNumItems )
		return NULL;

	return pList->pItems[nIndex].pStr;
}

unsigned int GUI_Ctrl_List_GetItemData( GUIControl_t *pCtrl, unsigned int nIndex )
{
	GUICtrl_List_t *pList = pCtrl->pCtrl;

	if( nIndex >= pList->nNumItems )
		return 0;

	return pList->pItems[nIndex].nValue;
}

void GUI_Ctrl_List_Clean( GUIControl_t *pCtrl )
{
	unsigned int i;
	GUICtrl_List_t *pList = pCtrl->pCtrl;

	for( i = 0; i < pList->nNumItems; i++ )
	{
		if( pList->pItems[i].pStr )
			free(pList->pItems[i].pStr);
	}

	free(pList->pItems);

	pList->pItems			= NULL;
	pList->nNumItems		= 0;
	pList->nSelectedItem	= 0;
	pList->nStartItem		= 0;
}

void GUI_Ctrl_List_SetCursor( GUIControl_t *pCtrl, unsigned int nPos )
{
	unsigned int nItemHeight, nNumDraw, nOld;
	GUICtrl_List_t *pList = pCtrl->pCtrl;
	const GUIMenuFont_t *pFont;

	pFont = GUI_MenuGetFont( pCtrl->pParent, pList->nFontIdx );

	nItemHeight = pList->nItemHeight ? pList->nItemHeight : gsLib_font_height(pFont->gsFont);
	nNumDraw	= pList->nHeight / (nItemHeight + pList->nPadding);

	if( nPos < pList->nStartItem )
	{
		pList->nStartItem = nPos;
	}
	else if( nPos >= (pList->nStartItem + nNumDraw) )
	{
		pList->nStartItem = nPos - nNumDraw + 1;
	}

	nOld = pList->nSelectedItem;
	pList->nSelectedItem = nPos;

	if( nOld != pList->nSelectedItem )
	{
		pCtrl->pParent->pfnCallback(	pCtrl->pParent, GUI_MSG_CONTROL,
										MAKEPARAM( pCtrl->nID, GUI_NOT_LIST_POS ),
										pList->nSelectedItem );
	}
}

int GUI_Ctrl_List_Input( GUIControl_t *pCtrl, unsigned int nPadBtns )
{
	GUICtrl_List_t *pList = pCtrl->pCtrl;

	if( !pList->nNumItems )
		return 0;

	if( nPadBtns & PAD_DOWN )
	{
		if( pList->nSelectedItem < (pList->nNumItems - 1) )
		{
			GUI_Ctrl_List_SetCursor( pCtrl, pList->nSelectedItem + 1 );
			GUI_Render();
			return 1;
		}
	}
	else if( nPadBtns & PAD_UP )
	{
		if( pList->nSelectedItem > 0 )
		{
			GUI_Ctrl_List_SetCursor( pCtrl, pList->nSelectedItem - 1 );
			GUI_Render();
			return 1;
		}
	}
	else if( nPadBtns & PAD_L1 )
	{
		GUI_Ctrl_List_SetCursor( pCtrl, 0 );
		GUI_Render();
		return 1;
	}
	else if( nPadBtns & PAD_R1 )
	{
		GUI_Ctrl_List_SetCursor( pCtrl, pList->nNumItems - 1 );
		GUI_Render();
		return 1;
	}
	else if( nPadBtns & PAD_CROSS )
	{
		pCtrl->pParent->pfnCallback(	pCtrl->pParent, GUI_MSG_CONTROL,
										MAKEPARAM( pCtrl->nID, GUI_NOT_LIST_CLICK ),
										pList->nSelectedItem );
	}
	else if( nPadBtns & PAD_SQUARE )
	{
		if( pList->pItems[ pList->nSelectedItem ].nFlags & FLAG_MARKED )
		{
			pList->pItems[ pList->nSelectedItem ].nFlags &= ~FLAG_MARKED;
			
			pCtrl->pParent->pfnCallback(	pCtrl->pParent, GUI_MSG_CONTROL,
											MAKEPARAM( pCtrl->nID, GUI_NOT_LIST_UNMARK ),
											0 );
		}
		else
		{
			pList->pItems[ pList->nSelectedItem ].nFlags |= FLAG_MARKED;

			pCtrl->pParent->pfnCallback(	pCtrl->pParent, GUI_MSG_CONTROL,
											MAKEPARAM( pCtrl->nID, GUI_NOT_LIST_MARK ),
											0 );
		}

		return 1;
	}

	return 0;
}

unsigned int GUI_Ctrl_List_GetCount( const GUIControl_t *pCtrl )
{
	GUICtrl_List_t *pList = pCtrl->pCtrl;

	return pList->nNumItems;
}

const char *GUI_Ctrl_List_GetSel( const GUIControl_t *pCtrl )
{
	GUICtrl_List_t *pList = pCtrl->pCtrl;

	if( !pList->nNumItems )
		return NULL;

	return pList->pItems[ pList->nSelectedItem ].pStr;
}

unsigned int GUI_Ctrl_List_GetSelIndex( const GUIControl_t *pCtrl )
{
	GUICtrl_List_t *pList = pCtrl->pCtrl;

	return pList->nSelectedItem;
}

unsigned int GUI_Ctrl_List_NumMark( const GUIControl_t *pCtrl )
{
	unsigned int i, nNum;
	GUICtrl_List_t *pList = pCtrl->pCtrl;

	for( i = 0, nNum = 0; i < pList->nNumItems; i++ )
	{
		if( pList->pItems[i].nFlags & FLAG_MARKED )
			nNum++;
	}

	return nNum;
}

const char *GUI_Ctrl_List_GetMarked( const GUIControl_t *pCtrl, unsigned int n )
{
	unsigned int i, nNum;
	GUICtrl_List_t *pList = pCtrl->pCtrl;

	if( n >= pList->nNumItems )
		return NULL;

	for( i = 0, nNum = 0; i < pList->nNumItems; i++ )
	{
		if( pList->pItems[i].nFlags & FLAG_MARKED )
		{
			if( nNum == n )
				return pList->pItems[i].pStr;

			nNum++;
		}
	}

	return NULL;
}

void GUI_Ctrl_List_SetMarked( const GUIControl_t *pCtrl, unsigned int nIndex,
							  unsigned int bMarked )
{
	GUICtrl_List_t *pList = pCtrl->pCtrl;

	if( nIndex >= pList->nNumItems )
		return;

	if( bMarked )
		pList->pItems[nIndex].nFlags |= FLAG_MARKED;
	else
		pList->pItems[nIndex].nFlags &= ~FLAG_MARKED;
}

int GUI_Ctrl_List_Empty( const GUIControl_t *pCtrl )
{
	GUICtrl_List_t *pList = pCtrl->pCtrl;

	return (pList->nNumItems == 0);
}
