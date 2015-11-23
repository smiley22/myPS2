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
#include <File.h>
#include <SysConf.h>
#include <Gamepad.h>
#include <Scheduler.h>
#include <Mp3.h>

GUIMenus_t GUIMenus[] =
{
	{	"main.xml",				GUI_CB_Main				},
	{	"mypictures.xml",		GUI_CB_MyPictures		},
	{	"dlg_thumb.xml",		GUI_CB_DlgThumb			},
	{	"myprograms.xml",		GUI_CB_MyPrograms		},
	{	"myfiles.xml",			GUI_CB_MyFiles			},
	{	"dlg_msgbox.xml",		GUI_CB_DlgMsgBox		},
	{	"dlg_yesno.xml",		GUI_CB_DlgYesNo			},
	{	"dlg_keyboard.xml",		GUI_CB_DlgKeyboard		},
	{	"dlg_copy.xml",			GUI_CB_DlgCopy			},
	{	"mymusic.xml",			GUI_CB_MyMusic			},
	{	"radio.xml",			GUI_CB_Radio			},
	{	"settings.xml",			GUI_CB_Settings			},
	{	"network.xml",			GUI_CB_Network			},
	{	"partitions.xml",		GUI_CB_Partitions		},
	{	"skins.xml",			GUI_CB_Skins			},
	{	"language.xml",			GUI_CB_Language			},
	{	"dlg_partition1.xml",	GUI_CB_DlgPartition1	},
	{	"dlg_partition2.xml",	GUI_CB_DlgPartition2	},
	{	"view.xml",				GUI_CB_View				},
	{	"samba.xml",			GUI_CB_Samba			}

};

GUI_t GUI;

int GUI_Init( void )
{
	char		szSkinPath[MAX_PATH + 1];
	char		szSkinName[MAX_PATH + 1];
	char		szLangFile[MAX_PATH + 1];
	char		szFileName[MAX_PATH + 1];

	memset( &GUI, 0, sizeof(GUI) );

	if( !SC_GetValueForKey_Str( "lang_file", szLangFile ) )
		return 0;

	if( !SC_GetValueForKey_Str( "skin_name", szSkinName ) )
		return 0;

	snprintf( szFileName, sizeof(szFileName), "%slanguage/%s", GetElfPath(), szLangFile );

	Bootscreen_printf("Loading GUI String Table...", szFileName);

	if( !GUI_LoadLangTable( szFileName ) ) {
		Bootscreen_printf("^2FAILED\n");
		return 0;
	}
	Bootscreen_printf("^1OK\n");

	snprintf( szSkinPath, sizeof(szSkinPath), "%sskins/%s", GetElfPath(), szSkinName );
	Bootscreen_printf("Parsing GUI XML files, this may take a few seconds\n");

	if( !GUI_LoadSkin( szSkinPath ) ) {
		Bootscreen_printf("Error while parsing GUI XML files!\n");
		return 0;
	}

	Bootscreen_printf("Everything loaded. Resetting GS and initializing GUI\n");
	Bootscreen_Shutdown();

	// reset GS, this will kill off the bootscreen
	gsLib_init( GS_MODE_AUTO, GS_DOUBLE_BUFFERING );
	gsLib_clear(colorBlack);
	gsLib_swap();

	gsLib_set_offset( SC_GetValueForKey_Int( "scr_adjust_x", NULL ),
					  SC_GetValueForKey_Int( "scr_adjust_y", NULL ) );

	GUI_OpenMenu( GUI_MENU_MAIN );
	return 1;
}

void GUI_Run( void *pThreadArgs )
{
	unsigned int	nPadBtns		= 0;
	unsigned int	nPadBtnsOld		= 0;
	static u64		nPlayTime		= 0;
	static time_t	nTimeUpdate		= 0;

	while(1)
	{
		// update timer display
		if( nPlayTime != MP3_GetCurrentTime() )
		{
			nPlayTime = MP3_GetCurrentTime();
			GUI_Render();
		}

		// update clock display once a minute
		if( nTimeUpdate < ps2time_time(NULL) )
		{
			nTimeUpdate = ps2time_time(NULL) + 60;
			GUI_Render();
		}

		if( !GP_GetPressMode() )
		{
			if( nPadBtns > nPadBtnsOld )
				GUI_Input( nPadBtns );
			else
				Scheduler_YieldThread();
		}
		else
		{
			if( nPadBtns > 0 )
				GUI_Input( nPadBtns );
		}

		nPadBtnsOld	= nPadBtns;
		nPadBtns	= GP_GetButtons();
	}
}

