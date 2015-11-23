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
# Date: 17-7-2005
#
#	Based on 'Accelerated Smooth Bitmap Resizing'
#	article written by Ivaylo Byalkov.
#
*/

#ifndef _IMGSCALE_H
#define _IMGSCALE_H

#include <tamtypes.h>
#include <string.h>
#include <stdlib.h>

int ScaleBitmap( u8 *pInBuff, u16 wWidth, u16 wHeight, u8 **pOutBuff, u16 wNewWidth, u16 wNewHeight );

int ShrinkData( u8 *pInBuff, u16 wWidth, u16 wHeight, u8 *pOutBuff, u16 wNewWidth, u16 wNewHeight );
int EnlargeData( u8 *pInBuff, u16 wWidth, u16 wHeight, u8 *pOutBuff, u16 wNewWidth, u16 wNewHeight );
int *CreateCoeffInt( int nLen, int nNewLen, int bShrink );

#endif
