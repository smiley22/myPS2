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

#include <libhdd.h>

#define ID_MYPICTURES		100
#define ID_MYPROGRAMS		101
#define ID_MYFILES			102
#define ID_MYMUSIC			103
#define ID_SETTINGS			104
#define ID_EXIT				105

#define DLG_CONFIRM_EXIT	0
#define DLG_PARTITION_1		1
#define DLG_PARTITION_2		2

unsigned int GUI_CB_Main( GUIMenu_t *lpGUIMenu, unsigned int nGUIMsg, unsigned int nCtrlParam,
						  unsigned int nOther )
{
	static int bInit = 0;

	switch( nGUIMsg )
	{
		case GUI_MSG_OPEN:
			if( !bInit )
			{
				// myPS2 partition couldn't be found, show dialog
				if( HDD_Available() == HDD_AVAIL_NO_PART )
					GUI_OpenDialog( GUI_MENU_PARTITION1, lpGUIMenu, DLG_PARTITION_1 );

				bInit = 1;
			}
			break;

		case GUI_MSG_CONTROL:
			switch( LOWORD(nCtrlParam) )
			{
				case ID_MYPICTURES:
					GUI_OpenMenu( GUI_MENU_MYPICTURES );
					break;

				case ID_MYPROGRAMS:
					GUI_OpenMenu( GUI_MENU_MYPROGRAMS );
					break;

				case ID_MYFILES:
					GUI_OpenMenu( GUI_MENU_MYFILES );
					break;

				case ID_MYMUSIC:
					GUI_OpenMenu( GUI_MENU_MYMUSIC );
					break;

				case ID_SETTINGS:
					GUI_OpenMenu( GUI_MENU_SETTINGS );
					break;

				case ID_EXIT:
					GUI_DlgYesNo( GUI_GetLangStr(LANG_STR_TURN_OFF_PS2),
								  GUI_GetLangStr(LANG_STR_SHUTDOWN),
								  DLG_CONFIRM_EXIT );
					break;
			}
			break;

		case GUI_MSG_DIALOG:
			switch( LOWORD(nCtrlParam) )
			{
				case DLG_CONFIRM_EXIT:
					if( HIWORD(nCtrlParam) == DLG_RET_YES )
						hddPowerOff();
					break;

				case DLG_PARTITION_1:
					if( HIWORD(nCtrlParam) == DLG_RET_YES )
						GUI_OpenDialog( GUI_MENU_PARTITION2, lpGUIMenu, DLG_PARTITION_2 );
					break;
					
				case DLG_PARTITION_2:
					if( HIWORD(nCtrlParam) == DLG_RET_CREATE )
					{
						switch( nOther )
						{
							case DLG_ERR_CREATE:
								GUI_DlgMsgBox( GUI_GetLangStr(LANG_STR_PART_ERR_CREATE),
											   GUI_GetLangStr(LANG_STR_ERROR), 0 );
								break;

							case DLG_ERR_MOUNT:
								GUI_DlgMsgBox( GUI_GetLangStr(LANG_STR_PART_ERR_MOUNT),
											   GUI_GetLangStr(LANG_STR_ERROR), 0 );
								break;

							case DLG_ERR_NOERROR:
							default:
								GUI_DlgMsgBox( GUI_GetLangStr(LANG_STR_PART_CREATED),
											   GUI_GetLangStr(LANG_STR_OK), 0 );
								break;
						}
					}
					break;
			}
			break;
	}

	return 0;
}