void GUI_Input( unsigned int nPadBtns )
{
	GUIControl_t	*pCtrl, *pNext;
	int				bHandled = 0;
	unsigned int	nID;

	if( GUI.pActiveMenu->pfnInput )
	{
		GUI.pActiveMenu->pfnInput( nPadBtns );
		return;
	}

	if( nPadBtns & PAD_SELECT )
		GUI_Screenshot();

	if( !(pCtrl = GUI_ControlByID(GUI.pActiveMenu->iSelID)) )
		return;

	switch(pCtrl->nType)
	{
		case GUI_CTRL_SLIDER:
			bHandled = GUI_Ctrl_Slider_Input(pCtrl, nPadBtns);
			break;

		case GUI_CTRL_THUMBNAILPANEL:
			bHandled = GUI_Ctrl_ThumbnailPanel_Input(pCtrl, nPadBtns);
			break;

		case GUI_CTRL_LIST:
			bHandled = GUI_Ctrl_List_Input(pCtrl, nPadBtns);
			break;

		case GUI_CTRL_DIRVIEW:
			bHandled = GUI_Ctrl_Dirview_Input(pCtrl, nPadBtns);
			break;

		case GUI_CTRL_TOGGLEBUTTON:
			bHandled = GUI_Ctrl_ToggleButton_Input(pCtrl, nPadBtns);
			break;

		case GUI_CTRL_COMBO:
			bHandled = GUI_Ctrl_Combo_Input(pCtrl, nPadBtns);
			break;

		default:
			break;
	}

	if( bHandled )
		return;

	if( nPadBtns & PAD_DOWN )
	{
		nID = pCtrl->nDownID;

		while( (pNext = GUI_ControlByID(nID)) )
		{
			if( pNext->nVisible && !pNext->nDisabled )
				break;

			nID = pNext->nDownID;
		}

		if( nID )
		{
			GUI.pActiveMenu->iSelID = nID;
			GUI_Render();
			GP_SetPressMode(0);
		}
	}
	else if( nPadBtns & PAD_LEFT )
	{
		nID = pCtrl->nLeftID;

		while( (pNext = GUI_ControlByID(nID)) )
		{
			if( pNext->nVisible && !pNext->nDisabled )
				break;
			
			nID = pNext->nLeftID;
		}

		if( nID )
		{
			GUI.pActiveMenu->iSelID = nID;
			GUI_Render();
			GP_SetPressMode(0);
		}
	}
	else if( nPadBtns & PAD_UP )
	{
		nID = pCtrl->nUpID;

		while( (pNext = GUI_ControlByID(nID)) )
		{
			if( pNext->nVisible && !pNext->nDisabled )
				break;

			nID = pNext->nUpID;
		}

		if( nID )
		{
			GUI.pActiveMenu->iSelID = nID;
			GUI_Render();
			GP_SetPressMode(0);
		}
	}
	else if( nPadBtns & PAD_RIGHT )
	{
		nID = pCtrl->nRightID;

		while( (pNext = GUI_ControlByID(nID)) )
		{
			if( pNext->nVisible && !pNext->nDisabled )
				break;

			nID = pNext->nRightID;
		}

		if( nID )
		{
			GUI.pActiveMenu->iSelID = nID;
			GUI_Render();
			GP_SetPressMode(0);
		}
	}
	else if( nPadBtns & PAD_CROSS )
	{
		GUI.pActiveMenu->pfnCallback(	GUI.pActiveMenu, GUI_MSG_CONTROL,
										MAKEPARAM( pCtrl->nID, GUI_NOT_CLICKED ), 0 );
	}
	else if( nPadBtns & PAD_TRIANGLE )
	{
		GUI.pActiveMenu->pfnCallback(	GUI.pActiveMenu, GUI_MSG_CONTROL,
										MAKEPARAM( ID_GO_BACK, GUI_NOT_CLICKED ), 0 );
	}
}

GUIControl_t *GUI_ControlByID( unsigned int nID )
{
	unsigned int i;

	for( i = 0; i < GUI.pActiveMenu->iNumControls; i++ )
	{
		if( nID == GUI.pActiveMenu->pControls[i].nID )
			return &GUI.pActiveMenu->pControls[i];
	}

	return NULL;
}

void GUI_CtrlSetVisible( GUIControl_t *pCtrl, unsigned int bVisible )
{
	if( pCtrl )
		pCtrl->nVisible = bVisible;
}

void GUI_CtrlSetEnable( unsigned int nID, unsigned int bEnable )
{
	GUIControl_t *pCtrl;

	if( (pCtrl = GUI_ControlByID(nID)) )
		pCtrl->nDisabled = !bEnable;
}

int GUI_LoadLangTable( const char *lpLangFile )
{
	GUI_t			*lpGUI;
	scew_parser		*pParser;
	scew_tree		*pTree;
	scew_element	*pElem, **pList, *pChild;
	const char		*pName;
	unsigned char	*pBuf;
	unsigned int	nNum, nSize, i;
	FHANDLE			fHandle;

	lpGUI = &GUI;

	fHandle = FileOpen( lpLangFile, O_RDONLY );

	if( fHandle.fh < 0 )
		return 0;

	nSize = FileSeek( fHandle, 0, SEEK_END );
	FileSeek( fHandle, 0, SEEK_SET );

	if( !nSize )
	{
		FileClose(fHandle);
		return 0;
	}

	if( !(pBuf = malloc( nSize )) )
	{
		FileClose(fHandle);
		return 0;
	}

	FileRead( fHandle, pBuf, nSize );
	FileClose(fHandle);

	if( (pParser = scew_parser_create()) == NULL )
	{
		free(pBuf);
		return 0;
	}

	if( !scew_parser_load_buffer( pParser, pBuf, nSize ) )
	{
		free(pBuf);
		return 0;
	}

	if( (pTree = scew_parser_tree(pParser)) == NULL )
	{
		free(pBuf);
		scew_parser_free(pParser);
		return 0;
	}

	if( (pElem = scew_tree_root(pTree)) == NULL )
	{
		free(pBuf);
		scew_tree_free(pTree);
		scew_parser_free( pParser );
		return 0;
	}

	if( (pName = scew_element_name(pElem)) == NULL )
	{
		free(pBuf);
		scew_tree_free(pTree);
		scew_parser_free(pParser);
		return 0;
	}

	if( strcmp( pName, "strings" ) )
	{
		free(pBuf);
		scew_tree_free(pTree);
		scew_parser_free(pParser);
		return 0;
	}

	if( (pList = scew_element_list( pElem, "string", &nNum )) == NULL )
	{
		free(pBuf);
		scew_tree_free(pTree);
		scew_parser_free(pParser);
		return 0;
	}

	lpGUI->LangTable.pEntry			= malloc( sizeof(GUIString_t) * nNum );
	lpGUI->LangTable.nNumEntries	= 0;
	
	if( !lpGUI->LangTable.pEntry )
	{
		free( pBuf );
		scew_tree_free(pTree);
		scew_parser_free(pParser);
		return 0;
	}

	for( i = 0; i < nNum; i++ )
	{
		pElem = *(pList + i);

		if( (pChild = scew_element_by_name( pElem, "id" )) == NULL )
			continue;

		if( (pName = scew_element_contents( pChild )) == NULL )
			continue;

		lpGUI->LangTable.pEntry[ lpGUI->LangTable.nNumEntries ].nStrID = atoi(pName);

		if( (pChild = scew_element_by_name( pElem, "value" )) == NULL )
			continue;

		if( (pName = scew_element_contents( pChild )) == NULL )
			continue;

		lpGUI->LangTable.pEntry[ lpGUI->LangTable.nNumEntries ].pGUIString =
			malloc( strlen(pName) + 1 );

		if( !lpGUI->LangTable.pEntry[ lpGUI->LangTable.nNumEntries ].pGUIString )
			break;

		strcpy( lpGUI->LangTable.pEntry[ lpGUI->LangTable.nNumEntries ].pGUIString, pName );

		lpGUI->LangTable.nNumEntries++;
	}

	free(pBuf);
	scew_element_list_free(pList);
	scew_tree_free(pTree);
	scew_parser_free(pParser);
	return 1;
}

