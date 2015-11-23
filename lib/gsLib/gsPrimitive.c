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

void gsLib_prim_sprite( u32 x, u32 y, u32 w, u32 h, u64 color )
{
	u32 x2, y2;
	u64* p_store, *p_data;
	int size = 5;

	x2 = x + w;
	y2 = y + h;

	gsLib_adjust( &x, GS_AXIS_X ); gsLib_adjust( &x2, GS_AXIS_X );
	gsLib_adjust( &y, GS_AXIS_Y ); gsLib_adjust( &y2, GS_AXIS_Y );

	p_store = p_data = dmaKit_spr_alloc( size * 16 );

	*p_data++ = GIF_TAG( size - 1, 1, 0, 0, 0, 1 );
	*p_data++ = GIF_AD;

	*p_data++ = GS_SETREG_PRIM( GS_PRIM_PRIM_SPRITE, 0, 0, 0, 1, 0, 0, 0, 0) ;
	*p_data++ = GS_PRIM;

	*p_data++ = color;
	*p_data++ = GS_RGBAQ;

	*p_data++ = GS_SETREG_XYZ( x, y, 0 );
	*p_data++ = GS_XYZ2;

	*p_data++ = GS_SETREG_XYZ( x2, y2, 0 );
	*p_data++ = GS_XYZ2;
	
	dmaKit_send_spr( DMA_CHANNEL_GIF, 0, p_store, size );
}
