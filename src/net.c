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

/*
# myPS2
# Author: tk
# Date: 12-11-2005
#
# File: net.c
#
*/

#include <tamtypes.h>
#include <sysconf.h>	// ReadBufLine
#include <string.h>
#include <stdio.h>
#include <ps2ip.h>
#include <net.h>
#include <misc.h>
#include <loadfile.h>

//
// HttpDownload -  Attempts to download a file off an HTTP server.
//
//				   pHost	: Name or address of HTTP server
//				   nPort	: HTTP server port
//				   pFile	: Path of remote file
//				   ppBuffer : Address of a pointer that will point to the
//							  memory allocated by the function after completion.
//				   pSize	: Size in bytes of allocated memory for pBuffer
//				   pCallback: Optional pointer to a callback function 
//
//				   Returns 1 on sucess, otherwise error number.
//

int HttpDownload( const char *pHost, int nPort, const char *pFile, char **pBuffer, int *pSize, int (*callback)(int, int) )
{
	int					s;
	struct sockaddr_in	addr;
	int					nRet, nRecv, nReqOK, nContentSize, nBreak, nRead;
	int					nTotal;

	char				strGet[256];
	char				strBuf[4096];
	char				strLine[1024];
	char				*pStr, *pLine;

	s = socket( AF_INET, SOCK_STREAM, IPPROTO_TCP );
	if( s < 0 ) {
#ifdef _DEBUG
		printf("HttpDownload: socket() failed!\n");
#endif
		return HTTP_ERROR_SOCKET;
	}

	memset( &addr, 0, sizeof(addr) );

	addr.sin_family			= AF_INET;
	addr.sin_port			= htons(nPort);
	addr.sin_addr.s_addr	= inet_addr( pHost );

	// not an ip address, try to resolve the hostname
	if( addr.sin_addr.s_addr == INADDR_NONE ) {
		if( gethostbyname( (char*) pHost, &addr.sin_addr ) != 0 ) {
#ifdef _DEBUG
			printf("HttpDownload: Could not resolve hostname : %s\n", pHost);
#endif
			return HTTP_ERROR_RESOLVE;
		}
	}

	nRet = connect( s, (struct sockaddr*)&addr, sizeof(addr) );
	if( nRet < 0 ) {
#ifdef _DEBUG
		printf("HttpDownload: connect() failed!\n");
#endif
		disconnect(s);
		return HTTP_ERROR_CONNECT;
	}

	// prepare GET request
	sprintf( strGet,"GET %s HTTP/1.0\r\n\r\n", pFile );

	nRet = send( s, strGet, strlen(strGet), 0 );
	if( nRet < 0 ) {
#ifdef _DEBUG
		printf("HttpDownload: send() failed!\n");
#endif
		disconnect(s);
		return HTTP_ERROR_SEND;
	}

	// read HTTP header response
	nRecv			= 0;
	nReqOK			= 0;
	nContentSize	= 0;
	nBreak			= 0;

	// read header in 1024 chunks
	while(1) {
		nRet = recv( s, strBuf + nRecv, sizeof(strBuf) - 1 - nRecv, 0 );

		if( nRet < 0 || nRet == 0 ) {
#ifdef _DEBUG
			printf("HttpDownload: connection was reset!\n");
#endif
			disconnect(s);
			return HTTP_ERROR_RECV;
		}

		strBuf[nRet] = 0;
		pStr = strBuf;

		while(1) {
			nRead = ReadBufLine( &pStr, strLine );

			if( nRead == 0 ) {
				memcpy( strBuf, pStr, pStr - strBuf );
				nRecv = pStr - strBuf;
				break;
			}

			// end of HTTP header
			if( strLine[0] == '\r' && strLine[1] == '\n' ) {

				// remaining bytes are of file already
				nRecv = nRet - (pStr - strBuf);
				memcpy( strBuf, pStr, nRecv );

				// break out of loop
				nBreak = 1;
				break;
			}

			// check for HTTP error code...make this properly?
			if( strstr( strLine, "HTTP/1." ) ) {
				if( strstr( strLine, "200 OK" ) )
					nReqOK = 1;
			}
			else if( strstr( strLine, "Content-Length:" ) ) {

				// this is the file size in bytes
				pLine = strLine;
				pLine += strlen("Content-Length:");

				// skip whitespaces
				while( *pLine == ' ' || *pLine == '\t' )
					pLine++;

				nContentSize = atoi(pLine);
			}
		}

		// HTTP header end reached
		if( nBreak )
			break;
	}

	// file doesn't exist or some other HTTP error occured
	if( !nReqOK ) {
#ifdef _DEBUG
		printf("HttpDownload: nReqOK != 200 OK\n");
#endif
		disconnect(s);
		return HTTP_ERROR_BADREQ;
	}

	if( !nContentSize ) {
#ifdef _DEBUG
		printf("HttpDownload: (Notice) HTTP header did not contain Content-Length field!\n");
#endif
	}

	// check if it's not too big
	if( nContentSize > HTTP_MAX_FILESIZE ) {
#ifdef _DEBUG
		printf("HttpDownload: file size is too big (%i)!\n", nContentSize);
#endif
		disconnect(s);
		return HTTP_ERROR_SIZE;
	}

	*pBuffer	= NULL;
	nTotal		= 0;

	// nRecv is the number of bytes already belonging to the file that
	// were in the last strBuf chunk of the HTTP header.
	if( nRecv > 0 ) {
		*pBuffer = (char*) realloc( *pBuffer, nRecv );
		if( !(*pBuffer) ) {
#ifdef _DEBUG
			printf("HttpDownload: realloc failed for pBuffer!\n");
#endif
			disconnect(s);
			return HTTP_ERROR_MEMORY;
		}
		memcpy( *pBuffer, strBuf, nRecv );
		nTotal = nRecv;
	}

	// can start receiving the file now
	while( (nRet = recv( s, strBuf, sizeof(strBuf), 0 )) ) {

		if( nRet < 0 ) {
			printf("HttpDownload: recv() failed\n");

			free(*pBuffer);
			disconnect(s);
			return HTTP_ERROR_RECV;
		}

		// don't let the file get too big
		if( (nTotal + nRet) > HTTP_MAX_FILESIZE ) {
#ifdef _DEBUG
			printf("HttpDownload: file size exceeded HTTP_MAX_FILESIZE!\n");
#endif
			free(*pBuffer);
			disconnect(s);
			return HTTP_ERROR_SIZE;
		}

		*pBuffer = (char*) realloc( *pBuffer, nTotal + nRet );
		if( !(*pBuffer) ) {
#ifdef _DEBUG
			printf("HttpDownload: realloc failed for pBuffer!\n");
#endif
			disconnect(s);
			return HTTP_ERROR_MEMORY;
		}
		
		memcpy( (*pBuffer) + nTotal, strBuf, nRet );

		// total count of bytes received
		nTotal += nRet;

		// if a callback was specified call it
		if( callback )
			callback( nTotal, nContentSize );
	}

#ifdef _DEBUG
	if( (nContentSize > 0) && (nTotal == nContentSize) )
		printf("HttpDownload : This is good\n");
#endif

	*pSize = nTotal;

	disconnect(s);
	return 1;
}