int GUI_FreeLangTable( void )
{
	GUI_t *lpGUI = &GUI;
	unsigned int i;

	for( i = 0; i < lpGUI->LangTable.nNumEntries; i++ )
	{
		if( lpGUI->LangTable.pEntry[i].pGUIString )
			free( lpGUI->LangTable.pEntry[i].pGUIString );
	}

	if( lpGUI->LangTable.pEntry )
		free( lpGUI->LangTable.pEntry );

	lpGUI->LangTable.nNumEntries	= 0;
	lpGUI->LangTable.pEntry			= NULL;

	return 1;
}

const char *GUI_GetLangStr( unsigned int nStrID )
{
	unsigned int i;
	
	for( i = 0; i < GUI.LangTable.nNumEntries; i++ )
	{
		if( GUI.LangTable.pEntry[i].nStrID == nStrID )
			return GUI.LangTable.pEntry[i].pGUIString;
	}

	return NULL;
}

int GUI_LoadSkin( const char *lpSkinName )
{
	char			szFileName[MAX_PATH + 1];
	int				iNumMenuFiles;
	int				i, nSize;
	FHANDLE			fHandle;
	u8				*pBuf;
	scew_parser		*pParser;
	scew_tree		*pTree;
	scew_element	*pElem, *pChild;
	const char		*pName, *lpStr, *lpAlt;
	int				nMode, nGSMode;
	float			fScaleX, fScaleY;

	strncpy( szFileName, lpSkinName, sizeof(szFileName) );
	strncat( szFileName, "/skin.xml", sizeof(szFileName) );

	fHandle = FileOpen( szFileName, O_RDONLY );

	if( fHandle.fh < 0 )
		return 0;

	nSize = FileSeek( fHandle, 0, SEEK_END );
	FileSeek( fHandle, 0, SEEK_SET );

	if( !nSize )
	{
		FileClose(fHandle);
		return 0;
	}

	if( !(pBuf = malloc( nSize )) )
	{
		FileClose(fHandle);
		return 0;
	}

	FileRead( fHandle, pBuf, nSize );
	FileClose(fHandle);

	if( (pParser = scew_parser_create()) == NULL )
	{
		free(pBuf);
		return 0;
	}

	if( !scew_parser_load_buffer( pParser, pBuf, nSize ) )
	{
		free(pBuf);
		return 0;
	}

	if( (pTree = scew_parser_tree(pParser)) == NULL )
	{
		free(pBuf);
		scew_parser_free(pParser);
		return 0;
	}

	if( (pElem = scew_tree_root(pTree)) == NULL )
	{
		free(pBuf);
		scew_tree_free(pTree);
		scew_parser_free( pParser );
		return 0;
	}

	if( (pName = scew_element_name(pElem)) == NULL )
	{
		free(pBuf);
		scew_tree_free(pTree);
		scew_parser_free(pParser);
		return 0;
	}

	if( strcmp( pName, "skin" ) )
	{
		free(pBuf);
		scew_tree_free(pTree);
		scew_parser_free(pParser);
		return 0;
	}

	if( (pChild = scew_element_by_name( pElem, "default" )) == NULL )
	{
		scew_tree_free(pTree);
		scew_parser_free(pParser);
		return 0;
	}

	if( (pName = scew_element_contents(pChild)) == NULL )
	{
		scew_tree_free(pTree);
		scew_parser_free(pParser);
		return 0;
	}

	scew_tree_free(pTree);
	scew_parser_free(pParser);	
	free(pBuf);

	if( !stricmp( pName, "NTSC" ) )
		nMode = GS_MODE_NTSC;
	else
		nMode = GS_MODE_PAL;

	nGSMode = gsLib_get_mode();

	lpStr = nGSMode == GS_MODE_PAL ? "pal" : "ntsc";
	lpAlt = nMode	== GS_MODE_PAL ? "pal" : "ntsc";

	// load font.xml
	snprintf(	szFileName, sizeof(szFileName), "%s/%s/font.xml", lpSkinName,
				lpStr );

	if( !GUI_LoadFonts( szFileName, &GUI ) )
	{
		// try to load font.xml from default directory
		snprintf(	szFileName, sizeof(szFileName), "%s/%s/font.xml", lpSkinName,
					lpAlt );

		if( !GUI_LoadFonts( szFileName, &GUI ) )
			return 0;
	}

	iNumMenuFiles = sizeof(GUIMenus) / sizeof(GUIMenus_t);

	// load skin xml files
	for( i = 0; i < iNumMenuFiles; i++ )
	{
		snprintf(	szFileName, sizeof(szFileName), "%s/%s/%s", lpSkinName,
					lpStr, GUIMenus[i].pFileName );

		if( !GUI_LoadMenu( szFileName, &GUI, &GUI.pGUIMenus[i], 1.0f, 1.0f ) )
		{
			if( nGSMode == nMode )
				return 0;

			if( nGSMode == GS_MODE_PAL && nMode == GS_MODE_NTSC )
			{
				fScaleX = 640.0f / 640.0f;
				fScaleY = 512.0f / 448.0f;
			}
			else
			{
				fScaleX = 640.0f / 640.0f;
				fScaleY = 448.0f / 512.0f;
			}

			// try to load skin from skin default directory
			snprintf(	szFileName, sizeof(szFileName), "%s/%s/%s", lpSkinName,
						lpAlt, GUIMenus[i].pFileName );

			if( !GUI_LoadMenu( szFileName, &GUI, &GUI.pGUIMenus[i], fScaleX, fScaleY ) )
				return 0;
		}

		GUI.pGUIMenus[i].pfnCallback = GUIMenus[i].pfnCallback;
	}

	return 1;
}

