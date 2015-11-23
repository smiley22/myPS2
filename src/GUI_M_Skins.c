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

#define ID_LIST				100

#define DLG_SKIN_CONFIRM	0

static void ListSkins( void )
{
	char			szPath[MAX_PATH + 1], *pStr;
	int				nNumFiles, i;
	fileInfo_t		*pFiles;
	FHANDLE			fHandle;
	unsigned int	nSize;
	u8				*pBuf;
	scew_parser		*pParser;
	scew_tree		*pTree;
	scew_element	*pElem, *pChild;
	const char		*pName;
	GUIControl_t	*pCtrl;

	pCtrl = GUI_ControlByID(ID_LIST);

	if( !(pFiles = malloc( sizeof(fileInfo_t) * MAX_DIR_FILES )) )
		return;

#ifdef _DEVELOPER
	snprintf( szPath, sizeof(szPath), "mc0:/BOOT/MYPS2/skins/" );
#else
	snprintf( szPath, sizeof(szPath), "%sskins/", GetElfPath() );
#endif
	
	nNumFiles = DirGetContents( szPath, NULL, pFiles, MAX_DIR_FILES );

	for( i = 0; i < nNumFiles; i++ )
	{
		if( !(pFiles[i].flags & FLAG_DIRECTORY) )
			continue;

		if(!strcmp( pFiles[i].name, ".." ) || !strcmp( pFiles[i].name, "." ))
			continue;

		// try to read name from skin.xml
#ifdef _DEVELOPER
		snprintf( szPath, sizeof(szPath), "mc0:/BOOT/MYPS2/skins/%s/skin.xml", pFiles[i].name );
#else
		snprintf( szPath, sizeof(szPath), "%sskins/%s/skin.xml",
				  GetElfPath(), pFiles[i].name );
#endif

		fHandle = FileOpen( szPath, O_RDONLY );
		if( fHandle.fh < 0 )
			continue;

		nSize = FileSeek( fHandle, 0, SEEK_END );
		FileSeek( fHandle, 0, SEEK_SET );

		if( !nSize )
		{
			FileClose(fHandle);
			continue;
		}

		if( !(pBuf = malloc( nSize )) )
		{
			FileClose(fHandle);
			return;
		}

		FileRead( fHandle, pBuf, nSize );
		FileClose(fHandle);

		if( (pParser = scew_parser_create()) == NULL )
		{
			free(pBuf);
			return;
		}

		if( !scew_parser_load_buffer( pParser, pBuf, nSize ) )
		{
			free(pBuf);
			continue;
		}

		if( (pTree = scew_parser_tree(pParser)) == NULL )
		{
			free(pBuf);
			scew_parser_free(pParser);
			continue;
		}

		if( (pElem = scew_tree_root(pTree)) == NULL )
		{
			free(pBuf);
			scew_tree_free(pTree);
			scew_parser_free( pParser );
			continue;
		}

		if( (pName = scew_element_name(pElem)) == NULL )
		{
			free(pBuf);
			scew_tree_free(pTree);
			scew_parser_free(pParser);
			continue;
		}

		if( strcmp( pName, "skin" ) )
		{
			free(pBuf);
			scew_tree_free(pTree);
			scew_parser_free(pParser);
			continue;
		}

		if( (pChild = scew_element_by_name( pElem, "name" )) == NULL )
		{
			scew_tree_free(pTree);
			scew_parser_free(pParser);
			continue;
		}

		if( (pName = scew_element_contents(pChild)) == NULL )
		{
			scew_tree_free(pTree);
			scew_parser_free(pParser);
			continue;
		}

		scew_tree_free(pTree);
		scew_parser_free(pParser);	
		free(pBuf);

		// associate skin path with list item
#ifdef _DEVELOPER
		snprintf( szPath, sizeof(szPath), "mc0:/BOOT/MYPS2/skins/%s", pFiles[i].name );
#else
		snprintf( szPath, sizeof(szPath), "%sskins/%s", GetElfPath(), pFiles[i].name );
#endif

		if( !(pStr = malloc( strlen(szPath) + 1 )) )
			return;

		strcpy( pStr, szPath );
		GUI_Ctrl_List_AddItem( pCtrl, pName, (unsigned int) pStr );
	}

	free(pFiles);
}

unsigned int GUI_CB_Skins( GUIMenu_t *lpGUIMenu, unsigned int nGUIMsg,
						   unsigned int nCtrlParam, unsigned int nOther )
{
	GUIControl_t	*pCtrl;
	unsigned int	nNum, i;
	char			*pStr;
	char			szSkin[MAX_PATH + 1];

	switch( nGUIMsg )
	{
		case GUI_MSG_OPEN:
			ListSkins();
			break;

		// free associated item data
		case GUI_MSG_CLOSE:
			pCtrl	= GUI_ControlByID(ID_LIST);
			nNum	= GUI_Ctrl_List_GetCount(pCtrl);

			for( i = 0; i < nNum; i++ )
			{
				pStr = (char*) GUI_Ctrl_List_GetItemData( pCtrl, i );
				free( pStr );
			}

			GUI_Ctrl_List_Clean(pCtrl);
			break;

		case GUI_MSG_CONTROL:
			switch( LOWORD(nCtrlParam) )
			{
				case ID_LIST:
					switch( HIWORD(nCtrlParam) )
					{
						case GUI_NOT_LIST_CLICK:
							GUI_DlgYesNo( GUI_GetLangStr(LANG_STR_SKIN_CONFIRM),
										  GUI_GetLangStr(LANG_STR_CONFIRM),
										  DLG_SKIN_CONFIRM );
							break;
					}
					break;

				case ID_GO_BACK:
					GUI_OpenMenu( GUI_MENU_SETTINGS );
					break;
			}
			break;

		case GUI_MSG_DIALOG:
			switch( LOWORD(nCtrlParam) )
			{
				case DLG_SKIN_CONFIRM:
					if( HIWORD(nCtrlParam) == DLG_RET_YES )
					{
						pCtrl	= GUI_ControlByID(ID_LIST);
						pStr	= (char*) GUI_Ctrl_List_GetItemData( pCtrl,
									GUI_Ctrl_List_GetSelIndex(pCtrl) );

						strncpy( szSkin, pStr, MAX_PATH );
						szSkin[MAX_PATH] = 0;

						pStr = strrchr( szSkin, '/' );
						if( !pStr )
							break;

						// skip '/'
						pStr++;

						// update skin name config value
						SC_SetValueForKey_Str( "skin_name", pStr );

						GUI_FreeSkin();
						GUI_LoadSkin(szSkin);

						// re-open menu so vram gets updated
						GUI_OpenMenu( GUI_MENU_SKINS );
					}
					break;
			}
			break;
	}
	return 0;
}
