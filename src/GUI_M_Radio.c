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
#include <Net.h>
#include <SysConf.h>
#include <Mp3.h>

#define ID_DOWNLOAD				100
#define ID_SORT_BY_NAME			101
#define ID_SORT_BY_GENRE		102
#define ID_SORT_BY_BITRATE		103
#define ID_SORT_BY_COUNT		104
#define ID_BOOKMARKS			105
#define ID_LIST					107
#define ID_STATUS				108

#define ID_STATION_NAME			109
#define ID_STATION_GENRE		110
#define ID_STATION_PLAYSTRING	111
#define ID_STATION_LISTENERS	112
#define ID_STATION_BITRATE		113

static shoutEntry_t		*pStations		= NULL;
static int				nNumStations	= 0;
static time_t			nNextRefresh	= 0;
static int				nNumBookmarks	= 0;
static shoutBookmark_t	*pBookmarks		= NULL;

unsigned int GUI_CB_Radio( GUIMenu_t *lpGUIMenu, unsigned int nGUIMsg, unsigned int nCtrlParam,
						   unsigned int nOther )
{
	char			szPath[MAX_PATH + 1];
	FHANDLE			fHandle;
	int				nSize, nRet;
	char			*pXMLFile;
	GUIControl_t	*pStatus, *pCtrl;

	switch( nGUIMsg )
	{
		case GUI_MSG_OPEN:
			LoadRadioBookmarks();

			if( GUI_Ctrl_List_Empty(GUI_ControlByID(ID_LIST)) )
			{
				if( SC_GetValueForKey_Int( "radio_cache_list", NULL ) )
				{
					snprintf( szPath, sizeof(szPath), "%sshoutcast/stations.xml",
							  GetElfPath() );
					
					fHandle = FileOpen( szPath, O_RDONLY );
					if( fHandle.fh < 0 )
						return 0;

					if( !(nSize = FileSeek( fHandle, 0, SEEK_END )) )
					{
						FileClose(fHandle);
						return 0;
					}

					FileSeek( fHandle, 0, SEEK_SET );

					pXMLFile = malloc( nSize );
					FileRead( fHandle, pXMLFile, nSize );
					FileClose( fHandle );
					
					pStatus = GUI_ControlByID(ID_STATUS);

					if( pStatus )
					{
						GUI_Ctrl_Label_SetText( pStatus, GUI_GetLangStr(LANG_STR_PARSING_LIST));
						GUI_Render();
					}

					if( !ParseStationXML( pXMLFile,nSize ) )
					{
						if( pStatus )
							GUI_Ctrl_Label_SetText( pStatus, GUI_GetLangStr(LANG_STR_ERROR_PARSE) );
					}
					else
					{
						if( pStatus )
							GUI_Ctrl_Label_SetText( pStatus, GUI_GetLangStr(LANG_STR_OK) );
					}

					free( pXMLFile );
					RefreshStationList();

					GUI_Render();
				}
			}
			break;

		case GUI_MSG_CLOSE:
			FreeRadioBookmarks();
			break;

		case GUI_MSG_CONTROL:
			switch( LOWORD(nCtrlParam) )
			{
				case ID_DOWNLOAD:
					// make sure net thread is shut down before downloading
					MP3_Stop();

					GetRadioStations();
					GUI_Render();
					break;

				case ID_SORT_BY_NAME:
					qsort( pStations, nNumStations, sizeof(shoutEntry_t), RadioSortName );
					RefreshStationList();
					GUI_Render();
					break;

				case ID_SORT_BY_GENRE:
					qsort( pStations, nNumStations, sizeof(shoutEntry_t), RadioSortGenre );
					RefreshStationList();
					GUI_Render();
					break;

				case ID_SORT_BY_BITRATE:
					qsort( pStations, nNumStations, sizeof(shoutEntry_t), RadioSortBR );
					RefreshStationList();
					GUI_Render();
					break;

				case ID_SORT_BY_COUNT:
					qsort( pStations, nNumStations, sizeof(shoutEntry_t), RadioSortLC );
					RefreshStationList();
					GUI_Render();
					break;
					
				case ID_BOOKMARKS:
					if( nOther == GUI_BUTTON_ON )
					{
						RefreshBookmarkList();
					}
					else
					{
						RefreshStationList();
					}
					GUI_Render();
					break;

				case ID_GO_BACK:
					GUI_OpenMenu( GUI_MENU_MYMUSIC );
					break;

				case ID_LIST:
					switch( HIWORD(nCtrlParam) )
					{
						case GUI_NOT_LIST_CLICK:
							if( GUI_Ctrl_ToggleButton_GetState(GUI_ControlByID(ID_BOOKMARKS))
								== GUI_BUTTON_OFF )
							{

								if( nOther < 0 || nOther >= nNumStations )
									return 1;

								// send/recv is not thread safe so be sure any running
								// net thread is shut down before downloading
								MP3_Stop();

								nRet = GetStationURL( &pStations[ nOther ], szPath,
												  sizeof(szPath) );
								if( !nRet )
									return 1;
							}
							else
							{
								if( nOther < 0 || nOther >= nNumBookmarks )
									return 1;

								strncpy( szPath, pBookmarks[nOther].url, MAX_PATH );
								szPath[MAX_PATH] = 0;
							}

							// try to connect
							pStatus = GUI_ControlByID(ID_STATUS);

							if( pStatus )
							{
								GUI_Ctrl_Label_SetText( pStatus,
														GUI_GetLangStr(LANG_STR_CONNECTING) );
								GUI_Render();
							}

							nRet = MP3_OpenStream( szPath );
							if( nRet != STREAM_ERROR_OK )
							{
								if( pStatus )
									GUI_Ctrl_Label_SetText( pStatus,
															GUI_GetLangStr(nRet) );
							}
							else
							{
								if( pStatus )
									GUI_Ctrl_Label_SetText( pStatus,
															GUI_GetLangStr(LANG_STR_PLAYBACK) );
								MP3_PlayStream();
							}

							GUI_Render();
							break;

						// update label controls with station info
						case GUI_NOT_LIST_POS:
							if( GUI_Ctrl_ToggleButton_GetState(GUI_ControlByID(ID_BOOKMARKS)) ==
								GUI_BUTTON_ON )
								break;

							if( (pCtrl = GUI_ControlByID(ID_STATION_NAME)) )
								GUI_Ctrl_Label_SetText( pCtrl, pStations[nOther].name );

							if( (pCtrl = GUI_ControlByID(ID_STATION_GENRE)) )
								GUI_Ctrl_Label_SetText( pCtrl, pStations[nOther].genre );

							if( (pCtrl = GUI_ControlByID(ID_STATION_PLAYSTRING)) )
								GUI_Ctrl_Label_SetText( pCtrl, pStations[nOther].playing );

							if( (pCtrl = GUI_ControlByID(ID_STATION_LISTENERS)) )
							{
								snprintf( szPath, sizeof(szPath), "%i",
										  pStations[nOther].listeners );

								GUI_Ctrl_Label_SetText( pCtrl, szPath );
							}

							if( (pCtrl = GUI_ControlByID(ID_STATION_BITRATE)) )
							{
								snprintf( szPath, sizeof(szPath), "%i",
										  pStations[nOther].bitrate );

								GUI_Ctrl_Label_SetText( pCtrl, szPath );
							}

							GUI_Render();
							break;
					}
					break;
			}
			break;
	}
	return 0;
}