//
// TokenizeURL - Extracts hostname, port and file from an URL in the
//				 form of http://hostname:port/file
//

int TokenizeURL( const char *pUrl, char *pHost, char *pFileName, int *pPort )
{
	char	*pStart, *pEnd;
	char	strTmp[128];
	int		nLen;

	*pPort = 80;

	if( (pStart = strchr( pUrl, ':' )) == NULL ) {
#ifdef _DEBUG
		printf("TokenizeURL: Invalid URL.\n");
#endif
		return 0;
	}

	// skip ':'
	pStart++;

	if( *(pStart++) != '/' || *(pStart++) != '/' ) {
#ifdef _DEBUG
		printf("TokenizeURL: Invalid URL.\n");
#endif
		return 0;
	}

	nLen = 0;

	// extract host
	while( *pStart ) {
		if( *pStart == ':' || *pStart == '/' )
			break;

		pHost[ nLen++ ]	= *pStart++;
		pHost[ nLen ]	= 0;
	}

	// parse custom port
	if( (*pStart) && (*pStart == ':') ) {

		// skip ':'
		pStart++;

		if( (pEnd = strchr( pStart, '/' )) != NULL ) {
			nLen = pEnd - pStart;
			strncpy( strTmp, pStart, nLen );

			strTmp[nLen]	= 0;
			pStart			= pEnd;
		}
		else {
			strcpy( strTmp, pStart );
		}

		*pPort = atoi(strTmp);
	}

	// parse filename
	if( (*pStart) && (*pStart == '/') )
		strcpy( pFileName, pStart );
	else
		strcpy( pFileName, "/" );

	return 1;
}