int GUI_FreeSkin( void )
{
	GUI_t *lpGUI;
	unsigned int i, c;
	GUIControl_t *pCtrl;

	lpGUI = &GUI;
	lpGUI->pActiveMenu->pfnCallback( lpGUI->pActiveMenu, GUI_MSG_CLOSE, 0, 0 );

	// free GUI Image Storage Table
	for( i = 0; i < lpGUI->nNumGUIImages; i++ )
	{
		free( lpGUI->pGUIImages[i].pImage );
		free( lpGUI->pGUIImages[i].pImageName );
	}

	// free GUI Font Storage Table
	for( i = 0; i < lpGUI->nNumGUIFonts; i++ )
	{
		free( lpGUI->pGUIFonts[i].pFontName );
		free( lpGUI->pGUIFonts[i].pPNGFile );
		free( lpGUI->pGUIFonts[i].pDATFile );
	}

	// free GS images
	for( i = 0; i < lpGUI->pActiveMenu->iNumImages; i++ )
		gsLib_texture_free( lpGUI->pActiveMenu->pImages[i].gsTexture );

	// free GS fonts
	for( i = 0; i < lpGUI->pActiveMenu->iNumFonts; i++ )
		gsLib_font_destroy( lpGUI->pActiveMenu->pFonts[i].gsFont );

	lpGUI->nNumGUIImages	= 0;
	lpGUI->nNumGUIFonts		= 0;
	lpGUI->pGUIImages		= NULL;
	lpGUI->pActiveMenu		= NULL;

	for( i = 0; i < GUI_MENU_NUM; i++ )
	{
		free( lpGUI->pGUIMenus[i].pImages );

		for( c = 0; c < lpGUI->pGUIMenus[i].iNumControls; c++ )
		{
			pCtrl = &lpGUI->pGUIMenus[i].pControls[c];

			if( pCtrl->nType == GUI_CTRL_LABEL )
			{
				if( ((GUICtrl_Label_t*) pCtrl->pCtrl)->lpStr )
					free( ((GUICtrl_Label_t*) pCtrl->pCtrl)->lpStr );
			}
			else if( pCtrl->nType == GUI_CTRL_BUTTON )
			{
				if( ((GUICtrl_Button_t*) pCtrl->pCtrl)->lpStr )
					free( ((GUICtrl_Button_t*) pCtrl->pCtrl)->lpStr );
			}
			else if( pCtrl->nType == GUI_CTRL_TOGGLEBUTTON )
			{
				if( ((GUICtrl_ToggleButton_t*) pCtrl->pCtrl)->lpStr )
					free( ((GUICtrl_ToggleButton_t*) pCtrl->pCtrl)->lpStr );
			}

			free( lpGUI->pGUIMenus[i].pControls[c].pCtrl );
		}

		free( lpGUI->pGUIMenus[i].pControls );

		lpGUI->pGUIMenus[i].iNumControls	= 0;
		lpGUI->pGUIMenus[i].iNumImages		= 0;
		lpGUI->pGUIMenus[i].pControls		= NULL;
		lpGUI->pGUIMenus[i].pImages			= NULL;
	}

	// for every menu
	//	free allocated video ram
	//	free menuImages
	//	for every control
	//		free control pointer
	//		free imageIndex Table if control uses one
	//	free controls array

	return 1;
}

int GUI_ImageLookup( const GUI_t *lpGUI, const char *lpImageName )
{
	unsigned int i;

	for( i = 0; i < lpGUI->nNumGUIImages; i++ )
	{
		if( !strcmp( lpImageName, lpGUI->pGUIImages[i].pImageName ) )
			return i;
	}

	return -1;
}

int GUI_ImageAdd( GUI_t *lpGUI, const char *lpImageName )
{
	char			szSkinName[MAX_PATH + 1];
	char			szFileName[MAX_PATH + 1];
	FHANDLE			fHandle;
	int				nSize;
	unsigned char	*pBuf;
	GUIImageStore_t	*pImages;

	if( !SC_GetValueForKey_Str( "skin_name", szSkinName ) )
		return -1;

	snprintf( szFileName, sizeof(szFileName), "%sskins/%s/media/%s", GetElfPath(), szSkinName, lpImageName );

	fHandle = FileOpen( szFileName, O_RDONLY );
	if( fHandle.fh < 0 )
		return -1;

	nSize = FileSeek( fHandle, 0, SEEK_END );
	FileSeek( fHandle, 0, SEEK_SET );

	if( !nSize ) {
		FileClose(fHandle);
		return -1;
	}

	if( (pBuf = malloc( nSize )) == NULL )
		return -1;

	FileRead( fHandle, pBuf, nSize );
	FileClose(fHandle);

	pImages = malloc( sizeof(GUIImageStore_t) * (lpGUI->nNumGUIImages + 1) );

	if( !pImages )
		return -1;

	if( lpGUI->pGUIImages )
	{
		memcpy( pImages, lpGUI->pGUIImages, lpGUI->nNumGUIImages * sizeof(GUIImageStore_t) );
		free(lpGUI->pGUIImages);
	}

	lpGUI->pGUIImages = pImages;

	pImages = &lpGUI->pGUIImages[ lpGUI->nNumGUIImages ];

	pImages->nImageSize	= nSize;
	pImages->pImage		= pBuf;

	if( (pImages->pImageName = malloc( strlen(lpImageName) + 1 )) == NULL )
		return -1;

	strcpy( pImages->pImageName, lpImageName );

	lpGUI->nNumGUIImages++;

	return (lpGUI->nNumGUIImages - 1);
}

