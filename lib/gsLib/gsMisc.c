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

#include "gsLib.h"

int gsLib_screenshot( const char *path )
{
#ifdef HAVE_LIBJPG
	int ret;

	// requires libdebug and libjpg
	ret = jpgScreenshot( path, gsGlobal.FrameBuffer[ (~gsGlobal.ActiveBuffer) & 1 ] / 256,
						 gsGlobal.Width, gsGlobal.Height, gsGlobal.PSM );

	return ret;
#else
	printf("ERROR: gsLib_screenshot not implemented\n");
	return -1;
#endif
}
