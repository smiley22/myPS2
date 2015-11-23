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

#include <charset.h>
#include <sysconf.h>

static iconv_t iconvHandle = (iconv_t) -1;

void CharsetConvert_Reset( void )
{
	if( iconvHandle == (iconv_t) -1 )
		iconv_close( iconvHandle );
	
	iconvHandle = (iconv_t) - 1;
}

int CharsetConvert_UTF8ToCharset( char *lpDst, const char *lpSrc, unsigned int nDstSize )
{
	size_t inBytes, outBytes;

	if( iconvHandle == (iconv_t) -1 ) {
		iconvHandle = iconv_open( SC_GetValueForKey_Str( "lang_charset", NULL ), "UTF-8" );

		if( iconvHandle == (iconv_t) - 1 )
			return 0;
	}

	inBytes		= strlen(lpSrc) + 1;
	outBytes	= nDstSize - 1;

	if( iconv( iconvHandle, &lpSrc, &inBytes, &lpDst, &outBytes ) == - 1 )
	{
		// if failed to convert just copy string
		strcpy( lpDst, lpSrc );
	}

	return outBytes;
}
