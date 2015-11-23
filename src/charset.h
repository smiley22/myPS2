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

#ifndef _CHARSET_H_
#define _CHARSET_H_

#include <tamtypes.h>
#include <string.h>
#include <../lib/libiconv/iconv.h>

#define DEFAULT_CHARSET	"ISO-8859-1"

typedef struct
{
	char	*lpCharset;
	char	*lpDesc;

} Charset_t;

void	CharsetConvert_Reset( void );
int		CharsetConvert_UTF8ToCharset( char *lpDst, const char *lpSrc, unsigned int nDstSize );

#endif
