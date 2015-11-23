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
# Date: 11-14-2005
#
# File: ELF loader code.
#		Based on loader code by Marcus R. Brown
#
*/

#include <tamtypes.h>
#include <kernel.h>
#include <sifrpc.h>
#include <elf.h>
#include <file.h>
#include <loadfile.h>

//
// wipeUserMem - Clear user memory. Taken from PS2Link.
//

void wipeUserMem(void)
{
	int i;
	for (i = 0x100000; i < 0x2000000 ; i += 64) {
		asm (
			"\tsq $0, 0(%0) \n"
			"\tsq $0, 16(%0) \n"
			"\tsq $0, 32(%0) \n"
			"\tsq $0, 48(%0) \n"
			:: "r" (i) );
	}
}

//
// CheckELFHeader - Checks for valid ELF Header.
//					Returns 1 if ELF Header is valid, otherwise 0.
//

int CheckELFHeader( const char *path )
{
	u8				*boot_elf	= (u8 *)0x1800000;
	elf_header_t	*eh			= (elf_header_t *)boot_elf;
	int				size;
	FHANDLE			fHandle;

	fHandle = FileOpen( path, O_RDONLY );
	if( fHandle.fh < 0 )
		return 0;

	size = FileSeek( fHandle, 0, SEEK_END );
	if( !size ) {
		FileClose( fHandle );
		return 0;
	}

	FileSeek( fHandle, 0, SEEK_SET );
	FileRead( fHandle, boot_elf, 52 );
	FileClose( fHandle );

	if( (_lw((u32)&eh->ident) != ELF_MAGIC) || eh->type != 2 )
		return 0;

	return 1;
}

//
// RunELF -  Loads ELF into memory and executes it.
//
//			 If pFileName is not a valid ELF file or could not
//			 be executed, -1 is returned.
//

int RunELF( char *pFileName )
{
	u8				*boot_elf	= (u8*) 0x1800000;
	elf_header_t	*eh			= (elf_header_t*) boot_elf;
	elf_pheader_t	*eph;
	int				i, nSize;
	char			*argv[1];
	FHANDLE			fHandle;

	// Load the ELF into RAM
	fHandle = FileOpen( pFileName, O_RDONLY );

	if( fHandle.fh < 0 )
		return -1;

	FileRead( fHandle, boot_elf, 52 );

	if( (_lw((u32)&eh->ident) != ELF_MAGIC) || eh->type != 2 )
		return -1;

	FileSeek( fHandle, eh->phoff, SEEK_SET );

	eph		= (elf_pheader_t *)(boot_elf + eh->phoff);
	nSize	= eh->phnum * eh->phentsize;

	FileRead( fHandle, (void*) eph, nSize );

	// Scan through the ELF's program headers and copy them
	// into RAM, then zero out any non-loaded regions.
	for( i = 0; i < eh->phnum; i++ )
	{
		if( eph[i].type != ELF_PT_LOAD )
			continue;

		FileSeek( fHandle, eph[i].offset, SEEK_SET );
		FileRead( fHandle, eph[i].vaddr, eph[i].filesz );

		if( eph[i].memsz > eph[i].filesz )
			memset( eph[i].vaddr + eph[i].filesz, 0, eph[i].memsz - eph[i].filesz );
	}

	FileClose(fHandle);

//	wipeUserMem();

	// Let's go
	fioExit();

	SifInitRpc(0);
	SifExitRpc();
	FlushCache(0);
	FlushCache(2);

	argv[0] = pFileName;
//fixme
	ExecPS2( (void*) eh->entry, 0, 1, argv );

	return 1;
}