void RefreshStationList( void )
{
	unsigned int i;
	GUIControl_t *pCtrl;

	pCtrl = GUI_ControlByID(ID_LIST);

	GUI_Ctrl_List_Clean(pCtrl);

	for( i = 0; i < nNumStations; i++ )
		GUI_Ctrl_List_AddItem( pCtrl, pStations[i].name, 0 );

	if( (pCtrl = GUI_ControlByID(ID_BOOKMARKS)) )
		GUI_Ctrl_ToggleButton_SetState( pCtrl, GUI_BUTTON_OFF );
}

// parses stations.xml file and populates
// global pStations array.
int ParseStationXML( const char *pXMLFile, int nSize )
{
	scew_parser		*pParser;
	scew_tree		*pTree;
	scew_element	*pElem, **pList, *pChild;
	scew_attribute	*pAttr;
	const char		*pName;
	unsigned int	nNum, i;

	// clean up existing entries first
	if( pStations )
	{
		for( i = 0; i < nNumStations; i++ )
		{
			if( pStations[i].name )
				free( pStations[i].name );

			if( pStations[i].genre )
				free( pStations[i].genre );

			if( pStations[i].url )
				free( pStations[i].url );

			if( pStations[i].playing )
				free( pStations[i].playing );
		}
	}

	pStations		= NULL;
	nNumStations	= 0;

	if( (pParser = scew_parser_create()) == NULL )
		return 0;

	if( !scew_parser_load_buffer( pParser, pXMLFile, nSize ) ) {
		scew_error code = scew_error_code();

		printf(	"ParseStationXML: Unable to load file (error %d: %s)\n", 
				code, scew_error_string(code) );

		return 0;
	}

	if( (pTree = scew_parser_tree(pParser)) == NULL ) {
		scew_parser_free( pParser );
		return 0;
	}

	if( (pElem = scew_tree_root(pTree)) == NULL ) {
		scew_tree_free(pTree);
		scew_parser_free( pParser );
		return 0;
	}

	// root should be <WinampXML>
	if( (pName = scew_element_name(pElem)) == NULL ) {
		scew_tree_free(pTree);
		scew_parser_free(pParser);
		return 0;
	}

	if( strcmp( pName, "WinampXML" ) ) {
		scew_tree_free(pTree);
		scew_parser_free(pParser);
		return 0;
	}

	// next element must be <playlist>
	if( (pElem = scew_element_by_name( pElem, "playlist" )) == NULL ) {
		scew_tree_free(pTree);
		scew_parser_free(pParser);
		return 0;
	}

	if( (pName = scew_element_name(pElem)) == NULL ) {
		scew_tree_free(pTree);
		scew_parser_free(pParser);
		return 0;
	}

	if( strcmp( pName, "playlist" ) ) {
		scew_tree_free(pTree);
		scew_parser_free(pParser);
		return 0;
	}

	// get a list of all entries
	if( (pList = scew_element_list( pElem, "entry", &nNum )) == NULL ) {
		scew_tree_free(pTree);
		scew_parser_free(pParser);
		return 0;
	}

	if( nNum > SHOUTCAST_STATIONS )
		nNum = SHOUTCAST_STATIONS;

	// there should be 500 entries in the XML file max.
	pStations = (shoutEntry_t*) malloc( sizeof(shoutEntry_t) * nNum );
	if( pStations == NULL ) {
		scew_tree_free(pTree);
		scew_parser_free(pParser);
		return 0;
	}

	// parse the entries
	for( i = 0; i < nNum; i++ )
	{
		pElem = *(pList + i);

		// fetch the Playstring attribute
		if( (pAttr = scew_attribute_by_name( pElem, "Playstring" )) == NULL )
			continue;

		if( (pName = scew_attribute_value(pAttr)) == NULL )
			continue;

		if( (pStations[ nNumStations ].url = (char*) malloc( strlen(pName) + 1 )) == NULL )
			break;

		strcpy( pStations[ nNumStations ].url, pName );

		// fetch the Station name element
		if( (pChild = scew_element_by_name( pElem, "Name" )) == NULL )
			continue;

		if( (pName = scew_element_contents( pChild )) == NULL )
			continue;

		if( (pStations[ nNumStations ].name = (char*) malloc( strlen(pName) + 1 )) == NULL )
			break;

		strcpy( pStations[ nNumStations ].name, pName );

		// fetch the Station Genre element
		if( (pChild = scew_element_by_name( pElem, "Genre" )) == NULL )
			continue;

		if( (pName = scew_element_contents( pChild )) == NULL )
			continue;

		if( (pStations[ nNumStations ].genre = (char*) malloc( strlen(pName) + 1 )) == NULL )
			break;

		strcpy( pStations[ nNumStations ].genre, pName );

		// fetch the Nowplaying element
		if( (pChild = scew_element_by_name( pElem, "Nowplaying" )) == NULL )
			continue;

		if( (pName = scew_element_contents( pChild )) == NULL )
			continue;

		if( (pStations[ nNumStations ].playing = (char*) malloc( strlen(pName) + 1 )) == NULL )
			break;

		strcpy( pStations[ nNumStations ].playing, pName );

		// fetch the Listeners element
		if( (pChild = scew_element_by_name( pElem, "Listeners" )) == NULL )
			continue;

		if( (pName = scew_element_contents( pChild )) == NULL )
			continue;

		pStations[ nNumStations ].listeners = atoi(pName);

		// fetch the Bitrate element
		if( (pChild = scew_element_by_name( pElem, "Bitrate" )) == NULL )
			continue;

		if( (pName = scew_element_contents( pChild )) == NULL )
			continue;

		pStations[ nNumStations ].bitrate = atoi(pName);

		// done parsing this entry
		nNumStations++;
	}

	scew_element_list_free(pList);
	scew_tree_free(pTree);
	scew_parser_free(pParser);

	return 1;
}

