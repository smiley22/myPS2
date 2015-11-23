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
#include <Elf.h>
#include <Mp3.h>

#define ID_SELECT_ELF	100
#define ID_SHORTCUTS	101
//#define ID_GO_BACK		102
#define ID_DIR_VIEW		103

unsigned int GUI_CB_MyPrograms( GUIMenu_t *lpGUIMenu, unsigned int nGUIMsg,
								unsigned int nCtrlParam, unsigned int nOther )
{
	switch( nGUIMsg )
	{
		case GUI_MSG_OPEN:
			GUI_Ctrl_Dirview_SetFilter( GUI_ControlByID(ID_DIR_VIEW), "ELF" );
			break;

		case GUI_MSG_CONTROL:
			switch( LOWORD(nCtrlParam) )
			{
				case ID_SELECT_ELF:
					lpGUIMenu->iSelID = ID_DIR_VIEW;
					GUI_Render();
					break;

				case ID_GO_BACK:
					GUI_OpenMenu( GUI_MENU_MAIN );
					break;

				case ID_DIR_VIEW:
					switch( HIWORD(nCtrlParam) )
					{
						case GUI_NOT_DIRVIEW_CLICKED:
							StartELF();
							break;
					}
					break;
			}
			break;
	}
	return 0;
}

void StartELF( void )
{
	static char szELFPath[MAX_PATH + 1];

	snprintf(	szELFPath, sizeof(szELFPath), "%s%s",
				GUI_Ctrl_Dirview_GetDir(GUI_ControlByID(ID_DIR_VIEW)),
				GUI_Ctrl_Dirview_GetSel(GUI_ControlByID(ID_DIR_VIEW))->name );

	if( !CheckELFHeader(szELFPath) )
	{
		GUI_DlgMsgBox(	GUI_GetLangStr( LANG_STR_ERROR_ELF ),
						GUI_GetLangStr( LANG_STR_ERROR ),
						MB_ICON_ERROR );
		return;
	}

	// make sure any playing sounds are stopped
	MP3_Stop();
	
	RunELF(szELFPath);
}