const GUIMenuImage_t *GUI_MenuGetImage( const GUIMenu_t *lpGUIMenu, unsigned int nIndex )
{
	unsigned int i;

	for( i = 0; i < lpGUIMenu->iNumImages; i++ )
	{
		if( lpGUIMenu->pImages[i].nIndex == nIndex )
			return &lpGUIMenu->pImages[i];
	}

	return NULL;
}

int GUI_MenuAddImage( GUIMenu_t *lpGUIMenu, unsigned int nImageIndex )
{
	GUIMenuImage_t *pImages;

	pImages = malloc( sizeof(GUIMenuImage_t) * (lpGUIMenu->iNumImages + 1) );
	if( !pImages )
		return -1;

	if( lpGUIMenu->pImages )
	{
		memcpy( pImages, lpGUIMenu->pImages, lpGUIMenu->iNumImages * sizeof(GUIMenuImage_t) );
		free( lpGUIMenu->pImages );
	}

	lpGUIMenu->pImages = pImages;
	pImages = &lpGUIMenu->pImages[ lpGUIMenu->iNumImages ];

	pImages->nIndex		= nImageIndex;
	pImages->gsTexture	= NULL;

	lpGUIMenu->iNumImages++;
	return 1;
}

int GUI_LoadFonts( const char *lpFileName, GUI_t *lpGUI )
{
	int				i, nSize, nNum;
	FHANDLE			fHandle;
	u8				*pBuf;
	scew_parser		*pParser;
	scew_tree		*pTree;
	scew_element	*pElem, **pList, *pChild, *pDefault, *pFontset;
	const char		*pName, *pStr;
	scew_attribute	*pAttrib;
	char			szCharset[MAX_PATH + 1];

	SC_GetValueForKey_Str( "lang_charset", szCharset );

	fHandle = FileOpen( lpFileName, O_RDONLY );

	if( fHandle.fh < 0 )
		return 0;

	nSize = FileSeek( fHandle, 0, SEEK_END );
	FileSeek( fHandle, 0, SEEK_SET );

	if( !nSize )
	{
		FileClose(fHandle);
		return 0;
	}

	if( !(pBuf = malloc( nSize )) )
	{
		FileClose(fHandle);
		return 0;
	}

	FileRead( fHandle, pBuf, nSize );
	FileClose(fHandle);

	if( (pParser = scew_parser_create()) == NULL )
	{
		free(pBuf);
		return 0;
	}

	if( !scew_parser_load_buffer( pParser, pBuf, nSize ) )
	{
		free(pBuf);
		return 0;
	}

	if( (pTree = scew_parser_tree(pParser)) == NULL )
	{
		free(pBuf);
		scew_parser_free(pParser);
		return 0;
	}

	if( (pElem = scew_tree_root(pTree)) == NULL )
	{
		free(pBuf);
		scew_tree_free(pTree);
		scew_parser_free( pParser );
		return 0;
	}

	if( (pName = scew_element_name(pElem)) == NULL )
	{
		free(pBuf);
		scew_tree_free(pTree);
		scew_parser_free(pParser);
		return 0;
	}

	if( strcmp( pName, "fonts" ) )
	{
		free(pBuf);
		scew_tree_free(pTree);
		scew_parser_free(pParser);
		return 0;
	}

	if( (pList = scew_element_list( pElem, "fontset", &nNum )) == NULL )
	{
		free(pBuf);
		scew_tree_free(pTree);
		scew_parser_free(pParser);
		return 0;
	}

	pDefault	= NULL;
	pFontset	= NULL;

	for( i = 0; i < nNum; i++ )
	{
		pElem = *(pList + i);

		if( (pAttrib = scew_attribute_by_name( pElem, "charset")) == NULL )
			continue;

		if( (pStr = scew_attribute_value(pAttrib)) == NULL )
			continue;

		if( !strcmp( pStr, DEFAULT_CHARSET ) )
			pDefault = pElem;

		if( !strcmp( pStr, szCharset ) )
		{
			pFontset = pElem;
			break;
		}
	}

	scew_element_list_free(pList);

	if( pFontset == NULL )
	{
		printf("Did not find fontset for charset %s, using default\n", szCharset);
		pFontset = pDefault;
	}

	// skin doesn't have default fontset
	if( pFontset == NULL )
	{
		printf("Error: default fontset (%s) not found\n", DEFAULT_CHARSET);

		free(pBuf);
		scew_tree_free(pTree);
		scew_parser_free(pParser);
		return 0;
	}

	if( (pList = scew_element_list( pFontset, "font", &nNum )) == NULL )
	{
		free(pBuf);
		scew_tree_free(pTree);
		scew_parser_free(pParser);
		return 0;
	}

	for( i = 0; i < nNum; i++ )
	{
		pElem = *(pList + i);

		// parse name field
		if( (pChild = scew_element_by_name( pElem, "name" )) == NULL )
			continue;

		if( (pName = scew_element_contents(pChild)) == NULL )
			continue;

		// parse filename
		if( (pChild = scew_element_by_name( pElem, "file" )) == NULL )
			continue;

		if( (pStr = scew_element_contents(pChild)) == NULL )
			continue;

		if( GUI_FontAdd( lpGUI, pName, pStr ) < 0 )
			break;
	}

	scew_element_list_free(pList);
	scew_tree_free(pTree);
	scew_parser_free(pParser);	
	free(pBuf);
	return 1;
}

