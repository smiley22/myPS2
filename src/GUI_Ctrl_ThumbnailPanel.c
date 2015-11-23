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

void GUI_Ctrl_ThumbnailPanel_Draw( const GUIControl_t *pCtrl )
{
	unsigned int i, c, nNumItemsX, nNumItemsY;
	unsigned int nItemPosX, nItemPosY;
	unsigned int nTexPosX, nTexPosY;
	unsigned int nLabelPosX, nLabelPosY;
	unsigned int nThumbPosX, nThumbPosY, nThumbWidth, nThumbHeight;
	float f;
	const GUIMenuImage_t *pItemTex, *pItemTexFocus, *pTex, *pScrollTex;
	const GUIMenuFont_t *pFont;
	unsigned int bHasFocus, bItemFocus, nItemIdx;
	u64	uTextColor;
	GUICtrl_ThumbnailPanel_t *pPanel = pCtrl->pCtrl;
	GUIThumbItem_t *pThumbItem;
	char szStr[MAX_PATH + 1];
	unsigned int nStrIndex;

	if( pPanel->nItemTexture == -1 || pPanel->nItemTextureFocus == -1 ||
		pPanel->nFontIdx == -1 )
		return;

	pItemTex = GUI_MenuGetImage( pCtrl->pParent, pPanel->nItemTexture );
	if( !pItemTex )
		return;

	pItemTexFocus = GUI_MenuGetImage( pCtrl->pParent, pPanel->nItemTextureFocus );
	if( !pItemTexFocus )
		return;

	pFont = GUI_MenuGetFont( pCtrl->pParent, pPanel->nFontIdx );
	if( !pFont )
		return;

	nNumItemsX	= pPanel->nWidth / (pPanel->nItemWidth + pPanel->nPaddingX);
	nNumItemsY	= pPanel->nHeight / (pPanel->nItemHeight + pPanel->nPaddingY);

	nItemPosX	= pCtrl->nPosX + pPanel->nPaddingX;
	nItemPosY	= pCtrl->nPosY + pPanel->nPaddingY;

	bHasFocus	= pCtrl->pParent->iSelID == pCtrl->nID;

	// start at current Index
	c = 0;

	for( i = 0; i < nNumItemsY; i++ )
	{
		for( c = 0; c < nNumItemsX; c++ )
		{
			nItemIdx = pPanel->nStartItem + i * nNumItemsX + c;

			if( nItemIdx >= pPanel->nNumItems )
				break;

			bItemFocus = (bHasFocus) && (nItemIdx == pPanel->nSelectedItem);

			nTexPosX = nItemPosX + (pPanel->nItemWidth - pPanel->nTextureWidth) / 2;
			nTexPosY = nItemPosY;

			pTex = bItemFocus ? pItemTexFocus : pItemTex;

			gsLib_prim_sprite_texture(	pTex->gsTexture, nTexPosX, nTexPosY,
										pPanel->nTextureWidth, pPanel->nTextureHeight,
										0, 0, pTex->gsTexture->Width,
										pTex->gsTexture->Height, colorTex );

			// Parent Callback aufrufen um Thumbnail und String
			// zu erhalten
			pThumbItem = &pPanel->pItems[nItemIdx];

			if( !pPanel->pItems[nItemIdx].pStr || !pPanel->pItems[nItemIdx].pTexture )
			{
				pCtrl->pParent->pfnCallback(	pCtrl->pParent, GUI_MSG_CONTROL,
												MAKEPARAM( pCtrl->nID, GUI_NOT_NEED_INFO ),
												(u32) &pPanel->pItems[nItemIdx] );
			}

			nThumbWidth		= pPanel->nThumbWidth;
			nThumbHeight	= pPanel->nThumbHeight;

			// scale thumbnail dimensions so they fit thumbwidth and thumbheight
			if( pThumbItem->pTexture->Width > pThumbItem->pTexture->Height )
			{
				f = (float) pThumbItem->pTexture->Height /
					(float) pThumbItem->pTexture->Width;

				nThumbHeight *= f;
			}
			else
			{
				f = (float) pThumbItem->pTexture->Width /
					(float) pThumbItem->pTexture->Height;

				nThumbWidth *= f;
			}

			nThumbPosX = nTexPosX + pPanel->nThumbOffsetX;
			nThumbPosY = nTexPosY + pPanel->nThumbOffsetY;

			if( pPanel->nThumbAlign & GUI_THUMB_ALIGN_CENTER )
			{
				nThumbPosX = nTexPosX + (pPanel->nTextureWidth - nThumbWidth) / 2;
				nThumbPosY = nTexPosY + (pPanel->nTextureHeight - nThumbHeight) / 2;
			}

			gsLib_prim_sprite_texture(	pThumbItem->pTexture, nThumbPosX, nThumbPosY,
										nThumbWidth, nThumbHeight, 0, 0,
										pThumbItem->pTexture->Width,
										pThumbItem->pTexture->Height,
										colorTex );

			// text label is vertically aligned with the bottom of the focus texture
			// and center aligned horizontally
			if( pThumbItem->pStr )
			{
				CharsetConvert_UTF8ToCharset( szStr, pThumbItem->pStr, sizeof(szStr) );
				nStrIndex = strlen(szStr) - 1;

				while( gsLib_font_width( pFont->gsFont, szStr ) > pPanel->nItemWidth )
				{
					szStr[ nStrIndex ] = 0;

					if( nStrIndex > 1 )
					{
						szStr[ nStrIndex - 1 ] = '.';
						szStr[ nStrIndex - 2 ] = '.';
					}

					nStrIndex--;
				}

				nLabelPosX	=	nItemPosX + (pPanel->nItemWidth - gsLib_font_width( pFont->gsFont,
								szStr )) / 2;

				nLabelPosY	=	nTexPosY + pPanel->nTextureHeight;
				uTextColor	=	bItemFocus ? pPanel->nSelectedColor : pPanel->nTextColor;

				gsLib_font_print( pFont->gsFont, nLabelPosX, nLabelPosY, uTextColor, szStr );
			}

			nItemPosX += pPanel->nItemWidth + pPanel->nPaddingX;
		}

		// increment y draw position
		// reset x draw position
		nItemPosY	+= pPanel->nItemHeight + pPanel->nPaddingY;
		nItemPosX	= pCtrl->nPosX + pPanel->nPaddingX;
		c			= 0;

	}

	if( pPanel->nStartItem > 0 )
	{
		pScrollTex = GUI_MenuGetImage( pCtrl->pParent, pPanel->nUpTexture );

		if( pScrollTex )
		{
			gsLib_prim_sprite_texture(	pScrollTex->gsTexture, pCtrl->nPosX + pPanel->nUpTexPosX,
										pCtrl->nPosY + pPanel->nUpTexPosY,
										pScrollTex->gsTexture->Width, pScrollTex->gsTexture->Height,
										0, 0, pScrollTex->gsTexture->Width,
										pScrollTex->gsTexture->Height, colorTex );
		}
	}

	if( (pPanel->nStartItem + i * nNumItemsX + c) < pPanel->nNumItems )
	{
		pScrollTex = GUI_MenuGetImage( pCtrl->pParent, pPanel->nDownTexture );

		if( pScrollTex )
		{
			gsLib_prim_sprite_texture(	pScrollTex->gsTexture, pCtrl->nPosX + pPanel->nDownTexPosX,
										pCtrl->nPosY + pPanel->nDownTexPosY,
										pScrollTex->gsTexture->Width, pScrollTex->gsTexture->Height,
										0, 0, pScrollTex->gsTexture->Width,
										pScrollTex->gsTexture->Height, colorTex );
		}

	}

}

