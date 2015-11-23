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
# File: miscellaneous stuff that doesn't really
#		belong anywhere else.
#
*/

#ifndef _MISC_H
#define _MISC_H

#include <tamtypes.h>
#include <../lib/gsLib/gsLib.h>
#include <../lib/libps2time/libps2time.h>
#include <libcdvd.h>
#include <samba.h>

#define MYPS2_VERSION "1.2"

//
// main.c
//

extern u8	*iomanx_irx;
extern u8	*filexio_irx;
extern u8	*ps2dev9_irx;
extern u8	*ps2atad_irx;
extern u8	*ps2hdd_irx;
extern u8	*ps2fs_irx;
extern u8	*poweroff_irx;
extern u8	*cdvd_irx;
extern u8	*usb_mass_irx;
extern u8	*ps2ip_irx;
extern u8	*ps2smap_irx;
extern u8	*ps2ftpd_irx;
extern u8	*sjpcm_irx;
extern u8	*ps2ips_irx;
extern u8	*dns_irx;

extern int	size_iomanx_irx;
extern int	size_filexio_irx;
extern int	size_ps2dev9_irx;
extern int	size_ps2atad_irx;
extern int	size_ps2hdd_irx;
extern int	size_ps2fs_irx;
extern int	size_poweroff_irx;
extern int	size_cdvd_irx;
extern int	size_usb_mass_irx;
extern int	size_ps2ip_irx;
extern int	size_ps2smap_irx;
extern int	size_ps2ftpd_irx;
extern int	size_sjpcm_irx;
extern int	size_ps2ips_irx;
extern int	size_dns_irx;

#define RGB(r,g,b)		GS_SETREG_RGBAQ(r,g,b,0x80,0x00)
#define RGBA(r,g,b,a)	GS_SETREG_RGBAQ(r,g,b,a,0x00)

#define colorBlack		RGB( 0x00, 0x00, 0x00 )
#define colorRed		RGB( 0xFF, 0x00, 0x00 )
#define colorGreen		RGB( 0x00, 0xFF, 0x00 )
#define colorBlue		RGB( 0x00, 0x00, 0xFF )
#define colorWhite		RGB( 0xFF, 0xFF, 0xFF )
#define colorTex		RGB( 0x80, 0x80, 0x80 )

#define BCD2DEC(bcd)	(((((bcd)>>4) & 0x0F) * 10) + ((bcd) & 0x0F)) 
#define DEC2BCD(dec)	((((dec)/10)<<4)|((dec)%10))

#define MAKEPARAM(a, b)	((u32)(((u16)(a)) | ((u32)((u16)(b))) << 16))
#define LOWORD(l)		((u16)(l))
#define HIWORD(l)		((u16)(((u32)(l) >> 16) & 0xFFFF))

#define MAX_PATH		1024

#define isnum(c) ((c) >= '0' && (c) <= '9')

typedef enum {
	BOOT_HOST,
	BOOT_MC,
	BOOT_HDD,
	BOOT_USB,
	BOOT_CD
} bootDevice_t;

void loadModules( const char *path );
void CD_Init( void );
void CD_Exit( void );

void SetElfPath( const char *argv );
const char *GetElfPath( void );

void SetBootMode( const char *path );
int GetBootMode( void );
const char *BootMntPoint( void );

int IOPModulePresent( const char *lpModuleName );
void ResetIOP( void );

// HDD Stuff

#define PARTITION_NAME	"MYPS2"
#define HDD_MAX_MOUNT	8

enum
{
	HDD_NOT_AVAIL,			// HDD is not available
	HDD_AVAIL,				// HDD is available
	HDD_AVAIL_NO_PART		// HDD is available but MYPS2 partition							
							// could not be found
};

void HDD_Init( void );
int HDD_Available( void );
int HDD_GetFreeSpace( void );
int HDD_CreatePartition( char *partName, int partSize );
int HDD_MountPartition( const char *mountPoint, const char *partName );
void HDD_ShutDown( void );
int HDD_MountList( void );
int HDD_UnmountList( void );
int HDD_NumMounted( void );
const char *HDD_GetPartition( const char *lpMountPoint );

void MC_Init( void );
int MC_Available( int nPort );

void USB_Init( void );
int USB_Available( void );

void NET_Init( const char *path );
int NET_Available( void );
void FTP_Init( void );
int FTP_Available( void );

//
// bootscreen.c
//

void Bootscreen_Clear( void );
void Bootscreen_printf( const char *fmt, ... );
void Bootscreen_Init( void );
void Bootscreen_Shutdown( void );
int Bootscreen_GetX( void );
int Bootscreen_GetY( void );
void Bootscreen_SetX( int x );
void Bootscreen_SetY( int y );

#endif