static int DLCallback( int nRecvBytes, int nTotalBytes )
{
	GUIControl_t *pStatus;
	char szStr[256];

	pStatus = GUI_ControlByID(ID_STATUS);

	if( pStatus )
	{
		snprintf( szStr, sizeof(szStr), "%s (%i KB)",
				  GUI_GetLangStr(LANG_STR_DOWNLOADING_LIST),
				  nRecvBytes / 1024 );

		GUI_Ctrl_Label_SetText( pStatus, szStr );
		GUI_Render();
	}

	return 1;
}

int GetRadioStations( void )
{
	GUIControl_t	*pStatus;
	char			*pBuffer;
	int				nBufSize, nRet;
	char			szPath[MAX_PATH + 1];
	FHANDLE			fHandle;

	pStatus = GUI_ControlByID(ID_STATUS);

	if( nNextRefresh > ps2time_time(NULL) )
	{
		GUI_DlgMsgBox( GUI_GetLangStr(LANG_STR_EVERY_5_MINUTES),
					   GUI_GetLangStr(LANG_STR_ERROR),
					   0 );
		return 0;
	}

	if( pStatus )
	{
		GUI_Ctrl_Label_SetText( pStatus, GUI_GetLangStr(LANG_STR_CONNECTING) );
		GUI_Render();
	}

	nRet = HttpDownload( SHOUTCAST_HOST, SHOUTCAST_PORT, SHOUTCAST_FILE,
						 &pBuffer, &nBufSize, DLCallback );

	if( nRet != 1 )
	{
		if( pStatus )
		{
			GUI_Ctrl_Label_SetText( pStatus, GUI_GetLangStr(nRet) );
			return 0;
		}
	}

	// cache file
	if( SC_GetValueForKey_Int( "radio_cache_list", NULL ) )
	{
		snprintf( szPath, sizeof(szPath), "%sshoutcast/stations.xml", GetElfPath() );

		fHandle = FileOpen( szPath, O_RDONLY );

		if( fHandle.fh >= 0 )
		{
			FileClose(fHandle);
			FileRemove( szPath );
		}

		fHandle = FileOpen( szPath, O_WRONLY | O_CREAT | O_TRUNC );

		if( fHandle.fh >= 0 )
		{
			FileWrite( fHandle, pBuffer, nBufSize );
			FileClose( fHandle );
		}
	}

	if( pStatus )
	{
		GUI_Ctrl_Label_SetText( pStatus, GUI_GetLangStr(LANG_STR_PARSING_LIST) );
		GUI_Render();
	}

	if( ParseStationXML( pBuffer, nBufSize ) == 0 )
	{
		if( pStatus )
			GUI_Ctrl_Label_SetText( pStatus, GUI_GetLangStr(LANG_STR_ERROR_PARSE) );
	}
	else
	{
		if( pStatus )
			GUI_Ctrl_Label_SetText( pStatus, GUI_GetLangStr(LANG_STR_DOWNLOAD_DONE) );
	}

	// free memory allocated by HttpDownload
	free( pBuffer );

	RefreshStationList();

	nNextRefresh = ps2time_time(NULL) + 60 * 5;
	return 1;
}

