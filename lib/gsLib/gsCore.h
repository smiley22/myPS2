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

typedef struct gsMem_s
{
	u32				address;
	u32				size;
	struct gsMem_s	*next;
	struct gsMem_s	*prev;
} GSMEM;

typedef struct
{
	u8		Mode;
	u8		DoubleBuffering;
	u8		PSM;
	u32		Width;
	u32		Height;
 	u32		FrameBuffer[2];
	u32		ActiveBuffer;
	GSMEM	*gs_mem_head;
	u32		OffsetX;
	u32		OffsetY;

	GSTEXTURE *FontClut;
} GSGLOBAL;


#define GS_MODE_NTSC			0x02
#define GS_MODE_PAL				0x03
#define GS_MODE_AUTO			0xFF

#define GS_PSM_CT32				0x00
#define GS_PSM_CT24				0x01
#define GS_PSM_T8				0x13
#define GS_PSM_T4				0x14

#define GS_DOUBLE_BUFFERING		0x01

#define GS_MEM_SIZE				4194304
#define	GS_VRAM_BLOCKSIZE		8192
#define GS_ALLOC_ERROR			0xFFFFFFFF

#define	GS_AXIS_X				0
#define GS_AXIS_Y				1

// GS Privileged Registers

#define GS_REG_CSR				(volatile u64 *) 0x12001000

#define GS_SET_CSR(A,B,C,D,E,F,G,H,I,J,K,L) \
	*GS_REG_CSR = \
	(u64)((A) & 0x00000001) <<  0 | (u64)((B) & 0x00000001) <<  1 | \
	(u64)((C) & 0x00000001) <<  2 | (u64)((D) & 0x00000001) <<  3 | \
	(u64)((E) & 0x00000001) <<  4 | (u64)((F) & 0x00000001) <<  8 | \
	(u64)((G) & 0x00000001) <<  9 | (u64)((H) & 0x00000001) << 12 | \
	(u64)((I) & 0x00000001) << 13 | (u64)((J) & 0x00000003) << 14 | \
	(u64)((K) & 0x000000FF) << 16 | (u64)((L) & 0x000000FF) << 24

#define GS_REG_PMODE			(volatile u64 *) 0x12000000

#define GS_SET_PMODE(A,B,C,D,E,F) \
	*GS_REG_PMODE = \
	(u64)((A) & 0x00000001) <<  0 | (u64)((B) & 0x00000001) <<  1 | \
	(u64)((1) & 0x00000007) <<  2 | (u64)((C) & 0x00000001) <<  5 | \
	(u64)((D) & 0x00000001) <<  6 | (u64)((E) & 0x00000001) <<  7 | \
	(u64)((F) & 0x000000FF) <<  8

#define GS_REG_DISPLAY2			(volatile u64 *) 0x120000A0

#define GS_SET_DISPLAY(A,B,C,D,E,F) \
	*GS_REG_DISPLAY2 = \
	(u64)((A) & 0x00000FFF) <<  0 | (u64)((B) & 0x000007FF) << 12 | \
	(u64)((C) & 0x0000000F) << 23 | (u64)((D) & 0x00000003) << 27 | \
	(u64)((E) & 0x00000FFF) << 32 | (u64)((F) & 0x000007FF) << 44

#define GS_REG_DISPFB2			(volatile u64 *) 0x12000090

#define GS_SET_DISPFB(A,B,C,D,E) \
	*GS_REG_DISPFB2 = \
	(u64)((A) & 0x000001FF) <<  0 | (u64)((B) & 0x0000003F) <<  9 | \
	(u64)((C) & 0x0000001F) << 15 | (u64)((D) & 0x000007FF) << 32 | \
	(u64)((E) & 0x000007FF) << 43

// General Purpose Registers

#define GS_PRMODECONT			0x1A

#define GS_FRAME_1				0x4C

#define GS_SETREG_FRAME(fbp, fbw, psm, fbmask) \
	((u64)(fbp)			| ((u64)(fbw) << 16) | \
	((u64)(psm) << 24)	| ((u64)(fbmask) << 32))

#define GS_SCISSOR_1			0x40

#define GS_SETREG_SCISSOR(scax0, scax1, scay0, scay1) \
	((u64)(scax0)        | ((u64)(scax1) << 16) | \
	((u64)(scay0) << 32) | ((u64)(scay1) << 48))

#define GS_XYOFFSET_1			0x18

#define GS_SETREG_XYOFFSET(ofx, ofy) \
	((u64)(ofx) | ((u64)(ofy) << 32))

#define GS_TEST_1				0x47