int GUI_FontAdd( GUI_t *lpGUI, const char *lpFontName, const char *lpFileName )
{
	char			szSkinName[MAX_PATH + 1];
	char			szFileName[MAX_PATH + 1];
	FHANDLE			fHandle;
	GUIFontStore_t	*pFonts;

	char			szDAT[MAX_PATH + 1];
	unsigned char	*pPNGFile, *pDATFile;
	u32				nPNGSize, nDATSize;

	if( !SC_GetValueForKey_Str( "skin_name", szSkinName ) )
		return -1;

	// try to load PNG file from skin's fonts directory first
	snprintf( szFileName, sizeof(szFileName), "%sskins/%s/fonts/%s", GetElfPath(),
			  szSkinName, lpFileName );

	fHandle = FileOpen( szFileName, O_RDONLY );
	if( fHandle.fh < 0 )
	{
		// load from base font directory
		snprintf( szFileName, sizeof(szFileName), "%sfonts/%s", GetElfPath(), lpFileName );
		fHandle = FileOpen( szFileName, O_RDONLY );
	}

	if( fHandle.fh < 0 )
		return -1;

	nPNGSize = FileSeek( fHandle, 0, SEEK_END );
	FileSeek( fHandle, 0, SEEK_SET );

	if( !nPNGSize ) {
		FileClose(fHandle);
		return -1;
	}

	if( (pPNGFile = malloc( nPNGSize )) == NULL )
		return -1;

	FileRead( fHandle, pPNGFile, nPNGSize );
	FileClose(fHandle);

	// load DAT file
	StripFileExt( szDAT, lpFileName );
	strncat( szDAT, ".dat", sizeof(szDAT) );

	snprintf( szFileName, sizeof(szFileName), "%sskins/%s/fonts/%s", GetElfPath(),
			  szSkinName, szDAT );

	fHandle = FileOpen( szFileName, O_RDONLY );
	if( fHandle.fh < 0 )
	{
		snprintf( szFileName, sizeof(szFileName), "%sfonts/%s", GetElfPath(), szDAT );
		fHandle = FileOpen( szFileName, O_RDONLY );
	}

	if( fHandle.fh < 0 )
		return -1;

	nDATSize = FileSeek( fHandle, 0, SEEK_END );
	FileSeek( fHandle, 0, SEEK_SET );

	if( !nDATSize ) {
		FileClose(fHandle);
		return -1;
	}

	if( (pDATFile = malloc( nDATSize )) == NULL )
		return -1;

	FileRead( fHandle, pDATFile, nDATSize );
	FileClose(fHandle);


	// add font to list
	pFonts	= malloc( sizeof(GUIFontStore_t) * (lpGUI->nNumGUIFonts + 1) );

	if( !pFonts )
		return -1;

	if( lpGUI->pGUIFonts )
	{
		memcpy( pFonts, lpGUI->pGUIFonts, lpGUI->nNumGUIFonts * sizeof(GUIFontStore_t) );
		free(lpGUI->pGUIFonts);
	}

	lpGUI->pGUIFonts = pFonts;

	pFonts = &lpGUI->pGUIFonts[ lpGUI->nNumGUIFonts ];

	pFonts->nDATSize	= nDATSize;
	pFonts->pDATFile	= pDATFile;
	pFonts->nPNGSize	= nPNGSize;
	pFonts->pPNGFile	= pPNGFile;

	if( (pFonts->pFontName = malloc( strlen(lpFontName) + 1 )) == NULL )
		return -1;

	strcpy( pFonts->pFontName, lpFontName );

	lpGUI->nNumGUIFonts++;

	return (lpGUI->nNumGUIFonts - 1);
}

int GUI_FontLookup( const GUI_t *lpGUI, const char *lpFontName )
{
	unsigned int i;

	for( i = 0; i < lpGUI->nNumGUIFonts; i++ )
	{
		if( !strcmp( lpFontName, lpGUI->pGUIFonts[i].pFontName ) )
			return i;
	}

	return -1;
}

const GUIMenuFont_t *GUI_MenuGetFont( const GUIMenu_t *lpGUIMenu, unsigned int nIndex )
{
	unsigned int i;

	for( i = 0; i < lpGUIMenu->iNumFonts; i++ )
	{
		if( lpGUIMenu->pFonts[i].nIndex == nIndex )
			return &lpGUIMenu->pFonts[i];
	}

	return NULL;
}

int GUI_MenuAddFont( GUIMenu_t *lpGUIMenu, unsigned int nFontIndex )
{
	GUIMenuFont_t *pFonts;

	pFonts = malloc( sizeof(GUIMenuFont_t) * (lpGUIMenu->iNumFonts + 1) );
	if( !pFonts )
		return -1;

	if( lpGUIMenu->pFonts )
	{
		memcpy( pFonts, lpGUIMenu->pFonts, lpGUIMenu->iNumFonts * sizeof(GUIMenuFont_t) );
		free( lpGUIMenu->pFonts );
	}

	lpGUIMenu->pFonts = pFonts;
	pFonts = &lpGUIMenu->pFonts[ lpGUIMenu->iNumFonts ];

	pFonts->nIndex		= nFontIndex;
	pFonts->gsFont		= NULL;

	lpGUIMenu->iNumFonts++;
	return 1;
}

