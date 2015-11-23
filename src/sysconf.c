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

#include <tamtypes.h>
#include <file.h>
#include <sysconf.h>
#include <charset.h>

const pair_t SC_DEFAULTS[] =
{
	{	"scr_adjust_x",			"0",								NULL,	NULL	},
	{	"scr_adjust_y",			"0",								NULL,	NULL	},
	{	"scr_mode",				"255",								NULL,	NULL	},
	{	"scr_screenshot",		"1",								NULL,	NULL	},
	{	"scr_path",				"host:/",							NULL,	NULL	},

	{	"tbn_caching",			"1",								NULL,	NULL	},
	{	"tbn_cache_path",		"pfs0:/THUMBCACHE/",				NULL,	NULL	},

	{	"net_enable",			"1",								NULL,	NULL	},
	{	"net_ip",				DEFAULT_NET_IP,						NULL,	NULL	},
	{	"net_netmask",			DEFAULT_NETMASK,					NULL,	NULL	},
	{	"net_gateway",			DEFAULT_GATEWAY_IP,					NULL,	NULL	},
	{	"net_dns",				DEFAULT_DNS_IP,						NULL,	NULL	},
	{	"net_smb_workgroup",	DEFAULT_WORKGROUP,					NULL,	NULL	},
	{	"net_smb_wins",			"",									NULL,	NULL	},
	{	"net_smb_debug",		"0",								NULL,	NULL	},

	{	"ftp_enable",			"1",								NULL,	NULL	},
	{	"ftp_port",				DEFAULT_FTP_PORT,					NULL,	NULL	},
	{	"ftp_anonymous",		"0",								NULL,	NULL	},
	{	"ftp_login",			DEFAULT_FTP_LOGIN,					NULL,	NULL	},
	{	"ftp_password",			DEFAULT_FTP_PASSW,					NULL,	NULL	},

	{	"radio_cache_list",		"0",								NULL,	NULL	},

	{	"part_mount_list",		"",									NULL,	NULL	},

	{	"lang_file",			"english.xml",						NULL,	NULL	},
	{	"lang_charset",			DEFAULT_CHARSET,					NULL,	NULL	},

	{	"skin_name",			"default",							NULL,	NULL	},

	{	"hdd_boot_part",		"",									NULL,	NULL	},
	{	"hdd_boot_path",		"",									NULL,	NULL	},

	{	"time_timezone",		"0",								NULL,	NULL	},
	{	"time_dst",				"0",								NULL,	NULL	}

};

pair_t	*listHead = NULL;

//
// SC_LoadConfig - Attempts to load CONFIG.DAT and sets up linked list.
//
//				   Returns 1 if CONFIG.DAT was successfully loaded,
//				   otherwise 0.
//

int SC_LoadConfig( int bSafeMode )
{
	int			i, len;
	FHANDLE		fHandle;

	char		*p, *pEnd;
	char		strLine[1024];
	char		strKey[256];
	char		strValue[256];
	char		szFileName[MAX_PATH + 1];

	char		*pBuffer, *pBufPtr;

	// intialize linked list with default values
	for( i = 0; i < sizeof(SC_DEFAULTS) / sizeof(pair_t); i++ )
	{
		SC_SetValueForKey_Str( SC_DEFAULTS[i].key, SC_DEFAULTS[i].value );
	}

	// don't load config.dat in safe mode just use default values
	if( bSafeMode )
		return 1;

	snprintf( szFileName, sizeof(szFileName), "%sCONFIG.DAT", GetElfPath() );

	fHandle = FileOpen( szFileName, O_RDONLY );

	if( fHandle.fh < 0 )
		return 0;

	len = FileSeek( fHandle, 0, SEEK_END );
	FileSeek( fHandle, 0, SEEK_SET );

	if( len == 0 ) {
		FileClose(fHandle);
		return 0;
	}

	// read whole file into memory buffer
	pBuffer = (char*) malloc( len + 1 );
	if( pBuffer == NULL ) {
		FileClose(fHandle);
		return 0;
	}

	i = FileRead( fHandle, pBuffer, len );
	pBuffer[ i ] = 0;
	FileClose(fHandle);

	pBufPtr = pBuffer;

	while( ReadBufLine( &pBufPtr, strLine ) )
	{
		// skip comments and empty lines
		if( strLine[0] == '#' || strLine[0] == '\n' || (strLine[0] == '\r' && strLine[1] == '\n') )
			continue;

		// grab key
		p = strchr( strLine, '=' );

		// invalid line, go on to next line
		if( !p ) {
			printf("SC_LoadConfig : Ignoring Invalid Line\n");
			continue;
		}

		len = p - strLine;

		strncpy( strKey, strLine, len );
		strKey[ len ] = 0;

		// remove trailing whitespaces from token
		pEnd = strKey + strlen(strKey) - 1;

		while( *pEnd == ' ' || *pEnd == '\t' ) {
			*pEnd = 0;
			pEnd--;
		}

		// grab value
		pEnd = strLine + strlen(strLine) - 1;

		// skip '='
		p++;

		// remove leading whitespaces from token
		while( *p == ' '  || *p == '\t' )
			p++;

		// remove line end and trailing whitespaces from token
		while( *pEnd == ' ' || *pEnd == '\t' || *pEnd == '\n' || *pEnd == '\r' ) {
			*pEnd = 0;
			pEnd--;
		}

		len = pEnd - p + 1;

		if( *p ) {
			strncpy( strValue, p, len );
			strValue[ len ] = 0;
		}
		else {
			strValue[0] = 0;
		}

		// add pair to linked list
		SC_SetValueForKey_Str( strKey, strValue );
	}

	free( pBuffer );
	return 1;
}

