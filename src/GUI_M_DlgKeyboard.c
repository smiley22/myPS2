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
#include <GUI_Menu.h>

#define ID_LABEL_CAPTION		100
#define ID_LABEL_EDIT			101
#define ID_BUTTON_LEFT			103
#define ID_BUTTON_RIGHT			104
#define ID_BUTTON_DELETE		105
#define ID_BUTTON_SHIFT			106
#define ID_BUTTON_SPACE			107
#define ID_BUTTON_BACKSPACE		108
#define ID_BUTTON_OK			109
#define ID_BUTTON_CANCEL		110

extern GUI_t GUI;

static char szText[MAX_PATH + 1];
static char szCaption[MAX_PATH + 1];
static char szRetValue[MAX_PATH + 1];

static char szSymbols[10] = { ')', '!', '@', '#', '$', '%', '^', '&', '*', '(' };

static void UpdateButtons( void )
{
	unsigned int i, bShift;
	char szLabel[2];
	GUIControl_t *pCtrl;

	pCtrl	= GUI_ControlByID(ID_BUTTON_SHIFT);
	bShift	= GUI_Ctrl_ToggleButton_GetState(pCtrl);

	szLabel[1]	= 0;

	for( i = 48; i <= 57; i++ )
	{
		if( bShift == GUI_BUTTON_OFF )
			szLabel[0] = i;
		else
			szLabel[0] = szSymbols[ i - 48 ];

		if( (pCtrl = GUI_ControlByID(i)) )
			GUI_Ctrl_Button_SetText( pCtrl, szLabel );
	}

	for( i = 65; i <= 90; i++ )
	{
		szLabel[0] = i;

		// lower case
		if( bShift == GUI_BUTTON_OFF )
			szLabel[0] += 32;

		if( (pCtrl = GUI_ControlByID(i)) )
			GUI_Ctrl_Button_SetText( pCtrl, szLabel );
	}

	// ':'
	szLabel[0] = ':';
	GUI_Ctrl_Button_SetText( GUI_ControlByID(':'), szLabel );

	// '.'
	szLabel[0] = '.';
	GUI_Ctrl_Button_SetText( GUI_ControlByID('.'), szLabel );

	// '_'
	szLabel[0] = '_';
	GUI_Ctrl_Button_SetText( GUI_ControlByID('_'), szLabel );

	// '/'
	szLabel[0] = '/';
	GUI_Ctrl_Button_SetText( GUI_ControlByID('/'), szLabel );
}

static void KeyboardChar( char c )
{
	unsigned int bShift, nPos;

	bShift	= GUI_Ctrl_ToggleButton_GetState( GUI_ControlByID(ID_BUTTON_SHIFT) );
	nPos	= GUI_Ctrl_Label_GetCursor( GUI_ControlByID(ID_LABEL_EDIT) );

	// number
	if( c >= 48 && c <= 57 )
	{
		if( bShift == GUI_BUTTON_ON )
			c = szSymbols[ c - 48 ];
	}
	else if( c >= 65 && c <= 90 )
	{
		if( bShift == GUI_BUTTON_OFF )
			c += 32;
	}

	GUI_Ctrl_Label_Insert( GUI_ControlByID(ID_LABEL_EDIT), nPos, c );
	GUI_Ctrl_Label_SetCursor( GUI_ControlByID(ID_LABEL_EDIT), nPos + 1 );
}