void GUI_OpenMenu( unsigned int nMenuID )
{
	unsigned int	i;
	GUIMenu_t		*pMenu = &GUI.pGUIMenus[ nMenuID ];
	unsigned char	*pRGB;
	int				nPSM;

	if( GUI.pActiveMenu )
	{
		GUI.pActiveMenu->pfnCallback( GUI.pActiveMenu, GUI_MSG_CLOSE, 0, 0 );

		for( i = 0; i < GUI.pActiveMenu->iNumImages; i++ )
			gsLib_texture_free( GUI.pActiveMenu->pImages[i].gsTexture );

		for( i = 0; i < GUI.pActiveMenu->iNumFonts; i++ )
			gsLib_font_destroy( GUI.pActiveMenu->pFonts[i].gsFont );
	}

	// upload needed textures from EE memory to GS video ram
	for( i = 0; i < pMenu->iNumImages; i++ )
	{
		if( CmpFileExtension( GUI.pGUIImages[ pMenu->pImages[i].nIndex ].pImageName, "JPG" ) )
		{
			jpgData	*pJpg;

			pJpg = jpgOpenRAW(	GUI.pGUIImages[ pMenu->pImages[i].nIndex ].pImage,
								GUI.pGUIImages[ pMenu->pImages[i].nIndex ].nImageSize,
								JPG_NORMAL );

			if( !pJpg )
				continue;

			pRGB = memalign( 128, pJpg->width * pJpg->height * (pJpg->bpp >> 3) );

			if( !pRGB )
				continue;

			if( jpgReadImage( pJpg, pRGB ) == - 1 )
				continue;

			pMenu->pImages[i].gsTexture = gsLib_texture_raw( pJpg->width, pJpg->height,
															 GS_PSM_CT24, pRGB, GS_CLUT_NONE,
															 NULL );

			jpgClose(pJpg);
			free( pRGB );
		}
		else if( CmpFileExtension( GUI.pGUIImages[ pMenu->pImages[i].nIndex ].pImageName, "PNG" ) )
		{
			pngData *pPng;

			pPng = pngOpenRAW(	GUI.pGUIImages[ pMenu->pImages[i].nIndex ].pImage,
								GUI.pGUIImages[ pMenu->pImages[i].nIndex ].nImageSize );


			if( !pPng )
				continue;

			pRGB = memalign( 128, pPng->width * pPng->height * (pPng->bpp >> 3) );

			if( !pRGB )
				break;

			if( pngReadImage( pPng, pRGB ) == -1 )
				continue;

			nPSM = pPng->bpp == 32 ? GS_PSM_CT32 : GS_PSM_CT24;

			pMenu->pImages[i].gsTexture = gsLib_texture_raw( pPng->width, pPng->height,
															 nPSM, pRGB, GS_CLUT_NONE,
															 NULL );

			pngClose(pPng);
			free( pRGB );
		}
	}

	// create fonts and upload them from EE to GS
	for( i = 0; i < pMenu->iNumFonts; i++ )
	{
		pMenu->pFonts[i].gsFont = gsLib_font_create(
			GUI.pGUIFonts[ pMenu->pFonts[i].nIndex ].pPNGFile,
			GUI.pGUIFonts[ pMenu->pFonts[i].nIndex ].nPNGSize,
			GUI.pGUIFonts[ pMenu->pFonts[i].nIndex ].pDATFile,
			GUI.pGUIFonts[ pMenu->pFonts[i].nIndex ].nDATSize );
	}

	GUI.pActiveMenu = pMenu;

	pMenu->pfnCallback( GUI.pActiveMenu, GUI_MSG_OPEN, 0, 0 );

	GUI_Render();
}

void GUI_OpenDialog( unsigned int nMenuID, GUIMenu_t *pParent, unsigned int nDlgID )
{
	GUIMenu_t		*pMenu = &GUI.pGUIMenus[ nMenuID ];

	pMenu->pParent	= pParent;
	pMenu->nDlgID	= nDlgID;

	// parent is not closed so it must not free its graphics
	// in the video ram
	GUI.pActiveMenu = NULL;

	GUI_OpenMenu( nMenuID );
}

void GUI_CloseDialog( GUIMenu_t *pDialog, unsigned int nExitCode, unsigned int nRetValue )
{
	unsigned int i;

	pDialog->pfnCallback( pDialog, GUI_MSG_CLOSE, 0, 0 );

	for( i = 0; i < pDialog->iNumImages; i++ )
		gsLib_texture_free( pDialog->pImages[i].gsTexture );

	for( i = 0; i < pDialog->iNumFonts; i++ )
		gsLib_font_destroy( pDialog->pFonts[i].gsFont );

	GUI.pActiveMenu		= pDialog->pParent;

	pDialog->pParent->pfnCallback(	pDialog->pParent, GUI_MSG_DIALOG,
									MAKEPARAM( pDialog->nDlgID, nExitCode ),
									nRetValue );

	pDialog->pParent	= NULL;
	pDialog->nDlgID		= 0;

	GUI_Render();
}

void GUI_RenderMenu( const GUIMenu_t *pMenu )
{
	unsigned int i;

	for( i = 0; i < pMenu->iNumControls; i++ )
	{
		if( !pMenu->pControls[i].nVisible )
			continue;

		switch( pMenu->pControls[i].nType )
		{
			case GUI_CTRL_LABEL:
				GUI_Ctrl_Label_Draw( &pMenu->pControls[i] );
				break;

			case GUI_CTRL_BUTTON:
				GUI_Ctrl_Button_Draw( &pMenu->pControls[i] );
				break;

			case GUI_CTRL_IMAGE:
				GUI_Ctrl_Image_Draw( &pMenu->pControls[i] );
				break;

			case GUI_CTRL_SLIDER:
				GUI_Ctrl_Slider_Draw( &pMenu->pControls[i] );
				break;

			case GUI_CTRL_PROGRESS:
				GUI_Ctrl_Progress_Draw( &pMenu->pControls[i] );
				break;

			case GUI_CTRL_THUMBNAILPANEL:
				GUI_Ctrl_ThumbnailPanel_Draw( &pMenu->pControls[i] );
				break;

			case GUI_CTRL_LIST:
				GUI_Ctrl_List_Draw( &pMenu->pControls[i] );
				break;

			case GUI_CTRL_DIRVIEW:
				GUI_Ctrl_Dirview_Draw( &pMenu->pControls[i] );
				break;

			case GUI_CTRL_TOGGLEBUTTON:
				GUI_Ctrl_ToggleButton_Draw( &pMenu->pControls[i] );
				break;

			case GUI_CTRL_COMBO:
				GUI_Ctrl_Combo_Draw( &pMenu->pControls[i] );
				break;
		}
	}
}

void GUI_Render( void )
{
	GUIMenu_t *pMenu = GUI.pActiveMenu;

	if( !pMenu )
		return;

	if( pMenu->pfnRender )
	{
		pMenu->pfnRender();
		return;
	}

	gsLib_clear(colorBlack);

	GUI_UpdateSpecialCtrls(pMenu);

	if( pMenu->pParent )
	{
		GUI_UpdateSpecialCtrls( pMenu->pParent );
		GUI_RenderMenu( pMenu->pParent );
	}

	GUI_RenderMenu( pMenu );
	gsLib_swap();
}

