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
# Date: 11-15-2005
#
# File: System Config Header file
#
*/

#ifndef _SYSCONF_H
#define _SYSCONF_H

typedef struct pair_s pair_t;

struct pair_s {
	char	*key;
	char	*value;
	pair_t	*next;
	pair_t	*prev;
};

#define DEFAULT_NET_IP		"192.168.0.10"
#define DEFAULT_NETMASK		"255.255.255.0"
#define DEFAULT_GATEWAY_IP	"192.168.0.1"
#define DEFAULT_DNS_IP		"0.0.0.0"
#define DEFAULT_WORKGROUP	"WORKGROUP"

#define DEFAULT_FTP_PORT	"21"
#define DEFAULT_FTP_LOGIN	"myPS2"
#define DEFAULT_FTP_PASSW	"myPS2"

int SC_LoadConfig( int bSafeMode );
int SC_SaveConfig( void );
void SC_Clean( void );

const char *SC_GetValueForKey_Str( const char *pKey, char *pResult );
int SC_GetValueForKey_Int( const char *pKey, int *pResult );
double SC_GetValueForKey_Double( const char *pKey, double *pResult );

int SC_SetValueForKey_Str( const char *pKey, const char *pValue );
int SC_SetValueForKey_Int( const char *pKey, int nValue );
int SC_SetValueForKey_Double( const char *pKey, double fValue );

int ReadBufLine( char **ppBuffer, char *pLine );

#endif