//
// SC_SaveConfig - Attempts to save CONFIG.DAT.
//
//				   Returns 1 if CONFIG.DAT was successfully saved,
//				   otherwise 0.
//

int SC_SaveConfig( void )
{

	FHANDLE			fHandle;
	int				numTabs, i;
	const pair_t	*pair;
	char			szFileName[MAX_PATH + 1];

	snprintf( szFileName, sizeof(szFileName), "%sCONFIG.DAT", GetElfPath() );

	fHandle = FileOpen( szFileName, O_RDWR | O_CREAT | O_TRUNC );

	if( fHandle.fh < 0 )
		return 0;

	// just write out all pairs
	pair = listHead;

	while(pair) {
		if( (pair->value == NULL) || (pair->value[0] == 0) ) {
			pair = pair->next;
			continue;
		}

		FileWrite( fHandle, pair->key, strlen(pair->key) );

		numTabs = 3 - strlen(pair->key) / 8;

		for( i = 0; i < numTabs; i++ )
			FileWrite( fHandle, "\t", strlen("\t") );

		FileWrite( fHandle, "=", strlen("=") );
		FileWrite( fHandle, " ", strlen(" ") );

		FileWrite( fHandle, pair->value, strlen(pair->value) );
		FileWrite( fHandle, "\r\n", strlen("\r\n") );

		pair = pair->next;
	}

	FileClose(fHandle);
	return 1;
}

//
// SC_Clean - frees up allocated memory
//

void SC_Clean( void )
{
	pair_t *p = listHead;
	pair_t *t;

	while(p) {
		t = p;
		p = p->next;

		if( t->key )
			free( t->key );

		if( t->value )
			free( t->value );

		free(t);
	}

	listHead = NULL;
}

//
// SC_GetValueForKey_Str
//

const char *SC_GetValueForKey_Str( const char *pKey, char *pResult )
{
	const pair_t *p = listHead;

	while(p) {
		if( !strcmp( p->key, pKey ) ) {
			if( pResult )
				strcpy( pResult, p->value );
			return p->value;
		}

		p = p->next;
	}

	return NULL;
}

//
// SC_GetValueForKey_Int
//

int SC_GetValueForKey_Int( const char *pKey, int *pResult )
{
	const pair_t *p = listHead;

	while(p) {
		if( !strcmp( p->key, pKey ) ) {
			if( pResult )
				*pResult = atoi(p->value);
			return atoi(p->value);
		}

		p = p->next;
	}

	return -1;
}

//
// SC_GetValueForKey_Double
//

double SC_GetValueForKey_Double( const char *pKey, double *pResult )
{
	const pair_t *p = listHead;

	while(p) {
		if( !strcmp( p->key, pKey ) ) {
			if( pResult )
				*pResult = atof(p->value);
			return atof(p->value);
		}

		p = p->next;
	}

	return -1;
}

//
// SC_SetValueForKey_Str - Adds or modifies entry in linked list.
//						   Returns 1 if entry was modified, 0 if entry
//						   was added to the end of the list.
//

int SC_SetValueForKey_Str( const char *pKey, const char *pValue )
{
	pair_t *l = NULL;
	pair_t *p = listHead;

	while(p) {
		if( !strcmp( p->key, pKey ) ) {
			// modify existing
			free( p->value );
			p->value = (char*) malloc( strlen(pValue) + 1 );
			strcpy( p->value, pValue );

			return 1;
		}

		l = p;
		p = p->next;
	}

	// not found, add to the end of linked list
	p = (pair_t*) malloc( sizeof(pair_t) );

	p->next = NULL;
	
	if( l == NULL ) {
		p->prev = NULL;

		listHead = p;
	}
	else {
		p->prev = l;
		l->next	= p;
	}

	p->key		= (char*) malloc( strlen(pKey) + 1 );
	p->value	= (char*) malloc( strlen(pValue) + 1 );

	strcpy( p->key, pKey );
	strcpy( p->value, pValue );

	return 0;
}

//
// SC_SetValueForKey_Int - Adds or modifies entry in linked list.
//						   Returns 1 if entry was modified, 0 if entry
//						   was added to the end of the list.
//

int SC_SetValueForKey_Int( const char *pKey, int nValue )
{
	char value[32];
	
	sprintf( value, "%i", nValue );

	return SC_SetValueForKey_Str( pKey, value );
}

//
// SC_SetValueForKey_Double - Adds or modifies entry in linked list.
//						      Returns 1 if entry was modified, 0 if entry
//						      was added to the end of the list.
//

int SC_SetValueForKey_Double( const char *pKey, double fValue )
{
	char value[32];

	sprintf( value, "%f", fValue );

	return SC_SetValueForKey_Str( pKey, value );
}

//
// ReadBufLine - Reads a line from a buffer. Newline character is
//				 included in returned string. If end of buffer
//				 is reached 0 is returned.
//

int ReadBufLine( char **ppBuffer, char *pLine )
{
	while( **ppBuffer ) {
		*pLine = **ppBuffer;
		*pLine++;

		if( **ppBuffer == '\n' ) {
			(*ppBuffer)++;
			*pLine = 0;
			return 1;
		}

		(*ppBuffer)++;
	}
	return 0;
}