////////////////////////////////////////////////////////////////////////////////////

//
// dnsInit - Needs to be called with address of DNS server
//			 before making any calls to gethostbyname.
//

int dnsInit( const char *dns_addr )
{
	u32 a;
	int nSizeArg, nRet, irx_ret;

	a = inet_addr( dns_addr );
	if( a == INADDR_NONE ) {
		printf("dnsInit : %s is not a valid address!\n", dns_addr);
		return 0;
	}

	// load dns.irx
	nSizeArg	= strlen(dns_addr) + 1;
	nRet		= SifExecModuleBuffer( &dns_irx, size_dns_irx, nSizeArg, dns_addr, &irx_ret );

	if( nRet < 0 ) {
		printf("dnsInit : Failed to initialize dns irx module!\n");
		return 0;
	}

	return 1;
}

////////////////////////////////////////////////////////////////////////////////////
// copied from ps2sdksrc/iop/tcpip/

u32 inet_addr( const char *cp )
{
	struct in_addr val;
	
	if( inet_aton(cp, &val) )
		return(val.s_addr);
	
	return (INADDR_NONE);
}

/*
 * Check whether "cp" is a valid ascii representation
 * of an Internet address and convert to a binary address.
 * Returns 1 if the address is valid, 0 if not.
 * This replaces inet_addr, the return value from which
 * cannot distinguish between failure and a local broadcast address.
 */

/* inet_aton */
int inet_aton( const char *cp, struct in_addr *addr )
{
     u32 val;
     int base, n;
     char c;
     u32 parts[4];
     u32* pp = parts;

     c = *cp;
     for (;;) {
         /*
          * Collect number up to ``.''.
          * Values are specified as for C:
          * 0x=hex, 0=octal, isdigit=decimal.
          */
         if (!isdigit(c))
             return (0);
         val = 0; base = 10;
         if (c == '0') {
             c = *++cp;
             if (c == 'x' || c == 'X')
                 base = 16, c = *++cp;
             else
                 base = 8;
         }
         for (;;) {
             if (isascii(c) && isdigit(c)) {
                 val = (val * base) + (c - '0');
                 c = *++cp;
             } else if (base == 16 && isascii(c) && isxdigit(c)) {
                 val = (val << 4) |
                     (c + 10 - (islower(c) ? 'a' : 'A'));
                 c = *++cp;
             } else
             break;
         }
         if (c == '.') {
             /*
              * Internet format:
              *  a.b.c.d
              *  a.b.c   (with c treated as 16 bits)
              *  a.b (with b treated as 24 bits)
              */
             if (pp >= parts + 3)
                 return (0);
             *pp++ = val;
             c = *++cp;
         } else
             break;
     }
     /*
      * Check for trailing characters.
      */
     if (c != '\0' && (!isascii(c) || !isspace(c)))
         return (0);
     /*
      * Concoct the address according to
      * the number of parts specified.
      */
     n = pp - parts + 1;
     switch (n) {

     case 0:
         return (0);     /* initial nondigit */

     case 1:             /* a -- 32 bits */
         break;

     case 2:             /* a.b -- 8.24 bits */
         if (val > 0xffffff)
             return (0);
         val |= parts[0] << 24;
         break;

     case 3:             /* a.b.c -- 8.8.16 bits */
         if (val > 0xffff)
             return (0);
         val |= (parts[0] << 24) | (parts[1] << 16);
         break;

     case 4:             /* a.b.c.d -- 8.8.8.8 bits */
         if (val > 0xff)
             return (0);
         val |= (parts[0] << 24) | (parts[1] << 16) | (parts[2] << 8);
         break;
     }
     if (addr)
         addr->s_addr = htonl(val);
     return (1);
}