void GUI_Ctrl_ThumbnailPanel_AddItem( GUIControl_t *pCtrl, void *pUser )
{
	GUICtrl_ThumbnailPanel_t *pPanel = pCtrl->pCtrl;
	GUIThumbItem_t *pItems;

	pItems = malloc( sizeof(GUIThumbItem_t) * (pPanel->nNumItems + 1) );

	if(!pItems)
	{
		pPanel->nNumItems = 0;
		return;
	}

	if( pPanel->pItems )
	{
		memcpy( pItems, pPanel->pItems, pPanel->nNumItems * sizeof(GUIThumbItem_t) );
		free( pPanel->pItems );
	}

	pPanel->pItems		= pItems;
	pItems				= &pPanel->pItems[ pPanel->nNumItems ];

	pItems->pStr		= NULL;
	pItems->pTexture	= NULL;
	pItems->pUser		= pUser;

	pPanel->nNumItems++;
}

void GUI_Ctrl_ThumbnailPanel_Clean( GUIControl_t *pCtrl )
{
	unsigned int i;
	unsigned int nNumItemsX;
	unsigned int nNumItemsY;
	GUICtrl_ThumbnailPanel_t *pPanel = pCtrl->pCtrl;

	nNumItemsX	= pPanel->nWidth / (pPanel->nItemWidth + pPanel->nPaddingX);
	nNumItemsY	= pPanel->nHeight / (pPanel->nItemHeight + pPanel->nPaddingY);


	for( i = 0; i < nNumItemsX * nNumItemsY; i++ )
	{
		if( (pPanel->nStartItem + i) >= pPanel->nNumItems )
			break;

		pCtrl->pParent->pfnCallback(	pCtrl->pParent, GUI_MSG_CONTROL,
										MAKEPARAM( pCtrl->nID, GUI_NOT_REMOVE ),
										(u32) &pPanel->pItems[ pPanel->nStartItem + i] );

	}

	if( pPanel->pItems )
		free( pPanel->pItems );

	pPanel->pItems			= NULL;
	pPanel->nNumItems		= 0;
	pPanel->nStartItem		= 0;
	pPanel->nSelectedItem	= 0;
}

