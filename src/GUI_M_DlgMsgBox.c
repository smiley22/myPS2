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
#define ID_OK_BUTTON		102
#define ID_IMAGE_ERROR		103
#define ID_IMAGE_INFO		104
#define ID_IMAGE_OK			105

extern GUI_t GUI;

static char szText[MAX_PATH + 1];
static char szCaption[MAX_PATH + 1];
static u32	nType;

#define DLG_ID_MSGBOX 0xFFFFFFFF

unsigned int GUI_CB_DlgMsgBox( GUIMenu_t *lpGUIMenu, unsigned int nGUIMsg,
							   unsigned int nCtrlParam, unsigned int nOther )
{
	switch( nGUIMsg )
	{
		case GUI_MSG_OPEN:
			GUI_Ctrl_Label_SetText( GUI_ControlByID(ID_LABEL_CAPTION), szCaption );
			GUI_Ctrl_Label_SetText( GUI_ControlByID(ID_LABEL_TEXT), szText );

			switch( nType )
			{
				case MB_ICON_ERROR:
					GUI_CtrlSetVisible( GUI_ControlByID(ID_IMAGE_ERROR), 1 );
					GUI_CtrlSetVisible( GUI_ControlByID(ID_IMAGE_INFO), 0 );
					GUI_CtrlSetVisible( GUI_ControlByID(ID_IMAGE_OK), 0 );
					break;

				case MB_ICON_INFO:
					GUI_CtrlSetVisible( GUI_ControlByID(ID_IMAGE_ERROR), 0 );
					GUI_CtrlSetVisible( GUI_ControlByID(ID_IMAGE_INFO), 1 );
					GUI_CtrlSetVisible( GUI_ControlByID(ID_IMAGE_OK), 0 );
					break;

				default:
					GUI_CtrlSetVisible( GUI_ControlByID(ID_IMAGE_ERROR), 0 );
					GUI_CtrlSetVisible( GUI_ControlByID(ID_IMAGE_INFO), 0 );
					GUI_CtrlSetVisible( GUI_ControlByID(ID_IMAGE_OK), 1 );
					break;
			}
			break;

		case GUI_MSG_CONTROL:
			switch( LOWORD(nCtrlParam) )
			{
				case ID_OK_BUTTON:
					GUI_CloseDialog(lpGUIMenu, 0, 0);
					break;
			}
			break;
	}
	return 0;
}

void GUI_DlgMsgBox( const char *lpText, const char *lpCaption, u32 nFlags )
{
	strncpy( szText, lpText, MAX_PATH );
	szText[MAX_PATH] = 0;

	strncpy( szCaption, lpCaption, MAX_PATH );
	szCaption[MAX_PATH] = 0;

	nType = nFlags;
	GUI_OpenDialog( GUI_MENU_MSGBOX, GUI.pActiveMenu, DLG_ID_MSGBOX );
}
