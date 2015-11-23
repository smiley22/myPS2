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

#define ID_SORT_BY_NAME		100
#define ID_SORT_BY_SIZE		101
#define ID_SORT_BY_TYPE		102
#define ID_NEW_FOLDER		103
#define ID_SELECT_ALL		104
#define ID_DELETE			105
#define ID_RENAME			106
#define ID_COPY				107
#define ID_START			108
//#define ID_GO_BACK			109
#define ID_FILE_LIST		110

#define DLG_CONFIRM_DELETE	0
#define DLG_NEW_FOLDER		1
#define DLG_RENAME			2
#define DLG_COPY			3

static char				szSrc[MAX_PATH + 1];
static char				szDst[MAX_PATH + 1];
static char				szOldName[MAX_PATH + 1];
static fileInfo_t *lpFiles				= NULL;
static unsigned int		nNumFiles				= 0;

static void SetButtonStates( void )
{
	const char		*lpStr;
	unsigned int	nSel;

	GUI_CtrlSetEnable( ID_NEW_FOLDER,	1 );
	GUI_CtrlSetEnable( ID_SELECT_ALL,	1 );
	GUI_CtrlSetEnable( ID_DELETE,		1 );
	GUI_CtrlSetEnable( ID_RENAME,		1 );
	GUI_CtrlSetEnable( ID_COPY,			1 );
	GUI_CtrlSetEnable( ID_START,		1 );

	if( GUI_Ctrl_Dirview_IsRoot(GUI_ControlByID(ID_FILE_LIST)) )
	{
		GUI_CtrlSetEnable( ID_NEW_FOLDER,	0 );
		GUI_CtrlSetEnable( ID_SELECT_ALL,	0 );
		GUI_CtrlSetEnable( ID_DELETE,		0 );
		GUI_CtrlSetEnable( ID_RENAME,		0 );
		GUI_CtrlSetEnable( ID_COPY,			0 );
		GUI_CtrlSetEnable( ID_START,		0 );

		return;
	}

	lpStr	= GUI_Ctrl_Dirview_GetDir( GUI_ControlByID(ID_FILE_LIST) );
	nSel	= GUI_Ctrl_Dirview_NumMark( GUI_ControlByID(ID_FILE_LIST) );

	if( !strncmp( lpStr, "cdfs:", 5 ) )
	{
		GUI_CtrlSetEnable( ID_NEW_FOLDER,	0 );
		GUI_CtrlSetEnable( ID_DELETE,		0 );
		GUI_CtrlSetEnable( ID_RENAME,		0 );
		GUI_CtrlSetEnable( ID_START,		0 );
	}

	if( nSel > 1 )
		GUI_CtrlSetEnable( ID_RENAME,		0 );

	if( !nSel )
	{
		GUI_CtrlSetEnable( ID_DELETE,		0 );
		GUI_CtrlSetEnable( ID_RENAME,		0 );
		GUI_CtrlSetEnable( ID_COPY,			0 );
	}

	if( !lpFiles || !nNumFiles )
		GUI_CtrlSetEnable( ID_START,		0 );

}

static void SelectAllFiles( void )
{
	const GUIControl_t *pCtrl;
	unsigned int nCount, i, bMark;
	const fileInfo_t *lpFile;

	pCtrl	= GUI_ControlByID(ID_FILE_LIST);
	nCount	= GUI_Ctrl_Dirview_GetCount( pCtrl );

	for( i = 0; i < nCount; i++ )
	{
		lpFile = GUI_Ctrl_Dirview_GetItem( pCtrl, i );

		if( !strcmp( lpFile->name, "." ) || !strcmp( lpFile->name, ".." ) )
			continue;

		bMark = !(lpFile->flags & FLAG_MARKED);

		GUI_Ctrl_Dirview_SetMarked( pCtrl, i, bMark );
	}
}

static void DeleteFiles( void )
{
	unsigned int nNum, i;
	const char *pDir;
	char szPath[MAX_PATH + 1];
	const fileInfo_t *pFile;
	GUIControl_t *pCtrl;

	pCtrl	= GUI_ControlByID(ID_FILE_LIST);
	nNum	= GUI_Ctrl_Dirview_NumMark(pCtrl);
	pDir	= GUI_Ctrl_Dirview_GetDir(pCtrl);	

	for( i = 0; i < nNum; i++ )
	{
		pFile = GUI_Ctrl_Dirview_GetMarked( pCtrl, i );

		snprintf( szPath, sizeof(szPath), "%s%s", pDir, pFile->name );

		if( pFile->flags & FLAG_DIRECTORY )
		{
			// hdd fix
			strncat( szPath, "/", sizeof(szPath) );
			DirRemove(szPath);
		}
		else
		{
			FileRemove(szPath);
		}
	}

	GUI_Ctrl_Dirview_Refresh(pCtrl);
}