void GUI_Ctrl_ThumbnailPanel_VramClean( GUIControl_t *pCtrl )
{
	unsigned int i;
	unsigned int nNumItemsX;
	unsigned int nNumItemsY;
	GUICtrl_ThumbnailPanel_t *pPanel = pCtrl->pCtrl;

	nNumItemsX	= pPanel->nWidth / (pPanel->nItemWidth + pPanel->nPaddingX);
	nNumItemsY	= pPanel->nHeight / (pPanel->nItemHeight + pPanel->nPaddingY);


	for( i = 0; i < nNumItemsX * nNumItemsY; i++ )
	{
		if( (pPanel->nStartItem + i) >= pPanel->nNumItems )
			break;

		pCtrl->pParent->pfnCallback(	pCtrl->pParent, GUI_MSG_CONTROL,
										MAKEPARAM( pCtrl->nID, GUI_NOT_REMOVE ),
										(u32) &pPanel->pItems[ pPanel->nStartItem + i] );

	}
}

int GUI_Ctrl_ThumbnailPanel_Input( GUIControl_t *pCtrl, unsigned int nPadBtns )
{
	unsigned int nNumItemsX;
	unsigned int nNumItemsY;
	unsigned int nNewSelect, i;
	GUICtrl_ThumbnailPanel_t *pPanel = pCtrl->pCtrl;

	nNumItemsX	= pPanel->nWidth / (pPanel->nItemWidth + pPanel->nPaddingX);
	nNumItemsY	= pPanel->nHeight / (pPanel->nItemHeight + pPanel->nPaddingY);

	if( nPadBtns & PAD_LEFT )
	{
		if( (pPanel->nSelectedItem % nNumItemsX) == 0 )
		{
			//printf("Leftmost\n");
			return 0;
		}

		// move to the adjacent left item
		pPanel->nSelectedItem--;
		GUI_Render();

		return 1;
	}
	else if( nPadBtns & PAD_RIGHT )
	{
		if( (pPanel->nSelectedItem % nNumItemsX) == (nNumItemsX -1) )
		{
			//printf("Rightmost\n");
			return 0;
		}

		// move to the adjacent right item if this isn't the last
		// item in the thumbnail panel
		if( pPanel->nSelectedItem == (pPanel->nNumItems - 1) )
		{
			//printf("Last item in panel\n");
			return 0;
		}

		pPanel->nSelectedItem++;
		GUI_Render();

		return 1;
	}
	else if( nPadBtns & PAD_UP )
	{
		if( pPanel->nSelectedItem <= (pPanel->nStartItem + nNumItemsX - 1) )
		{
			//printf("Upmost row\n");

			if( pPanel->nStartItem > 0 )
			{
				//printf("scrolling up\n");
				// let parent know which items are no longer needed
				for( i = 0; i < nNumItemsY * nNumItemsX; i++ )
				{
					if( (pPanel->nStartItem + i) >= pPanel->nNumItems )
						break;

					pCtrl->pParent->pfnCallback(	pCtrl->pParent, GUI_MSG_CONTROL,
													MAKEPARAM( pCtrl->nID, GUI_NOT_REMOVE ),
													(u32) &pPanel->pItems[ pPanel->nStartItem +
													i ] );
				}

				pPanel->nStartItem -= nNumItemsY * nNumItemsX;

				// select item above current position
				nNewSelect =	pPanel->nStartItem + (pPanel->nSelectedItem % nNumItemsX) +
								(nNumItemsX * (nNumItemsY - 1));

				pPanel->nSelectedItem = nNewSelect;
				GUI_Render();
				return 1;
			}
			return 0;
		}

		// otherwise move to the adjacent upper pane
		pPanel->nSelectedItem -= nNumItemsX;
		GUI_Render();

		return 1;
	}
	else if( nPadBtns & PAD_DOWN )
	{
		if( pPanel->nSelectedItem >= (pPanel->nStartItem + nNumItemsY *
									 (nNumItemsX - 1)) )
		{
			//printf("Downmost row\n");

			if( pPanel->nNumItems > (pPanel->nStartItem + nNumItemsY * nNumItemsX) )
			{
				//printf("scrolling down\n");
				for( i = 0; i < nNumItemsY * nNumItemsX; i++ )
				{
					pCtrl->pParent->pfnCallback(	pCtrl->pParent, GUI_MSG_CONTROL,
													MAKEPARAM( pCtrl->nID, GUI_NOT_REMOVE ),
													(u32) &pPanel->pItems[ pPanel->nStartItem +
													i ] );
				}

				pPanel->nStartItem += nNumItemsY * nNumItemsX;

				// try to set selected item below current position
				nNewSelect = pPanel->nStartItem + (pPanel->nSelectedItem % nNumItemsX);

				if( nNewSelect >= pPanel->nNumItems )
					nNewSelect = pPanel->nStartItem;

				pPanel->nSelectedItem = nNewSelect;
				GUI_Render();

				return 1;
			}

			return 0;
		}

		// if not on downmost row move down one row
		if( (pPanel->nSelectedItem + nNumItemsX) >= pPanel->nNumItems )
			return 0;

		pPanel->nSelectedItem += nNumItemsX;
		GUI_Render();

		return 1;
	}
	else if( nPadBtns & PAD_CROSS )
	{
		pCtrl->pParent->pfnCallback(	pCtrl->pParent, GUI_MSG_CONTROL,
										MAKEPARAM( pCtrl->nID, GUI_NOT_THUMB ),
										(u32) &pPanel->pItems[ pPanel->nSelectedItem ] );
	}

	return 0;
}
