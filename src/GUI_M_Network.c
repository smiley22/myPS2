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

#define ID_NET_ENABLE		100
#define ID_NET_IP			101
#define ID_NET_MASK			102
#define ID_NET_GATEWAY		103
#define ID_NET_DNS			104
#define ID_FTP_ENABLE		105
#define ID_FTP_PORT			106
#define ID_FTP_ANONYMOUS	107
#define ID_FTP_USERNAME		108
#define ID_FTP_PASSWORD		109
#define ID_SMB_SETTINGS		110

#define DLG_NET_IP			0
#define DLG_NET_MASK		1
#define DLG_NET_GATEWAY		2
#define DLG_NET_DNS			3
#define DLG_FTP_PORT		4
#define DLG_FTP_USERNAME	5
#define DLG_FTP_PASSWORD	6

unsigned int GUI_CB_Network( GUIMenu_t *lpGUIMenu, unsigned int nGUIMsg,
							 unsigned int nCtrlParam, unsigned int nOther )
{
	GUIControl_t *pCtrl;

	switch( nGUIMsg )
	{
		case GUI_MSG_OPEN:
			pCtrl = GUI_ControlByID(ID_NET_ENABLE);
			GUI_Ctrl_ToggleButton_SetState( pCtrl, SC_GetValueForKey_Int("net_enable",
											NULL ));

			pCtrl = GUI_ControlByID(ID_FTP_ENABLE);
			GUI_Ctrl_ToggleButton_SetState( pCtrl, SC_GetValueForKey_Int("ftp_enable",
											NULL ));

			pCtrl = GUI_ControlByID(ID_FTP_ANONYMOUS);
			GUI_Ctrl_ToggleButton_SetState( pCtrl, SC_GetValueForKey_Int("ftp_anonymous",
											NULL ));
			break;

		case GUI_MSG_CONTROL:
			switch( LOWORD(nCtrlParam) )
			{
				case ID_NET_ENABLE:
					SC_SetValueForKey_Int( "net_enable", nOther );
					break;

				case ID_NET_IP:
					GUI_DlgKeyboard( SC_GetValueForKey_Str( "net_ip", NULL ),
									 GUI_GetLangStr(LANG_STR_IP_ADDRESS),
									 DLG_NET_IP );
					break;

				case ID_NET_MASK:
					GUI_DlgKeyboard( SC_GetValueForKey_Str( "net_netmask", NULL ),
									 GUI_GetLangStr(LANG_STR_SUBNETMASK),
									 DLG_NET_MASK );
					break;

				case ID_NET_GATEWAY:
					GUI_DlgKeyboard( SC_GetValueForKey_Str( "net_gateway", NULL ),
									 GUI_GetLangStr(LANG_STR_GATEWAY),
									 DLG_NET_GATEWAY );
					break;

				case ID_NET_DNS:
					GUI_DlgKeyboard( SC_GetValueForKey_Str( "net_dns", NULL ),
									 GUI_GetLangStr(LANG_STR_DNS),
									 DLG_NET_DNS );
					break;

				case ID_FTP_ENABLE:
					SC_SetValueForKey_Int( "ftp_enable", nOther );
					break;

				case ID_FTP_PORT:
					GUI_DlgKeyboard( SC_GetValueForKey_Str( "ftp_port", NULL ),
									 GUI_GetLangStr(LANG_STR_FTP_PORT),
									 DLG_FTP_PORT );
					break;

				case ID_FTP_ANONYMOUS:
					SC_SetValueForKey_Int( "ftp_anonymous", nOther );
					break;

				case ID_FTP_USERNAME:
					GUI_DlgKeyboard( SC_GetValueForKey_Str( "ftp_login", NULL ),
									 GUI_GetLangStr(LANG_STR_FTP_USERNAME),
									 DLG_FTP_USERNAME );
					break;

				case ID_FTP_PASSWORD:
					GUI_DlgKeyboard( SC_GetValueForKey_Str( "ftp_password", NULL ),
									 GUI_GetLangStr(LANG_STR_FTP_PASSWORD),
									 DLG_FTP_PASSWORD );
					break;
					
				case ID_SMB_SETTINGS:
					GUI_OpenMenu( GUI_MENU_SAMBA );
					break;

				case ID_GO_BACK:
					GUI_OpenMenu( GUI_MENU_SETTINGS );
					break;
			}
			break;

		case GUI_MSG_DIALOG:
			if( HIWORD(nCtrlParam) == DLG_RET_SUBMIT )
			{
				switch( LOWORD(nCtrlParam) )
				{
					case DLG_NET_IP:
						SC_SetValueForKey_Str( "net_ip", (const char*) nOther );
						break;

					case DLG_NET_MASK:
						SC_SetValueForKey_Str( "net_netmask", (const char*) nOther );
						break;

					case DLG_NET_GATEWAY:
						SC_SetValueForKey_Str( "net_gateway", (const char*) nOther );
						break;

					case DLG_NET_DNS:
						SC_SetValueForKey_Str( "net_dns", (const char*) nOther );
						break;

					case DLG_FTP_PORT:
						SC_SetValueForKey_Str( "ftp_port", (const char*) nOther );
						break;

					case DLG_FTP_USERNAME:
						SC_SetValueForKey_Str( "ftp_login", (const char*) nOther );
						break;

					case DLG_FTP_PASSWORD:
						SC_SetValueForKey_Str( "ftp_password", (const char*) nOther );
						break;
				}
			}
			break;
	}
	return 0;
}
