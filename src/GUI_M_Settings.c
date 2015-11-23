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

#define ID_LOAD_SETTINGS		100
#define ID_SAVE_SETTINGS		101
#define ID_NETWORK_SETTINGS		102
#define ID_PARTITION_SETTINGS	103
#define ID_SKIN_SETTINGS		104
#define ID_LANGUAGE_SETTINGS	105
#define ID_SCREEN_MODE			106
#define ID_SCREEN_ADJUST_X		107
#define ID_SCREEN_ADJUST_Y		108
#define ID_SCREENSHOTS			109
#define ID_SCREENSHOTS_PATH		110
#define ID_CACHE_THUMBS			111
#define ID_CACHE_THUMBS_PATH	112
#define ID_CACHE_SHOUTCAST		113

#define DLG_SCREENSHOTS_PATH	0
#define DLG_THUMBS_PATH			1

unsigned int GUI_CB_Settings( GUIMenu_t *lpGUIMenu, unsigned int nGUIMsg,
							  unsigned int nCtrlParam, unsigned int nOther )
{
	GUIControl_t *pCtrl;
	int	nMode, nSelect;
	int nValue;

	switch( nGUIMsg )
	{
		// init controls with settings from config file
		case GUI_MSG_OPEN:
			pCtrl = GUI_ControlByID(ID_SCREEN_MODE);
			if( GUI_Ctrl_Combo_Empty(pCtrl) )
			{
					GUI_Ctrl_Combo_Add( pCtrl, GUI_GetLangStr(LANG_STR_AUTO), 0 );
					GUI_Ctrl_Combo_Add( pCtrl, GUI_GetLangStr(LANG_STR_NTSC), 0 );
					GUI_Ctrl_Combo_Add( pCtrl, GUI_GetLangStr(LANG_STR_PAL), 0 );

					nMode = SC_GetValueForKey_Int( "scr_mode", NULL );
					
					if( nMode == GS_MODE_AUTO )
						nSelect = 0;
					else if( nMode == GS_MODE_NTSC )
						nSelect = 1;
					else
						nSelect = 2;

					GUI_Ctrl_Combo_SetCurSel( pCtrl, nSelect );
			}

			pCtrl = GUI_ControlByID(ID_SCREEN_ADJUST_X);
			GUI_Ctrl_Slider_SetBounds( pCtrl, -255, 255 );
			GUI_Ctrl_Slider_SetStep( pCtrl, 5 );
			GUI_Ctrl_Slider_SetPos( pCtrl, SC_GetValueForKey_Int("scr_adjust_x", NULL ));

			pCtrl = GUI_ControlByID(ID_SCREEN_ADJUST_Y);
			GUI_Ctrl_Slider_SetBounds( pCtrl, -255, 255 );
			GUI_Ctrl_Slider_SetStep( pCtrl, 5 );
			GUI_Ctrl_Slider_SetPos( pCtrl, SC_GetValueForKey_Int("scr_adjust_y", NULL ));

			pCtrl = GUI_ControlByID(ID_SCREENSHOTS);
			GUI_Ctrl_ToggleButton_SetState( pCtrl, SC_GetValueForKey_Int("scr_screenshot",
											NULL ));

			pCtrl = GUI_ControlByID(ID_CACHE_THUMBS);
			GUI_Ctrl_ToggleButton_SetState( pCtrl, SC_GetValueForKey_Int("tbn_caching",
											NULL ));

			pCtrl = GUI_ControlByID(ID_CACHE_SHOUTCAST);
			GUI_Ctrl_ToggleButton_SetState( pCtrl,
											SC_GetValueForKey_Int("radio_cache_list", NULL ));

			// disable save button if running from CD
			if( GetBootMode() == BOOT_CD )
				GUI_CtrlSetEnable( ID_SAVE_SETTINGS, 0 );
			break;

		case GUI_MSG_CONTROL:
			switch( LOWORD(nCtrlParam) )
			{
				case ID_LOAD_SETTINGS:
					LoadSettings();
					break;

				case ID_SAVE_SETTINGS:
					if( SC_SaveConfig() )
					{
						GUI_DlgMsgBox( GUI_GetLangStr(LANG_STR_SETTINGS_SAVED),
									   GUI_GetLangStr(LANG_STR_OK), 0 );
					}
					else
					{
						GUI_DlgMsgBox( GUI_GetLangStr(LANG_STR_SETTINGS_ERROR),
									   GUI_GetLangStr(LANG_STR_ERROR), 0 );
					}
					break;

				case ID_NETWORK_SETTINGS:
					GUI_OpenMenu( GUI_MENU_NETWORK );
					break;

				case ID_PARTITION_SETTINGS:
					GUI_OpenMenu( GUI_MENU_PARTITIONS );
					break;

				case ID_SKIN_SETTINGS:
					GUI_OpenMenu( GUI_MENU_SKINS );
					break;

				case ID_LANGUAGE_SETTINGS:
					GUI_OpenMenu( GUI_MENU_LANGUAGE );
					break;

				case ID_SCREEN_MODE:
					if( nOther == 0 )
						nMode = GS_MODE_AUTO;
					else if( nOther == 1 )
						nMode = GS_MODE_NTSC;
					else
						nMode = GS_MODE_PAL;

					SC_SetValueForKey_Int( "scr_mode", nMode );
					break;

				case ID_SCREEN_ADJUST_X:
					switch( HIWORD(nCtrlParam) )
					{
						case GUI_NOT_SL_POS_CHANGED:
							pCtrl = GUI_ControlByID(ID_SCREEN_ADJUST_X);

							nValue = GUI_Ctrl_Slider_GetPos(pCtrl);
							SC_SetValueForKey_Int( "scr_adjust_x", nValue );

							gsLib_set_offset( SC_GetValueForKey_Int( "scr_adjust_x", NULL ),
											  SC_GetValueForKey_Int( "scr_adjust_y", NULL ) );
							break;
					}
					break;

				case ID_SCREEN_ADJUST_Y:
					switch( HIWORD(nCtrlParam) )
					{
						case GUI_NOT_SL_POS_CHANGED:
							pCtrl = GUI_ControlByID(ID_SCREEN_ADJUST_Y);

							nValue = GUI_Ctrl_Slider_GetPos(pCtrl);
							SC_SetValueForKey_Int( "scr_adjust_y", nValue );

							gsLib_set_offset( SC_GetValueForKey_Int( "scr_adjust_x", NULL ),
											  SC_GetValueForKey_Int( "scr_adjust_y", NULL ) );
							break;
					}
					break;

				case ID_SCREENSHOTS:
					SC_SetValueForKey_Int( "scr_screenshot", nOther );
					break;

				case ID_SCREENSHOTS_PATH:
					GUI_DlgKeyboard( SC_GetValueForKey_Str( "scr_path", NULL ),
									 GUI_GetLangStr(LANG_STR_ENTER_PATH),
									 DLG_SCREENSHOTS_PATH );
					break;

				case ID_CACHE_THUMBS:
					SC_SetValueForKey_Int( "tbn_caching", nOther );
					break;

				case ID_CACHE_THUMBS_PATH:
					GUI_DlgKeyboard( SC_GetValueForKey_Str( "tbn_cache_path", NULL ),
									 GUI_GetLangStr(LANG_STR_ENTER_PATH),
									 DLG_THUMBS_PATH );
					break;

				case ID_CACHE_SHOUTCAST:
					SC_SetValueForKey_Int( "radio_cache_list", nOther );
					break;

				case ID_GO_BACK:
					GUI_OpenMenu( GUI_MENU_MAIN );
					break;
			}
			break;

		case GUI_MSG_DIALOG:
			switch( LOWORD(nCtrlParam) )
			{
				case DLG_SCREENSHOTS_PATH:
					if( HIWORD(nCtrlParam) == DLG_RET_SUBMIT )
					{
						SC_SetValueForKey_Str( "scr_path", (const char*) nOther ); 
					}
					break;

				case DLG_THUMBS_PATH:
					if( HIWORD(nCtrlParam) == DLG_RET_SUBMIT )
					{
						SC_SetValueForKey_Str( "tbn_cache_path", (const char*) nOther );
					}
					break;
			}
			break;
	}
	return 0;
}

void LoadSettings( void )
{
	// clean up and reload settings
	SC_Clean();
	SC_LoadConfig(0);

	gsLib_set_offset( SC_GetValueForKey_Int( "scr_adjust_x", NULL ),
					  SC_GetValueForKey_Int( "scr_adjust_y", NULL ) );

	// this will re-init all controls
	GUI_OpenMenu( GUI_MENU_SETTINGS );
}