//
// GetStationURL -	Retrieves the URL of a radio stream server.
//
//					Entries in the XML file don't link to the
//					stations directly but have a download link
//					to a playlist which contains the actual
//					url of the stream server.
//

int GetStationURL( const shoutEntry_t *pEntry, char *pStationURL, int nSize )
{
	int		nRet;
	char	strHost[256], strFile[256];
	int		nPort;
	char	*pBuffer, *pStr, *pEnd;
	int		nBufSize;
	char	strLine[1024];

	if( !pEntry->url || !pStationURL )
		return 0;

	// extract host, port and filename from url
	nRet = TokenizeURL( pEntry->url, strHost, strFile, &nPort );

	if( !nRet )
		return 0;

	// download the playlist
	nRet = HttpDownload( strHost, nPort, strFile, &pBuffer, &nBufSize, NULL );

	// some error occured
	if( nRet != 1 )
		return 0;

	// currently we only care for the first File1 entry and discard
	// all other playlist entries
	if( (pStr = strstr( pBuffer, "File1=" )) == NULL )
	{
		free(pBuffer);
		return 0;
	}

	ReadBufLine( &pStr, strLine );

	// remove line end from url
	pEnd = strLine + strlen(strLine) - 1;

	while( *pEnd == '\n' || *pEnd == '\r' || *pEnd == ' ' ) {
		*pEnd = 0;
		pEnd--;
	}

	// parse URL
	if( (pStr = strchr( strLine, '=' )) == NULL )
	{
		free(pBuffer);
		return 0;
	}

	// skip '='
	pStr++;

	strncpy( pStationURL, pStr, nSize - 1 );
	pStationURL[ nSize ] = 0;

	free( pBuffer );
	return 1;
}