static void MakeFileList( void )
{
	GUIControl_t *pCtrl;
	unsigned int i;
	const fileInfo_t *pFile;

	if( lpFiles )
		free(lpFiles);

	pCtrl		= GUI_ControlByID(ID_FILE_LIST);
	nNumFiles	= GUI_Ctrl_Dirview_NumMark(pCtrl);

	lpFiles		= malloc( nNumFiles * sizeof(fileInfo_t) );

	for( i = 0; i < nNumFiles; i++ )
	{
		pFile = GUI_Ctrl_Dirview_GetMarked( pCtrl, i );

		lpFiles[i].flags	= pFile->flags;
		lpFiles[i].size		= pFile->size;

		strcpy( lpFiles[i].name, pFile->name );
	}

	strncpy( szSrc, GUI_Ctrl_Dirview_GetDir(pCtrl), MAX_PATH );
	szSrc[MAX_PATH] = 0;
}

static void MakeDir( const char *lpDirName )
{
	const char *pDir;
	char szDirName[MAX_PATH + 1];
	GUIControl_t *pCtrl;

	pCtrl	= GUI_ControlByID(ID_FILE_LIST);
	pDir	= GUI_Ctrl_Dirview_GetDir(pCtrl);

	snprintf( szDirName, sizeof(szDirName), "%s%s", pDir, lpDirName );

	FileMkdir( szDirName );

	GUI_Ctrl_Dirview_Refresh(pCtrl);
}

static void Rename( const char *lpNewName )
{
	const char		*pDir;
	GUIControl_t	*pCtrl;
	char			szPathOld[MAX_PATH];
	char			szPathNew[MAX_PATH];

	pCtrl	= GUI_ControlByID(ID_FILE_LIST);
	pDir	= GUI_Ctrl_Dirview_GetDir(pCtrl);

	snprintf( szPathOld, sizeof(szPathOld), "%s%s", pDir, szOldName );
	snprintf( szPathNew, sizeof(szPathNew), "%s%s", pDir, lpNewName );

	FileRename( szPathOld, szPathNew );

	GUI_Ctrl_Dirview_Refresh(pCtrl);
}

unsigned int GUI_CB_MyFiles( GUIMenu_t *lpGUIMenu, unsigned int nGUIMsg,
							 unsigned int nCtrlParam, unsigned int nOther )
{
	switch( nGUIMsg )
	{
		case GUI_MSG_OPEN:
			SetButtonStates();
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

				case ID_NEW_FOLDER:
					GUI_DlgKeyboard( "", GUI_GetLangStr( LANG_STR_FOLDER_NAME ),
									 DLG_NEW_FOLDER );
					break;

				case ID_SELECT_ALL:
					SelectAllFiles();
					SetButtonStates();
					GUI_Render();
					break;

				case ID_DELETE:
					GUI_DlgYesNo( GUI_GetLangStr( LANG_STR_CONFIRM_DELETE_1 ),
								  GUI_GetLangStr( LANG_STR_CONFIRM_DELETE_2 ),
								  DLG_CONFIRM_DELETE );
					break;

				case ID_RENAME:
					// save old filename
					strncpy( szOldName, GUI_Ctrl_Dirview_GetMarked(
							 GUI_ControlByID(ID_FILE_LIST), 0)->name, MAX_PATH );
					szOldName[MAX_PATH] = 0;

					GUI_DlgKeyboard( szOldName, GUI_GetLangStr( LANG_STR_RENAME ),
									 DLG_RENAME );
					break;

				case ID_COPY:
					MakeFileList();
					GUI_Ctrl_Dirview_Init( GUI_ControlByID(ID_FILE_LIST) );
					SetButtonStates();
					lpGUIMenu->iSelID = ID_FILE_LIST;

					GUI_DlgMsgBox(	GUI_GetLangStr( LANG_STR_SELECT_DEST_DIR ),
									GUI_GetLangStr( LANG_STR_FILE_COPY ), 0 );
					break;

				case ID_START:
					strncpy( szDst, GUI_Ctrl_Dirview_GetDir(
						GUI_ControlByID(ID_FILE_LIST)), MAX_PATH );
					szDst[MAX_PATH] = 0;
					GUI_OpenDialog( GUI_MENU_COPYDLG, lpGUIMenu, DLG_COPY );
					break;

				case ID_GO_BACK:
					GUI_OpenMenu( GUI_MENU_MAIN );
					break;

				case ID_FILE_LIST:
					switch( HIWORD(nCtrlParam) )
					{
						case GUI_NOT_DIRVIEW_DIR:
						case GUI_NOT_DIRVIEW_MARK:
						case GUI_NOT_DIRVIEW_UNMARK:
							SetButtonStates();
							GUI_Render();
							break;
					}
					break;
			}
			break;

		case GUI_MSG_DIALOG:
			switch( LOWORD(nCtrlParam) )
			{
				case DLG_CONFIRM_DELETE:
					if( HIWORD(nCtrlParam) == DLG_RET_YES )
					{
						DeleteFiles();

						lpGUIMenu->iSelID = ID_FILE_LIST;
						SetButtonStates();
						GUI_Render();
					}
					break;

				case DLG_NEW_FOLDER:
					// nOther is a pointer to the label string
					if( HIWORD(nCtrlParam) == DLG_RET_SUBMIT )
					{
						MakeDir( (const char*) nOther );

						lpGUIMenu->iSelID = ID_FILE_LIST;
						SetButtonStates();
						GUI_Render();
					}
					break;

				case DLG_RENAME:
					if( HIWORD(nCtrlParam) == DLG_RET_SUBMIT )
					{
						Rename( (const char*) nOther );

						lpGUIMenu->iSelID = ID_FILE_LIST;
						SetButtonStates();
						GUI_Render();
					}
					break;

				case DLG_COPY:
					GUI_Ctrl_Dirview_Refresh( GUI_ControlByID(ID_FILE_LIST) );
					lpGUIMenu->iSelID = ID_FILE_LIST;
					SetButtonStates();
					GUI_Render();
					break;
			}
			break;
	}
	return 0;
}

