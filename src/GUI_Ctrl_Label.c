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

void GUI_Ctrl_Label_Draw( const GUIControl_t *pCtrl )
{
	const char			*pLangStr;
	char				szDisplayStr[MAX_PATH + 1];
	int					nStrIndex;
	unsigned int		nPosX;
	const GUIMenuFont_t	*pFont;
	GUICtrl_Label_t		*pLabel = pCtrl->pCtrl;

	if( !pLabel->lpStr )
		return;

	if( pLabel->nFontIdx == -1 )
		return;

	pLangStr = pLabel->lpStr;
	CharsetConvert_UTF8ToCharset( szDisplayStr, pLangStr, sizeof(szDisplayStr) );

	if( !(pFont = GUI_MenuGetFont( pCtrl->pParent, pLabel->nFontIdx )) )
		return;

	if( pLabel->nShowCursor )
	{
		szDisplayStr[ pLabel->nCursorPos ] = '|';
		
		strncpy( szDisplayStr + pLabel->nCursorPos + 1,
				 pLangStr + pLabel->nCursorPos, MAX_PATH - pLabel->nCursorPos );
	}

	szDisplayStr[ MAX_PATH ] = 0;
	nStrIndex = strlen(szDisplayStr) - 1;

	if( pLabel->nWidth > 0 )
	{
		while( gsLib_font_width( pFont->gsFont, szDisplayStr ) > pLabel->nWidth )
		{
			szDisplayStr[ nStrIndex ] = 0;

			if( nStrIndex > 1 )
			{
				szDisplayStr[ nStrIndex - 1 ] = '.';
				szDisplayStr[ nStrIndex - 2 ] = '.';
			}

			nStrIndex--;
		}
	}

	nPosX = pCtrl->nPosX;

	if( pLabel->nAlign & GUI_LABEL_RIGHT )
	{
		nPosX += pLabel->nWidth - gsLib_font_width( pFont->gsFont, szDisplayStr );
	}
	else if( pLabel->nAlign & GUI_LABEL_CENTER )
	{
		nPosX += (pLabel->nWidth - gsLib_font_width( pFont->gsFont, szDisplayStr )) / 2;
	}

	gsLib_font_print( pFont->gsFont, nPosX, pCtrl->nPosY, pLabel->nTextColor, szDisplayStr );
}

void GUI_Ctrl_Label_SetText( GUIControl_t *pCtrl, const char *lpText )
{
	GUICtrl_Label_t *pLabel = pCtrl->pCtrl;

	if( pLabel->lpStr )
		free(pLabel->lpStr);

	if( !(pLabel->lpStr = malloc( strlen(lpText) + 1 )) )
		return;

	strcpy( pLabel->lpStr, lpText );
}

const char *GUI_Ctrl_Label_GetText( const GUIControl_t *pCtrl )
{
	GUICtrl_Label_t *pLabel = pCtrl->pCtrl;

	return pLabel->lpStr;
}

void GUI_Ctrl_Label_ShowCursor( GUIControl_t *pCtrl, unsigned int bShow )
{
	GUICtrl_Label_t *pLabel = pCtrl->pCtrl;

	pLabel->nShowCursor = bShow;
}

unsigned int GUI_Ctrl_Label_GetCursor( const GUIControl_t *pCtrl )
{
	GUICtrl_Label_t *pLabel = pCtrl->pCtrl;

	return pLabel->nCursorPos;
}

void GUI_Ctrl_Label_SetCursor( GUIControl_t *pCtrl, unsigned int nPos )
{
	GUICtrl_Label_t *pLabel = pCtrl->pCtrl;

	if( !pLabel->lpStr )
		return;

	if( nPos > strlen(pLabel->lpStr) )
		nPos = strlen(pLabel->lpStr);

	pLabel->nCursorPos = nPos;
}

void GUI_Ctrl_Label_Insert( GUIControl_t *pCtrl, unsigned int nPos, char c )
{
	GUICtrl_Label_t *pLabel = pCtrl->pCtrl;
	unsigned int nLen, i;
	char *pStr;

	if( pLabel->lpStr )
	{
		nLen = strlen(pLabel->lpStr);
		pStr = malloc( nLen + 2 );

		if( nPos > nLen )
			nPos = nLen;

		for( i = 0; i < nPos; i++ )
			pStr[i] = pLabel->lpStr[i];

		pStr[i] = c;

		for( i = nPos; i < nLen; i++ )
			pStr[ i + 1 ] = pLabel->lpStr[i];

		pStr[ i + 1 ] = 0;

		free(pLabel->lpStr);
		pLabel->lpStr = pStr;
	}
	else
	{
		pLabel->lpStr = malloc( 2 );

		pLabel->lpStr[0] = c;
		pLabel->lpStr[1] = 0;
	}
}

void GUI_Ctrl_Label_Delete( GUIControl_t *pCtrl, unsigned int nPos )
{
	GUICtrl_Label_t		*pLabel = pCtrl->pCtrl;
	unsigned int		nLength, i;
	char				*pNew;

	if( !pLabel->lpStr )
		return;

	if( (nLength = strlen(pLabel->lpStr)) <= 1 )
	{
		free(pLabel->lpStr);

		pLabel->lpStr = NULL;
		return;
	}

	if( nPos >= nLength )
		nPos = nLength - 1;

	pNew = (char*) malloc( nLength );

	strncpy( pNew, pLabel->lpStr, nPos );

	pNew[nPos] = 0;

	for( i = nPos + 1; i < nLength; i++ )
		pNew[ i - 1 ] = pLabel->lpStr[i];

	pNew[ i - 1 ] = 0;

	free( pLabel->lpStr );
	pLabel->lpStr = pNew;
}
