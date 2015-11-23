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

static int log( int Value )
{
	int r = 0;
	
	Value--;

	while( Value > 0 )
	{
		Value = Value >> 1;
		r++;
	}
	
	return r;
}

u32 gsLib_texture_size( int width, int height, int psm )
{
	switch( psm )
	{
		case GS_PSM_CT32:
		case GS_PSM_CT24:
			return ( width * height * 4 );

		case GS_PSM_T8:
			return ( width * height );

		case GS_PSM_T4:
			return ( width * height / 2 );

		default:
			return -1;
	}
}

void gsLib_texture_send( u32 *mem, int width, int height, u32 tbp, u32 psm )
{
	u64* p_store, *p_data;
	u32* p_mem;
	int packets;
	int remain;
	int qwc;

	qwc = gsLib_texture_size( width, height, psm ) / 16;
	if( gsLib_texture_size( width, height, psm ) % 16 )
		qwc++;

	packets = qwc / DMA_MAX_SIZE;
	remain  = qwc % DMA_MAX_SIZE;
	p_mem   = (u32*)mem;

	p_store = p_data = dmaKit_spr_alloc( (10 + packets + (remain > 0) ) * 16 );

	FlushCache(0);

	// DMA DATA
	*p_data++ = DMA_TAG( 6, 0, DMA_CNT, 0, 0, 0 );
	*p_data++ = 0;

	*p_data++ = GIF_TAG( 4, 1, 0, 0, 0, 1 );
	*p_data++ = GIF_AD;


	if( width / 64 > 0 )
	{
		// texture buffer width must be at least as wide as texture's width
		if( (width % 64) )
			*p_data++ = GS_SETREG_BITBLTBUF(0, 0, 0, tbp / 256, 1 + (width / 64), psm);
		else
			*p_data++ = GS_SETREG_BITBLTBUF(0, 0, 0, tbp / 256, width / 64, psm);
	}
	else
	{
		*p_data++ = GS_SETREG_BITBLTBUF(0, 0, 0, tbp / 256, 1, psm);
	}

	*p_data++ = GS_BITBLTBUF;

	*p_data++ = GS_SETREG_TRXPOS(0, 0, 0, 0, 0);
	*p_data++ = GS_TRXPOS;

	*p_data++ = GS_SETREG_TRXREG(width, height);
	*p_data++ = GS_TRXREG;

	*p_data++ = GS_SETREG_TRXDIR(0);
	*p_data++ = GS_TRXDIR;

	*p_data++ = GIF_TAG( qwc, 1, 0, 0, 2, 1 );
	*p_data++ = 0;

	while( packets-- > 0 )
	{
		*p_data++ = DMA_TAG( DMA_MAX_SIZE, 1, DMA_REF, 0, (u32)p_mem, 0 );
		*p_data++ = 0;
		
		p_mem+= (DMA_MAX_SIZE * 16);
	}
	if( remain > 0 )
	{
		*p_data++ = DMA_TAG( remain, 1, DMA_REF, 0, (u32)p_mem, 0 );
		*p_data++ = 0;
	}

	*p_data++ = DMA_TAG( 2, 0, DMA_END, 0, 0, 0 );
	*p_data++ = 0;

	*p_data++ = GIF_TAG( 1, 1, 0, 0, 0, 1 );
	*p_data++ = GIF_AD;

	*p_data++ = GS_TEXFLUSH;
	*p_data++ = 0;
	
	dmaKit_send_chain_spr( DMA_CHANNEL_GIF, 0, p_store);
	dmaKit_wait( DMA_CHANNEL_GIF, 0 );
}

GSTEXTURE *gsLib_texture_raw( u32 width, u32 height, u32 psm, void *ee_mem, u8 type,
							  GSTEXTURE *clut )
{
	GSTEXTURE *texture;
	int size;

	if( !(texture = malloc(sizeof(GSTEXTURE))) )
		return NULL;

	size			= gsLib_texture_size( width, height, psm );
	texture->Vram	= gsLib_vram_alloc( size );

	if( texture->Vram == GS_ALLOC_ERROR )
	{
		printf("FATAL: VRAM alloc failed!\n");
		free(texture);
		return NULL;
	}

	gsLib_texture_send( ee_mem, width, height, texture->Vram, psm );

	texture->Height = height;
	texture->Width	= width;
	texture->PSM	= psm;
	texture->Type	= type;
	texture->Clut	= clut;

	return texture;
}

void gsLib_texture_free( GSTEXTURE *texture )
{
	gsLib_vram_free( texture->Vram );
	free(texture);
}

void gsLib_prim_sprite_texture( const GSTEXTURE *Texture, int x, int y, int w, int h,
								int u0, int v0, int u1, int v1, u64 color )
{
	u32 x2, y2;
	u64* p_store, *p_data;
	int size;
	int nTexBufWidth;

	x2 = x + w;
	y2 = y + h;

	gsLib_adjust( &x, GS_AXIS_X ); gsLib_adjust( &x2, GS_AXIS_X );
	gsLib_adjust( &y, GS_AXIS_Y ); gsLib_adjust( &y2, GS_AXIS_Y );
	
	size	= 8;
	p_store = p_data = dmaKit_spr_alloc( size * 16 );

	*p_data++ = GIF_TAG( size - 1, 1, 0, 0, 0, 1 );
	*p_data++ = GIF_AD;

	if( Texture->Width / 64 > 0 )
	{
		if( (Texture->Width % 64) )
			nTexBufWidth = 1 + (Texture->Width / 64);
		else
			nTexBufWidth = Texture->Width / 64;
	}
	else
	{
		nTexBufWidth = 1;
	}

	if( Texture->Type == GS_CLUT_TEXTURE )
	{
		*p_data++ = GS_SETREG_TEX0( Texture->Vram / 256, nTexBufWidth, Texture->PSM,
									log(Texture->Width), log(Texture->Height),
									1, 0, Texture->Clut->Vram / 256, Texture->Clut->PSM,
									0, 0, 1 );
	}
	else
	{
		*p_data++ = GS_SETREG_TEX0( Texture->Vram / 256, nTexBufWidth, Texture->PSM,
									log(Texture->Width), log(Texture->Height),
									1, 0, 0, 0, 0, 0, 1 );
	}
	
	*p_data++ = GS_TEX0_1;
 
	*p_data++ = GS_SETREG_PRIM( GS_PRIM_PRIM_SPRITE, 0, 1, 0, 1, 0, 1, 0, 0 );
	*p_data++ = GS_PRIM;

	*p_data++ = color;
	*p_data++ = GS_RGBAQ;

	*p_data++ = GS_SETREG_UV( u0 << 4, v0 << 4 );
	*p_data++ = GS_UV;

	*p_data++ = GS_SETREG_XYZ( x, y, 0 );
	*p_data++ = GS_XYZ2;

	*p_data++ = GS_SETREG_UV( u1 << 4, v1 << 4 );
	*p_data++ = GS_UV;

	*p_data++ = GS_SETREG_XYZ( x2, y2, 0 );
	*p_data++ = GS_XYZ2;

	dmaKit_send_spr( DMA_CHANNEL_GIF, 0, p_store, size );
}
