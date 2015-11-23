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

typedef struct
{
	u8		IdentLen;
	u8		ColormapType;
	u8		ImageType;
	u8		ColormapSpec[5];
	u16		OriginX;
	u16		OriginY;
	u16		Width;
	u16		Height;
	u8		BitsPerPixel;
	u8		ImageDescriptor;

} GSLIB_TGAHEADER __attribute__((packed));

int gsLib_screenshot( const char *path );