/////////////////////////////////////////////////
// GUI_M_DlgCopy

#define ID_LABEL_FILE		100
#define ID_PROGRESS			101
#define ID_BUTTON_OK		102
#define ID_LABEL_PERCENT	103

int CopyCallback( const char *pFileSrc, const char *pFileDst, unsigned int
					nCopied, unsigned int nTotal )
{
	double f;
	char szStr[32];
	GUIControl_t *pCtrl = GUI_ControlByID(ID_PROGRESS);

	GUI_Ctrl_Label_SetText( GUI_ControlByID(ID_LABEL_FILE), pFileSrc );

	GUI_Ctrl_Progress_SetBounds( pCtrl, 0, nTotal );
	GUI_Ctrl_Progress_SetPos( pCtrl, nCopied );

	f = ((double) nCopied / nTotal) * 100.0f;
	snprintf( szStr, sizeof(szStr), "%i %%", (int)f );

	GUI_Ctrl_Label_SetText( GUI_ControlByID(ID_LABEL_PERCENT), szStr );
	GUI_Render();
	return 1;
}

static void CopyFiles( void )
{
	char			szPath[MAX_PATH + 1];
	char			szOut[MAX_PATH + 1];
	unsigned int	i;

	if( !lpFiles || !nNumFiles )
		return;

	for( i = 0; i < nNumFiles; i++ )
	{
		if( lpFiles[i].flags & FLAG_DIRECTORY )
		{
			snprintf( szPath, sizeof(szPath), "%s%s/",
					  szSrc, lpFiles[i].name );

			snprintf( szOut, sizeof(szOut), "%s%s/",
					  szDst, lpFiles[i].name );

			DirCopy( szPath, szOut, CopyCallback );
		}
		else
		{
			snprintf( szPath, sizeof(szPath), "%s%s",
					  szSrc, lpFiles[i].name );

			snprintf( szOut, sizeof(szOut), "%s%s",
					  szDst, lpFiles[i].name );

			FileCopy( szPath, szOut, CopyCallback );
		}
	}

	// ugly
	free( lpFiles );
	lpFiles		= NULL;
	nNumFiles	= 0;
}

unsigned int GUI_CB_DlgCopy( GUIMenu_t *lpGUIMenu, unsigned int nGUIMsg,
							 unsigned int nCtrlParam, unsigned int nOther )
{
	switch( nGUIMsg )
	{
		case GUI_MSG_OPEN:
			CopyFiles();

			GUI_Ctrl_Label_SetText( GUI_ControlByID(ID_LABEL_FILE), "Done" );
			break;

		case GUI_MSG_CONTROL:
			switch( LOWORD(nCtrlParam) )
			{
				case ID_BUTTON_OK:
					GUI_CloseDialog( lpGUIMenu, 0, 0 );
					break;
			}
			break;
	}
	return 0;
}
