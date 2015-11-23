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

GSGLOBAL gsGlobal;

void gsLib_init( u8 mode, u32 flags )
{
	u64	*p_data, *p_store;
	u32 size;

	ResetEE(0x08);

	if( mode == GS_MODE_AUTO )
	{
		if( *(volatile char *)(0x1FC7FF52) == 'E' )
			mode = GS_MODE_PAL;
		else
			mode = GS_MODE_NTSC;
	}

	gsGlobal.PSM				= GS_PSM_CT32;
	gsGlobal.DoubleBuffering	= flags & GS_DOUBLE_BUFFERING;
	gsGlobal.ActiveBuffer		= 1;
	gsGlobal.gs_mem_head		= NULL;
	gsGlobal.OffsetX			= 0;
	gsGlobal.OffsetY			= 0;
	gsGlobal.FontClut			= NULL;

	GS_SET_CSR( 0, 0, 0, 0, 0, 1, 1, 0, 0, 0, 0, 0 );
	SetGsCrt( 1, mode, 0 );
	GS_SET_PMODE( 0, 1, 0, 0, 0, 0x80 );

	switch( mode )
	{
		case GS_MODE_NTSC:
		default:
			gsGlobal.Width	= 640;
			gsGlobal.Height	= 448;
			gsGlobal.Mode	= GS_MODE_NTSC;

			GS_SET_DISPLAY( 632, 50, 3, 0, 2559,  447 );
			break;

		case GS_MODE_PAL:
			gsGlobal.Width	= 640;
			gsGlobal.Height	= 512;
			gsGlobal.Mode	= GS_MODE_PAL;

			GS_SET_DISPLAY( 652, 72, 3, 0, 2559,  511 );
			break;
	}

	GS_SET_DISPFB( 0, gsGlobal.Width >> 6, gsGlobal.PSM, 0, 0 );

	gsGlobal.FrameBuffer[0] = gsLib_vram_alloc( gsLib_texture_size( gsGlobal.Width,
												gsGlobal.Height, gsGlobal.PSM ) );

	if( gsGlobal.DoubleBuffering )
	{
		gsGlobal.FrameBuffer[1] = gsLib_vram_alloc( gsLib_texture_size( gsGlobal.Width,
													gsGlobal.Height, gsGlobal.PSM ) );
	}
	else
	{
		gsGlobal.FrameBuffer[1] = gsGlobal.FrameBuffer[0];
	}

	size	= 8;
	p_data	= p_store = dmaKit_spr_alloc( size * 16 );

	*p_data++ = GIF_TAG( size - 1, 1, 0, 0, 0, 1 );
	*p_data++ = GIF_AD;

	*p_data++ = 1;
	*p_data++ = GS_PRMODECONT;

	*p_data++ =	GS_SETREG_FRAME( gsGlobal.FrameBuffer[1] / 8192, gsGlobal.Width / 64, gsGlobal.PSM, 0 );
	*p_data++ = GS_FRAME_1;

	*p_data++ = GS_SETREG_SCISSOR( 0, gsGlobal.Width - 1, 0, gsGlobal.Height - 1 );
	*p_data++ = GS_SCISSOR_1;

	*p_data++ = GS_SETREG_XYOFFSET(	(2048 - (gsGlobal.Width  >>	1)) << 4,
									(2048 - (gsGlobal.Height >> 1)) << 4 );
	*p_data++ = GS_XYOFFSET_1;

	*p_data++ = GS_SETREG_TEST( 0, 0, 0, 0, 0, 0, 0, 0 );
	*p_data++ = GS_TEST_1;

	*p_data++ = GS_SETREG_ALPHA( 0, 1, 0, 1, 0 );
	*p_data++ = GS_ALPHA_1;

	*p_data++ = GS_SETREG_TEXA( 0x80, 0, 0 );
	*p_data++ = GS_TEXA;

	dmaKit_send_spr( DMA_CHANNEL_GIF, 0, p_store, size );

	// init font clut
//	gsLib_font_init();
}

