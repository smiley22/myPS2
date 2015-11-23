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

#include <samba.h>

static smbShare_t		*smbShares		= NULL;
static unsigned int		nNumSmbShares	= 0;

static char				szLogFile[MAX_PATH];

static void SMB_Auth( const char *pServer, const char *pShare, char *pWorkgroup,
					  int workgroup_len, char *pUsername, int username_len,
					  char *pPassword, int password_len )
{
}

static void SMB_Log( const char *pMessage )
{
	static FHANDLE fdLog = { -1, -1 };
	static int nFlush;

	if( fdLog.fh < 0 ) {
		fdLog = FileOpen( szLogFile, O_RDWR | O_APPEND | O_CREAT );
		if( fdLog.fh < 0 )
			return;
	}

	if( nFlush > 10 ) {
		FileClose( fdLog );
		fdLog = FileOpen( szLogFile, O_RDWR | O_APPEND | O_CREAT );
		nFlush = 0;
	}

	FileWrite( fdLog, (void*)pMessage, strlen(pMessage) );
	nFlush++;
}

int SMB_Init( void )
{
	const char	*pWINSServer;
	const char	*pSetting;
	int			nRet;

	Bootscreen_printf("Initializing SAMBA library...\n");

	// set up smb workgroup and ethernet interface
	pSetting = SC_GetValueForKey_Str( "net_smb_workgroup", NULL );

	Bootscreen_printf("\tSetting Workgroup to %s\n", pSetting);
	ps2_smb_setWorkgroup( (char*) pSetting );
	ps2_smb_setInterface( (char*) SC_GetValueForKey_Str( "net_ip", NULL ),
						  (char*) SC_GetValueForKey_Str( "net_netmask", NULL ) );
						

	// set up logging callback
	if( SC_GetValueForKey_Int( "net_smb_debug", NULL ) )
	{
		snprintf( szLogFile, sizeof(szLogFile), "%slogs/samba.log", GetElfPath() );
		ps2_smb_setLogCallback( SMB_Log );
	}

	// init library
	if( smbc_init(SMB_Auth, SC_GetValueForKey_Int( "net_smb_debug", NULL )) != 0 ) {
		Bootscreen_printf("Error: Could not initialize smbc library\n");
		return 0;
	}

	pWINSServer = SC_GetValueForKey_Str( "net_smb_wins", NULL );

	if( pWINSServer && pWINSServer[0] ) {
		Bootscreen_printf("\tSetting WINS Server to %s\n", pWINSServer );

		lp_do_parameter(-1, "wins server", pWINSServer );
		lp_do_parameter(-1, "name resolve order", "lmhosts wins host bcast");
	}
	else {
		lp_do_parameter(-1, "name resolve order", "lmhosts hosts bcast");
	}

	// parse share bookmarks from samba.xml
	nRet = SMB_ParseShares();
	Bootscreen_printf("Parsed %i shares from samba.xml\n", nRet);

	return 1;
}

int SMB_GetNumShares( void )
{
	return nNumSmbShares;
}

const smbShare_t *SMB_GetShare( unsigned int i )
{
	if( i >= nNumSmbShares )
		return NULL;

	return &smbShares[i];
}

const char *SMB_GetShareNameByPath( const char *pPath )
{
	unsigned int i;

	for( i = 0; i < nNumSmbShares; i++ )
	{
		if( !strcmp( smbShares[i].pSharePath, pPath ) )
			return smbShares[i].pShareName;
	}

	return NULL;
}

int SMB_ParseShares( void )
{
	FHANDLE			fHandle;
	char			szFileName[ MAX_PATH + 1 ];
	unsigned int	nSize, nNum, nRead, i;
	unsigned char	*pBuf;
	scew_parser		*pParser;
	scew_tree		*pTree;
	scew_element	*pElem, **pList, *pChild;
	const char		*pName;

	snprintf( szFileName, sizeof(szFileName), "%ssamba.xml", GetElfPath() );

	fHandle = FileOpen( szFileName, O_RDONLY );

	if( fHandle.fh < 0 )
		return 0;

	nSize = FileSeek( fHandle, 0, SEEK_END );
	FileSeek( fHandle, 0, SEEK_SET );

	if( !nSize ) {
		FileClose(fHandle);
		return 0;
	}

	if( !(pBuf = malloc( nSize )) ) {
		FileClose(fHandle);
		return 0;
	}

	FileRead( fHandle, pBuf, nSize );
	FileClose(fHandle);

	if( (pParser = scew_parser_create()) == NULL ) {
		free(pBuf);
		return 0;
	}

	if( !scew_parser_load_buffer( pParser, pBuf, nSize ) ) {
		free(pBuf);
		return 0;
	}

	if( (pTree = scew_parser_tree(pParser)) == NULL ) {
		free(pBuf);
		scew_parser_free(pParser);
		return 0;
	}

	if( (pElem = scew_tree_root(pTree)) == NULL ) {
		free(pBuf);
		scew_tree_free(pTree);
		scew_parser_free( pParser );
		return 0;
	}

	if( (pName = scew_element_name(pElem)) == NULL ) {
		free(pBuf);
		scew_tree_free(pTree);
		scew_parser_free(pParser);
		return 0;
	}

	if( strcmp( pName, "shares" ) ) {
		free(pBuf);
		scew_tree_free(pTree);
		scew_parser_free(pParser);
		return 0;
	}

	if( (pList = scew_element_list( pElem, "share", &nNum )) == NULL ) {
		free(pBuf);
		scew_tree_free(pTree);
		scew_parser_free(pParser);
		return 0;
	}

	smbShares		= malloc( sizeof(smbShare_t) * nNum );
	nNumSmbShares	= 0;
	nRead			= 0;

	if( !smbShares ) {
		free( pBuf );
		scew_tree_free(pTree);
		scew_parser_free(pParser);
		return 0;
	}

	for( i = 0; i < nNum; i++ )
	{
		pElem = *(pList + i);

		if( (pChild = scew_element_by_name( pElem, "name" )) == NULL )
			continue;

		if( (pName = scew_element_contents( pChild )) == NULL )
			continue;

		smbShares[ nRead ].pShareName = malloc( strlen(pName) + 1 );
		strcpy( smbShares[ nRead ].pShareName, pName );

		if( (pChild = scew_element_by_name( pElem, "path" )) == NULL )
			continue;

		if( (pName = scew_element_contents( pChild )) == NULL )
			continue;

		smbShares[ nRead ].pSharePath = malloc( strlen(pName) + 1 );
		strcpy( smbShares[ nRead ].pSharePath, pName );

		nRead++;
	}

	free(pBuf);
	scew_element_list_free(pList);
	scew_tree_free(pTree);
	scew_parser_free(pParser);

	nNumSmbShares = nRead;

	return nRead;
}