int RadioSortName( const void *a, const void *b )
{
	shoutEntry_t *p1, *p2;

	p1 = (shoutEntry_t*)a;
	p2 = (shoutEntry_t*)b;

	if( !p1->name )
		return -1;

	if( !p2->name )
		return 1;

	return strcmp( p1->name, p2->name );
}

int RadioSortGenre( const void *a, const void *b )
{
	shoutEntry_t *p1, *p2;

	p1 = (shoutEntry_t*)a;
	p2 = (shoutEntry_t*)b;

	if( !p1->genre )
		return -1;

	if( !p2->genre )
		return 1;

	return strcmp( p1->genre, p2->genre );
}

int RadioSortBR( const void *a, const void *b )
{
	shoutEntry_t *p1, *p2;

	p1 = (shoutEntry_t*)a;
	p2 = (shoutEntry_t*)b;

	return (p1->bitrate - p2->bitrate);
}

int RadioSortLC( const void *a, const void *b )
{
	shoutEntry_t *p1, *p2;

	p1 = (shoutEntry_t*)a;
	p2 = (shoutEntry_t*)b;

	return (p1->listeners - p2->listeners);
}

void LoadRadioBookmarks( void )
{
	char	szPath[MAX_PATH + 1];
	char	szLine[MAX_PATH + 1], szNext[MAX_PATH + 1];
	FHANDLE	fHandle;
	int		nSize, nRead;
	char	*pFile, *pBufPtr;
	char	*pEnd;

	snprintf( szPath, sizeof(szPath), "%sshoutcast/Winamp.bm",
			  GetElfPath() );

	fHandle = FileOpen( szPath, O_RDONLY );
	if( fHandle.fh < 0 )
		return;

	nSize = FileSeek( fHandle, 0, SEEK_END );
	if( !nSize ) {
		FileClose(fHandle);
		return;
	}

	FileSeek( fHandle, 0, SEEK_SET );

	if( !(pFile = malloc( nSize + 1 )) )
	{
		FileClose(fHandle);
		return;
	}

	nRead = FileRead( fHandle, pFile, nSize );
	pFile[ nRead ] = 0;

	FileClose( fHandle );

	pBufPtr = pFile;

	if( pBookmarks )
	{
		free( pBookmarks );

		pBookmarks		= NULL;
		nNumBookmarks	= 0;
	}

	while( ReadBufLine( &pBufPtr, szLine ) )
	{
		pBookmarks = realloc( pBookmarks, (nNumBookmarks + 1) *
							  sizeof(shoutBookmark_t) );
		if( !pBookmarks )
		{
			nNumBookmarks = 0;
			break;
		}

		// first line is the station address, second line station name
		ReadBufLine( &pBufPtr, szNext );

		// remove line end from url
		pEnd = szLine + strlen(szLine) - 1;

		while( *pEnd == '\n' || *pEnd == '\r' || *pEnd == ' ' )
		{
			*pEnd = 0;
			pEnd--;
		}

		pBookmarks[ nNumBookmarks ].url = malloc( strlen(szLine) + 1 );
		
		if( !pBookmarks[ nNumBookmarks ].url )
			break;

		strcpy( pBookmarks[ nNumBookmarks ].url, szLine );

		// remove line end from name
		pEnd = szNext + strlen(szNext) - 1;

		while( *pEnd == '\n' || *pEnd == '\r' || *pEnd == ' ' )
		{
			*pEnd = 0;
			pEnd--;
		}

		pBookmarks[ nNumBookmarks ].name = malloc( strlen(szNext) + 1 );

		if( !pBookmarks[ nNumBookmarks ].name )
			break;

		strcpy( pBookmarks[ nNumBookmarks ].name, szNext );
		nNumBookmarks++;
	}

	free(pFile);
}

void FreeRadioBookmarks( void )
{
	if( pBookmarks )
		free(pBookmarks);

	pBookmarks		= NULL;
	nNumBookmarks	= 0;
}

void RefreshBookmarkList( void )
{
	unsigned int i;
	GUIControl_t *pCtrl;

	pCtrl = GUI_ControlByID(ID_LIST);

	GUI_Ctrl_List_Clean(pCtrl);

	for( i = 0; i < nNumBookmarks; i++ )
		GUI_Ctrl_List_AddItem( pCtrl, pBookmarks[i].name, 0 );

	if( (pCtrl = GUI_ControlByID(ID_BOOKMARKS)) )
		GUI_Ctrl_ToggleButton_SetState( pCtrl, GUI_BUTTON_ON );
}