u32 gsLib_vram_alloc( u32 size )
{
	GSMEM *ptr, *last;
	u32 space;

	size = (-GS_VRAM_BLOCKSIZE) & (size + GS_VRAM_BLOCKSIZE - 1);
	if( size >= GS_MEM_SIZE )
		return GS_ALLOC_ERROR;

	if( gsGlobal.gs_mem_head == NULL ) {
		ptr = malloc( sizeof(GSMEM) );

		gsGlobal.gs_mem_head = ptr;

		ptr->address	= 0;
		ptr->prev		= NULL;
		ptr->next		= NULL;
		ptr->size		= size;

		//printf("Allocated head space at address %i for %i bytes\n", ptr->address, ptr->size);
		return ptr->address;
	}

	last	= gsGlobal.gs_mem_head;
	ptr		= gsGlobal.gs_mem_head->next;

	while(ptr) {
		space = ptr->address - (ptr->prev->address +
				ptr->prev->size );

		if( space >= size ) {
			GSMEM *node = malloc( sizeof(node) );

			node->prev			= ptr->prev;
			node->next			= ptr;
			node->size			= size;
			node->address		= ptr->prev->address + ptr->prev->size;

			node->prev->next	= node;
			node->next->prev	= node;

			//printf("Allocated in between space at address %i for %i bytes\n", node->address, node->size);
			return node->address;
		}

		last	= ptr;
		ptr		= ptr->next;
	}

	if( (GS_MEM_SIZE - (last->address + last->size)) < size )
		return GS_ALLOC_ERROR;

	ptr = malloc( sizeof(GSMEM) );

	ptr->size		= size;
	ptr->next		= NULL;
	ptr->address	= last->address + last->size;

	ptr->prev		= last;
	last->next		= ptr;

	//printf("Allocated end space at address %i for %i bytes\n", ptr->address, ptr->size);
	return ptr->address;
}

void gsLib_vram_free( u32 address )
{
	GSMEM *node = gsGlobal.gs_mem_head;

	while(node) {
		if( node->address == address ) {
			if( node->prev )
				node->prev->next = node->next;

			if( node->next )
				node->next->prev = node->prev;

			if( node == gsGlobal.gs_mem_head )
				gsGlobal.gs_mem_head = NULL;

			//printf("freeing vram block at address %i of size %i\n", node->address, node->size);
			free(node);
			return;
		}
		node = node->next;
	}
}

void gsLib_vsync( void )
{
	*GS_REG_CSR = *GS_REG_CSR & 8;

	while( !(*GS_REG_CSR & 8) );
}

void gsLib_swap( void )
{
	u64 *p_data, *p_store;

	if( !gsGlobal.DoubleBuffering )
		return;

	gsLib_vsync();

	GS_SET_DISPFB(	gsGlobal.FrameBuffer[ gsGlobal.ActiveBuffer & 1 ] / 8192,
					gsGlobal.Width >> 6, gsGlobal.PSM, 0, 0 );

	gsGlobal.ActiveBuffer ^= 1;

	p_data = p_store = dmaKit_spr_alloc( 2 * 16 );

	*p_data++ = GIF_TAG( 1, 1, 0, 0, 0, 1 );
	*p_data++ = GIF_AD;

	*p_data++ = GS_SETREG_FRAME(	gsGlobal.FrameBuffer[ gsGlobal.ActiveBuffer & 1 ] / 8192,
									gsGlobal.Width / 64, gsGlobal.PSM, 0 );
	*p_data++ = GS_FRAME_1;

	dmaKit_send_spr( DMA_CHANNEL_GIF, 0, p_store, 2 );
}

int gsLib_adjust( u32 *value, u8 axis )
{
	if( axis == GS_AXIS_X )
	{
		*value += (2048 - gsGlobal.Width / 2);
		*value += gsGlobal.OffsetX;
	}
	else if( axis == GS_AXIS_Y )
	{
		*value += (2048 - gsGlobal.Height / 2);
		*value += gsGlobal.OffsetY;
	}

	*value = *value << 4;

	return *value;
}

void gsLib_clear( u64 color )
{
	gsLib_prim_sprite( 0, 0, gsGlobal.Width, gsGlobal.Height, color );
}

int gsLib_get_mode( void )
{
	return gsGlobal.Mode;
}

int gsLib_get_width( void )
{
	return gsGlobal.Width;
}

int gsLib_get_height( void )
{
	return gsGlobal.Height;
}

void gsLib_set_offset( u32 OffsetX, u32 OffsetY )
{
	gsGlobal.OffsetX = OffsetX;
	gsGlobal.OffsetY = OffsetY;
}
