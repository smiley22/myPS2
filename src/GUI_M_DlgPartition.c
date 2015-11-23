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

#define ID_BUTTON_NO	100
#define ID_BUTTON_YES	101

unsigned int GUI_CB_DlgPartition1( GUIMenu_t *lpGUIMenu, unsigned int nGUIMsg,
								   unsigned int nCtrlParam, unsigned int nOther )
{
	switch( nGUIMsg )
	{
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

#define ID_SLIDER		100
#define ID_SIZE_LABEL	101
#define ID_CANCEL		102
#define ID_CREATE		103

unsigned int GUI_CB_DlgPartition2( GUIMenu_t *lpGUIMenu, unsigned int nGUIMsg,
								   unsigned int nCtrlParam, unsigned int nOther )
{
	GUIControl_t *pCtrl;
	char szLabel[128];
	int nRet;
	u64 nPartSize;

	switch( nGUIMsg )
	{
		case GUI_MSG_OPEN:
			pCtrl = GUI_ControlByID(ID_SLIDER);
			GUI_Ctrl_Slider_SetBounds( pCtrl, 128, HDD_GetFreeSpace() );
			GUI_Ctrl_Slider_SetStep( pCtrl, 64 );
			GUI_Ctrl_Slider_SetPos( pCtrl, 128 );

			snprintf( szLabel, sizeof(szLabel), "%i MB", (u32)
					  GUI_Ctrl_Slider_GetPos(pCtrl) );

			pCtrl = GUI_ControlByID(ID_SIZE_LABEL);
			GUI_Ctrl_Label_SetText( pCtrl, szLabel );
			break;

		case GUI_MSG_CONTROL:
			switch( LOWORD(nCtrlParam) )
			{
				case ID_SLIDER:
					switch( HIWORD(nCtrlParam) )
					{
						case GUI_NOT_SL_POS_CHANGED:
							pCtrl = GUI_ControlByID(ID_SLIDER);
							snprintf( szLabel, sizeof(szLabel), "%i MB", (u32)
									  GUI_Ctrl_Slider_GetPos(pCtrl) );

							pCtrl = GUI_ControlByID(ID_SIZE_LABEL);
							GUI_Ctrl_Label_SetText( pCtrl, szLabel );

							GUI_Render();
							break;
					}
					break;

				case ID_CANCEL:
					GUI_CloseDialog(lpGUIMenu, DLG_RET_CANCEL, 0);
					break;

				// try to create partition
				case ID_CREATE:
					pCtrl		= GUI_ControlByID(ID_SLIDER);
					nPartSize	= GUI_Ctrl_Slider_GetPos(pCtrl);
					nRet		= HDD_CreatePartition( PARTITION_NAME, nPartSize );

					if( !nRet )
					{
						GUI_CloseDialog( lpGUIMenu, DLG_RET_CREATE, DLG_ERR_CREATE );
						return 0;
					}

					nRet		= HDD_MountPartition( "pfs0:", PARTITION_NAME );

					if( !nRet )
					{
						GUI_CloseDialog( lpGUIMenu, DLG_RET_CREATE, DLG_ERR_MOUNT );
						return 0;
					}

					GUI_CloseDialog(lpGUIMenu, DLG_RET_CREATE, DLG_ERR_NOERROR );
					break;

			}
			break;
	}
	return 0;
}

