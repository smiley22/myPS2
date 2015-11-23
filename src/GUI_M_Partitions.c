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
#include <libhdd.h>

#define ID_APPLY	100
#define ID_LIST		101

static void ListPartitions( void )
{
	int				hddFd, nRet, nNumEntries, i;
	iox_dirent_t	dirEnt;
	char			szPart[MAX_PATH + 1];
	char			szMntList[MAX_PATH + 1];
	char			*pToken;
	const char		*pString, *pBootPart;
	GUIControl_t	*pCtrl = GUI_ControlByID(ID_LIST);

	pBootPart = SC_GetValueForKey_Str( "hdd_boot_part", NULL );

	// get a list of partitions
	hddFd = fileXioDopen("hdd0:");

	if( hddFd < 0 )
		return;

	nRet = fileXioDread( hddFd, &dirEnt );

	while( nRet > 0 )
	{
		if( (dirEnt.stat.attr & ATTR_SUB_PARTITION) || (dirEnt.stat.mode == FS_TYPE_EMPTY) ||
			(!strncmp(dirEnt.name, "PP.HDL.", 7))   /*|| (!strncmp(dirEnt.name, "__", 2))*/||
			(!strcmp(dirEnt.name, "+MYPS2")) || (pBootPart && !strcmp(dirEnt.name, pBootPart)) )
		{
			nRet = fileXioDread( hddFd, &dirEnt );
			continue;
		}

		snprintf( szPart, sizeof(szPart), "hdd0:%s", dirEnt.name );
		GUI_Ctrl_List_AddItem( pCtrl, szPart, 0 );

		nRet = fileXioDread( hddFd, &dirEnt );
	}

	fileXioDclose( hddFd );

	// mark mounted partition
	nNumEntries = GUI_Ctrl_List_GetCount(pCtrl);

	SC_GetValueForKey_Str( "part_mount_list", szMntList );

	pToken = strtok( szMntList, "," );
	
	while(pToken)
	{
		for( i = 0; i < nNumEntries; i++ )
		{
			pString = GUI_Ctrl_List_GetItem( pCtrl, i );
			
			if( !strcmp( pString, pToken ) )
			{
				GUI_Ctrl_List_SetMarked( pCtrl, i, 1 );
				break;
			}
		}

		pToken = strtok( NULL, "," );
	}
}

static void ApplyChanges( void )
{
	int					numMarked, i;
	char				szMntList[MAX_PATH + 1] = {0};
	const char			*pString;
	const GUIControl_t	*pCtrl;

	pCtrl = GUI_ControlByID(ID_LIST);

	// get marked entries in list
	numMarked = GUI_Ctrl_List_NumMark(pCtrl);

	for( i = 0; i < numMarked; i++ )
	{
		pString = GUI_Ctrl_List_GetMarked( pCtrl, i );

		if( i < (numMarked - 1) )
		{
			snprintf( szMntList, sizeof(szMntList), "%s%s,",
					  szMntList, pString );
		}
		else
		{
			snprintf( szMntList, sizeof(szMntList), "%s%s",
					  szMntList, pString );
		}
	}

	SC_SetValueForKey_Str( "part_mount_list", szMntList );

	// re-mount partition list
	HDD_MountList();
}

unsigned int GUI_CB_Partitions( GUIMenu_t *lpGUIMenu, unsigned int nGUIMsg,
								unsigned int nCtrlParam, unsigned int nOther )
{
	GUIControl_t *pCtrl;

	switch( nGUIMsg )
	{
		case GUI_MSG_OPEN:
			pCtrl = GUI_ControlByID(ID_LIST);

			if( GUI_Ctrl_List_Empty(pCtrl) )
				ListPartitions();
			break;

		case GUI_MSG_CONTROL:
			switch( LOWORD(nCtrlParam) )
			{
				case ID_APPLY:
					ApplyChanges();
					GUI_DlgMsgBox( GUI_GetLangStr(LANG_STR_CHANGES_APPLIED),
								   GUI_GetLangStr(LANG_STR_OK), 0 );
					break;

				case ID_GO_BACK:
					GUI_OpenMenu( GUI_MENU_SETTINGS );
					break;
			}
			break;
	}
	return 0;
}
