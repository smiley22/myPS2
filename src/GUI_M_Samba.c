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
#include <Sysconf.h>

#define ID_WORKGROUP		100
#define ID_WINS_SERVER		101

#define DLG_SMB_WORKGROUP	0
#define DLG_SMB_WINS		1

unsigned int GUI_CB_Samba( GUIMenu_t *lpGUIMenu, unsigned int nGUIMsg,
						   unsigned int nCtrlParam, unsigned int nOther )
{
	switch( nGUIMsg )
	{
		case GUI_MSG_CONTROL:
			switch( LOWORD(nCtrlParam) )
			{
				case ID_WORKGROUP:
					GUI_DlgKeyboard( SC_GetValueForKey_Str( "net_smb_workgroup", NULL ),
									 GUI_GetLangStr(LANG_STR_WORKGROUP),
									 DLG_SMB_WORKGROUP );
					break;

				case ID_WINS_SERVER:
					GUI_DlgKeyboard( SC_GetValueForKey_Str( "net_smb_wins", NULL ),
									 GUI_GetLangStr(LANG_STR_WINS),
									 DLG_SMB_WINS );
					break;

				case ID_GO_BACK:
					GUI_OpenMenu( GUI_MENU_NETWORK );
					break;

			}
			break;

		case GUI_MSG_DIALOG:
			if( HIWORD(nCtrlParam) == DLG_RET_SUBMIT )
			{
				switch( LOWORD(nCtrlParam) )
				{
					case DLG_SMB_WORKGROUP:
						SC_SetValueForKey_Str( "net_smb_workgroup", (const char*) nOther );
						break;

					case DLG_SMB_WINS:
						SC_SetValueForKey_Str( "net_smb_wins", (const char*) nOther );
						break;
				}
			}
			break;
	}
	return 0;
}