void GUI_UpdateSpecialCtrls( GUIMenu_t *pMenu )
{
	unsigned int i;
	char szLabel[MAX_PATH + 1];
	const char *pStr;
	int nMins, nSecs;
	time_t nTimeStamp;
	struct tm *pTimeNow;
	int nWDayStr;

	ps2time_time(&nTimeStamp);
	pTimeNow = ps2time_localtime(&nTimeStamp);
	nWDayStr = pTimeNow->tm_wday ? pTimeNow->tm_wday - 1 : 6;

	// update controls with special ids (1-100)
	for( i = 0; i < pMenu->iNumControls; i++ )
	{
		if( pMenu->pControls[i].nID > 100 )
			continue;

		switch(pMenu->pControls[i].nID)
		{
			// MP3 IDs
			case ID_MP3_PLAYTIME:
				if( MP3_GetStatus() == MP3_STOPPED )
				{
					GUI_CtrlSetVisible( &pMenu->pControls[i], 0 );
				}
				else
				{
					nSecs	= MP3_GetCurrentTime();
					nMins	= nSecs / 60;
					nSecs	= nSecs % 60;

					snprintf( szLabel, sizeof(szLabel), "%02i:%02i",
							  nMins, nSecs );

					GUI_CtrlSetVisible( &pMenu->pControls[i], 1 );
					GUI_Ctrl_Label_SetText( &pMenu->pControls[i], szLabel );
				}
				break;

			case ID_MP3_TRACKLENGTH:
				if( MP3_GetStatus() == MP3_STOPPED )
				{
					GUI_CtrlSetVisible( &pMenu->pControls[i], 0 );
				}
				else
				{
					nSecs	= MP3_GetTrackLength();
					nMins	= nSecs / 60;
					nSecs	= nSecs % 60;

					snprintf( szLabel, sizeof(szLabel), "%02i:%02i",
							  nMins, nSecs );

					GUI_CtrlSetVisible( &pMenu->pControls[i], 1 );
					GUI_Ctrl_Label_SetText( &pMenu->pControls[i], szLabel );
				}
				break;

			case ID_MP3_TRACKNAME:
				if( MP3_GetStatus() == MP3_STOPPED )
				{
					GUI_CtrlSetVisible( &pMenu->pControls[i], 0 );
				}
				else
				{
					pStr = MP3_GetTrackName();

					GUI_CtrlSetVisible( &pMenu->pControls[i], 1 );
					GUI_Ctrl_Label_SetText( &pMenu->pControls[i], pStr );
				}
				break;

			case ID_MP3_ISPLAYING:
				if( MP3_GetStatus() == MP3_STOPPED )
				{
					GUI_CtrlSetVisible( &pMenu->pControls[i], 0 );
				}
				else
				{
					GUI_CtrlSetVisible( &pMenu->pControls[i], 1 );
				}
				break;

			// TIME IDs
			case ID_TIME_DATESTRING:
				snprintf( szLabel, sizeof(szLabel), "%s, %s %02i",
						  GUI_GetLangStr( LANG_STR_MONDAY  + nWDayStr ),
						  GUI_GetLangStr( LANG_STR_JANUARY + pTimeNow->tm_mon ),
						  pTimeNow->tm_mday );

				GUI_Ctrl_Label_SetText( &pMenu->pControls[i], szLabel );
				break;

			case ID_TIME_WEEKDAY:
				snprintf( szLabel, sizeof(szLabel), "%s",
						  GUI_GetLangStr( LANG_STR_MONDAY + nWDayStr ) );

				GUI_Ctrl_Label_SetText( &pMenu->pControls[i], szLabel );
				break;

			case ID_TIME_DAY:
				snprintf( szLabel, sizeof(szLabel), "%02i", pTimeNow->tm_mday );
				GUI_Ctrl_Label_SetText( &pMenu->pControls[i], szLabel );
				break;

			case ID_TIME_MONTH_STR:
				snprintf( szLabel, sizeof(szLabel), "%s",
						  GUI_GetLangStr( LANG_STR_JANUARY + pTimeNow->tm_mon ) );

				GUI_Ctrl_Label_SetText( &pMenu->pControls[i], szLabel );
				break;

			case ID_TIME_MONTH:
				snprintf( szLabel, sizeof(szLabel), "%02i", pTimeNow->tm_mon + 1 );
				GUI_Ctrl_Label_SetText( &pMenu->pControls[i], szLabel );
				break;

			case ID_TIME_YEAR:
				snprintf( szLabel, sizeof(szLabel), "%i", pTimeNow->tm_year + 1900 );
				GUI_Ctrl_Label_SetText( &pMenu->pControls[i], szLabel );
				break;

			case ID_TIME_CURRENT:
				snprintf( szLabel, sizeof(szLabel), "%02i:%02i",
						  pTimeNow->tm_hour, pTimeNow->tm_min );

				GUI_Ctrl_Label_SetText( &pMenu->pControls[i], szLabel );
				break;
		}
	}
}

void GUI_Screenshot( void )
{
	char szPath[MAX_PATH + 1];
	char szName[MAX_PATH + 1];
	time_t timer;
	struct tm *now;
	
	if( !SC_GetValueForKey_Int( "scr_screenshot", NULL ) )
		return;

	SC_GetValueForKey_Str( "scr_path", szPath );

	ps2time_time( &timer );
	now = ps2time_localtime( &timer );

	if( szPath[ strlen(szPath) - 1 ] != '/' )
		strcat( szPath, "/" );

	snprintf( szName, sizeof(szName), "%02i-%02i-%02i_%02i-%02i-%02i.jpg",
			  now->tm_mon + 1, now->tm_mday, now->tm_year + 1900,
			  now->tm_hour, now->tm_min, now->tm_sec );

	strncat( szPath, szName, MAX_PATH );

	gsLib_screenshot( szPath );
	printf("GUI_Screenshot : Saved screenshot (%s)\n", szPath);
}
