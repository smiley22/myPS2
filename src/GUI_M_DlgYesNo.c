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

#define ID_LABEL_CAPTION	100
#define ID_LABEL_TEXT		101
#define ID_BUTTON_NO		102
#define ID_BUTTON_YES		103

extern GUI_t GUI;

static char szText[MAX_PATH + 1];
static char szCaption[MAX_PATH + 1];

unsigned int GUI_CB_DlgYesNo( GUIMenu_t *lpGUIMenu, unsigned int nGUIMsg,
							  unsigned int nCtrlParam, unsigned int nOther )
{
	switch( nGUIMsg )
	{
		case GUI_MSG_OPEN:
			GUI_Ctrl_Label_SetText( GUI_ControlByID(ID_LABEL_CAPTION), szCaption );
			GUI_Ctrl_Label_SetText( GUI_ControlByID(ID_LABEL_TEXT), szText );

			// always select 'NO' as default
			lpGUIMenu->iSelID = ID_BUTTON_NO;
			break;

		case GUI_MSG_CONTROL:
			switch( LOWORD(nCtrlParam) )
			{
				case ID_BUTTON_NO:
					GUI_CloseDialog(lpGUIMenu, DLG_RET_NO, 0);
					break;

				case ID_BUTTON_YES:
					GUI_CloseDialog(lpGUIMenu, DLG_RET_YES, 0);
					break;
			}
			break;
	}

	return 0;
}

void GUI_DlgYesNo( const char *lpText, const char *lpCaption, unsigned int nDlgID )
{
	strncpy( szText, lpText, MAX_PATH );
	szText[MAX_PATH] = 0;

	strncpy( szCaption, lpCaption, MAX_PATH );
	szCaption[MAX_PATH] = 0;

	GUI_OpenDialog( GUI_MENU_DLGYESNO, GUI.pActiveMenu, nDlgID );
}