unsigned int GUI_CB_DlgKeyboard( GUIMenu_t *lpGUIMenu, unsigned int nGUIMsg,
								 unsigned int nCtrlParam, unsigned int nOther )
{
	unsigned int nPos, nLen;
	GUIControl_t *pCtrl = GUI_ControlByID(ID_LABEL_EDIT);
	const char *pStr;

	switch( nGUIMsg )
	{
		case GUI_MSG_OPEN:
			GUI_Ctrl_Label_SetText( GUI_ControlByID(ID_LABEL_CAPTION), szCaption );
			GUI_Ctrl_Label_SetText(	pCtrl, szText );
			GUI_Ctrl_Label_ShowCursor( pCtrl, 1 );
			GUI_Ctrl_Label_SetCursor( pCtrl, strlen(szText) );

			UpdateButtons();
			break;

		case GUI_MSG_CONTROL:
			if( (LOWORD(nCtrlParam) >= 48 && LOWORD(nCtrlParam) <= 57) ||
				(LOWORD(nCtrlParam) >= 65 && LOWORD(nCtrlParam) <= 90) ||
				LOWORD(nCtrlParam) == '.' || LOWORD(nCtrlParam) == '/' ||
				LOWORD(nCtrlParam) == '_' || LOWORD(nCtrlParam) == ':' )
			{
				KeyboardChar( LOWORD(nCtrlParam) );
				GUI_Render();
				break;
			}

			switch( LOWORD(nCtrlParam) )
			{
				case ID_BUTTON_LEFT:
					nPos = GUI_Ctrl_Label_GetCursor(pCtrl);
					if( nPos > 0 )
					{
						GUI_Ctrl_Label_SetCursor( pCtrl, nPos - 1 );
						GUI_Render();
					}
					break;

				case ID_BUTTON_RIGHT:
					nPos = GUI_Ctrl_Label_GetCursor(pCtrl);
					nLen = strlen( GUI_Ctrl_Label_GetText(pCtrl) );
					if( nPos < nLen )
					{
						GUI_Ctrl_Label_SetCursor( pCtrl, nPos + 1 );
						GUI_Render();
					}
					break;

				case ID_BUTTON_DELETE:
					nPos = GUI_Ctrl_Label_GetCursor(pCtrl);
					pStr = GUI_Ctrl_Label_GetText(pCtrl);
					if( !pStr )
						break;

					nLen = strlen( pStr );
					if( nPos < nLen )
					{
						GUI_Ctrl_Label_Delete( pCtrl, nPos );
						GUI_Render();
					}
					break;

				case ID_BUTTON_SPACE:
					nPos = GUI_Ctrl_Label_GetCursor(pCtrl);
					GUI_Ctrl_Label_Insert( pCtrl, nPos, ' ' );
					GUI_Ctrl_Label_SetCursor( pCtrl, nPos + 1 );
					GUI_Render();
					break;
					
				case ID_BUTTON_BACKSPACE:
					nPos = GUI_Ctrl_Label_GetCursor(pCtrl);
					if( nPos > 0 )
					{
						GUI_Ctrl_Label_Delete( pCtrl, nPos - 1 );
						GUI_Ctrl_Label_SetCursor( pCtrl, nPos - 1 );
						GUI_Render();
					}
					break;

				case ID_BUTTON_SHIFT:
					UpdateButtons();
					GUI_Render();
					break;

				case ID_BUTTON_OK:
					strncpy( szRetValue, GUI_Ctrl_Label_GetText(pCtrl), MAX_PATH );
					szRetValue[ MAX_PATH ] = 0;

					GUI_CloseDialog( lpGUIMenu, DLG_RET_SUBMIT, (unsigned int)&szRetValue[0] );
					break;

				case ID_GO_BACK:
				case ID_BUTTON_CANCEL:
					GUI_CloseDialog( lpGUIMenu, DLG_RET_CANCEL, 0 );
					break;
			}
			break;
	}
	return 0;
}

void GUI_DlgKeyboard( const char *lpEdit, const char *lpCaption, unsigned int nDlgID )
{
	strncpy( szText, lpEdit, MAX_PATH );
	szText[MAX_PATH] = 0;

	strncpy( szCaption, lpCaption, MAX_PATH );
	szCaption[MAX_PATH] = 0;

	GUI_OpenDialog( GUI_MENU_DLGKEYBOARD, GUI.pActiveMenu, nDlgID );
}
