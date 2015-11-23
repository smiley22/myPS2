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
#include <Mp3.h>

#define ID_SORT_BY_NAME		100
#define	ID_SORT_BY_SIZE		101
#define ID_SORT_BY_TYPE		102
#define ID_SHOW_FILTER		103
#define ID_RADIO			104
#define ID_FILE_LIST		105
#define ID_PREV				106
#define ID_PLAY				107
#define ID_PAUSE			108
#define ID_STOP				109
#define ID_NEXT				110
#define ID_LOOP				111
#define ID_VOLUME_SLIDER	112

unsigned int GUI_CB_MyMusic( GUIMenu_t *lpGUIMenu, unsigned int nGUIMsg,
							 unsigned int nCtrlParam, unsigned int nOther )
{
	const char *lpDir;
	const fileInfo_t *lpFile;
	char szPath[MAX_PATH + 1];
	GUIControl_t *pCtrl;

	switch( nGUIMsg )
	{
		case GUI_MSG_OPEN:
			pCtrl = GUI_ControlByID(ID_VOLUME_SLIDER);
			if( pCtrl )
			{
				GUI_Ctrl_Slider_SetBounds( pCtrl, 0, 0x3FFF );
				GUI_Ctrl_Slider_SetStep( pCtrl, 0x3FF );
				GUI_Ctrl_Slider_SetPos( pCtrl, MP3_GetVolume() );
			}

			pCtrl = GUI_ControlByID(ID_SHOW_FILTER);
			if( pCtrl )
			{
				if( GUI_Ctrl_Combo_Empty(pCtrl) )
				{
					GUI_Ctrl_Combo_Add( pCtrl, GUI_GetLangStr(LANG_STR_SHOW_BOTH), 0 );
					GUI_Ctrl_Combo_Add( pCtrl, GUI_GetLangStr(LANG_STR_SHOW_MP3), 0 );
					GUI_Ctrl_Combo_Add( pCtrl, GUI_GetLangStr(LANG_STR_SHOW_M3U), 0 );
				}
			}
			break;

		case GUI_MSG_CONTROL:
			switch( LOWORD(nCtrlParam) )
			{
				case ID_SORT_BY_NAME:
					GUI_Ctrl_Dirview_Sort( GUI_ControlByID(ID_FILE_LIST), GUI_SORT_NAME );
					GUI_Render();
					break;

				case ID_SORT_BY_SIZE:
					GUI_Ctrl_Dirview_Sort( GUI_ControlByID(ID_FILE_LIST), GUI_SORT_SIZE );
					GUI_Render();
					break;

				case ID_SORT_BY_TYPE:
					GUI_Ctrl_Dirview_Sort( GUI_ControlByID(ID_FILE_LIST), GUI_SORT_TYPE );
					GUI_Render();
					break;

				case ID_SHOW_FILTER:
					switch( nOther )
					{
						default:
						case 0:
							GUI_Ctrl_Dirview_SetFilter( GUI_ControlByID(ID_FILE_LIST), "MP3 M3U" );
							break;

						case 1:
							GUI_Ctrl_Dirview_SetFilter( GUI_ControlByID(ID_FILE_LIST), "MP3" );
							break;

						case 2:
							GUI_Ctrl_Dirview_SetFilter( GUI_ControlByID(ID_FILE_LIST), "M3U" );
							break;
					}
					GUI_Ctrl_Dirview_Refresh(GUI_ControlByID(ID_FILE_LIST));
					GUI_Render();
					break;

				case ID_RADIO:
					GUI_OpenMenu( GUI_MENU_RADIO );
					break;

				case ID_GO_BACK:
					GUI_OpenMenu( GUI_MENU_MAIN );
					break;

				case ID_PREV:
					MP3_PrevTrack();
					break;

				case ID_PLAY:
					MP3_Play(NULL);
					break;

				case ID_PAUSE:
					MP3_SetPause( !(MP3_GetStatus() == MP3_PAUSED) );
					break;

				case ID_STOP:
					MP3_Stop();

					GUI_Render();
					break;

				case ID_NEXT:
					MP3_NextTrack();
					break;

				case ID_LOOP:
					MP3_SetLooping( GUI_Ctrl_ToggleButton_GetState(GUI_ControlByID(ID_LOOP)) );
					break;

				case ID_FILE_LIST:
					switch( HIWORD(nCtrlParam) )
					{
						case GUI_NOT_DIRVIEW_CLICKED:
							lpDir	= GUI_Ctrl_Dirview_GetDir( GUI_ControlByID(ID_FILE_LIST) );
							lpFile	= GUI_Ctrl_Dirview_GetSel( GUI_ControlByID(ID_FILE_LIST) );

							snprintf( szPath, sizeof(szPath), "%s%s", lpDir, lpFile->name );

							MP3_Play( szPath );
							break;
					}
					break;

				case ID_VOLUME_SLIDER:
					switch( HIWORD(nCtrlParam) )
					{
						case GUI_NOT_SL_POS_CHANGED:
							MP3_SetVolume( GUI_Ctrl_Slider_GetPos(GUI_ControlByID(ID_VOLUME_SLIDER)) );
							break;
					}
					break;
			}
			break;
	}
	return 0;
}