#define GS_SETREG_TEST(ate, atst, aref, afail, date, datm, zte, ztst) \
	((u64)(ate)         | ((u64)(atst) << 1)	| \
	((u64)(aref) << 4)  | ((u64)(afail) << 12)	| \
	((u64)(date) << 14) | ((u64)(datm) << 15)	| \
	((u64)(zte) << 16)  | ((u64)(ztst) << 17))

#define GS_ALPHA_1				0x42

#define GS_SETREG_ALPHA(A,B,C,D,E) \
	(u64)((A) & 0x00000003) <<  0 | (u64)((B) & 0x00000003) <<  2 | \
	(u64)((C) & 0x00000003) <<  4 | (u64)((D) & 0x00000003) <<  6 | \
	(u64)((E) & 0x000000FF) << 32

#define GS_TEXA					0x3B

#define GS_SETREG_TEXA(ta0, aem, ta1) \
	((u64)(ta0) | ((u64)(aem) << 15) | ((u64)(ta1) << 32))

#define GS_PRIM					0x00

#define GS_SETREG_PRIM(prim, iip, tme, fge, abe, aa1, fst, ctxt, fix) \
	((u64)(prim)      | ((u64)(iip) << 3)  | ((u64)(tme) << 4) | \
	((u64)(fge) << 5) | ((u64)(abe) << 6)  | ((u64)(aa1) << 7) | \
	((u64)(fst) << 8) | ((u64)(ctxt) << 9) | ((u64)(fix) << 10))

#define GS_RGBAQ				0x01

#define GS_SETREG_RGBAQ(r, g, b, a, q) \
	((u64)(r)        | ((u64)(g) << 8) | ((u64)(b) << 16) | \
	((u64)(a) << 24) | ((u64)(q) << 32))

#define GS_XYZ2					0x05

#define GS_SETREG_XYZ(x, y, z) \
	((u64)(x) | ((u64)(y) << 16) | ((u64)(z) << 32))

#define GS_BITBLTBUF			0x50

#define GS_SETREG_BITBLTBUF(sbp, sbw, spsm, dbp, dbw, dpsm) \
	((u64)(sbp)         | ((u64)(sbw) << 16) | \
	((u64)(spsm) << 24) | ((u64)(dbp) << 32) | \
	((u64)(dbw) << 48)  | ((u64)(dpsm) << 56))

#define GS_TRXPOS				0x51

#define GS_SETREG_TRXPOS(ssax, ssay, dsax, dsay, dir) \
	((u64)(ssax)        | ((u64)(ssay) << 16) | \
	((u64)(dsax) << 32) | ((u64)(dsay) << 48) | \
	((u64)(dir) << 59))

#define GS_TRXREG				0x52

#define GS_SETREG_TRXREG(rrw, rrh) \
	((u64)(rrw) | ((u64)(rrh) << 32))

#define GS_TRXDIR				0x53

#define GS_SETREG_TRXDIR(xdr) ((u64)(xdr))

#define GS_TEXFLUSH				0x3F

#define GS_TEX0_1				0x06

#define GS_SETREG_TEX0(tbp, tbw, psm, tw, th, tcc, tfx,cbp, cpsm, csm, csa, cld) \
	((u64)(tbp)         | ((u64)(tbw) << 14) | \
	((u64)(psm) << 20)  | ((u64)(tw) << 26) | \
	((u64)(th) << 30)   | ((u64)(tcc) << 34) | \
	((u64)(tfx) << 35)  | ((u64)(cbp) << 37) | \
	((u64)(cpsm) << 51) | ((u64)(csm) << 55) | \
	((u64)(csa) << 56)  | ((u64)(cld) << 61))

#define GS_UV					0x03

#define GS_SETREG_UV(u, v) ((u64)(u) | ((u64)(v) << 16))

#define GIF_AD					0x0E

#define GIF_TAG(NLOOP,EOP,PRE,PRIM,FLG,NREG) \
	((u64)(NLOOP)	<< 0)	| \
	((u64)(EOP)		<< 15)	| \
	((u64)(PRE)		<< 46)	| \
	((u64)(PRIM)	<< 47)	| \
	((u64)(FLG)		<< 58)	| \
	((u64)(NREG)    << 60);

#define GS_PRIM_PRIM_SPRITE		6

void	gsLib_init( u8 mode, u32 flags );
u32		gsLib_vram_alloc( u32 size );
void	gsLib_vram_free( u32 address );
void	gsLib_vsync( void );
void	gsLib_swap( void );
int		gsLib_adjust( u32 *value, u8 axis );
void	gsLib_clear( u64 color );
int		gsLib_get_mode( void );
int		gsLib_get_width( void );
int		gsLib_get_height( void );
void	gsLib_set_offset( u32 OffsetX, u32 OffsetY );
