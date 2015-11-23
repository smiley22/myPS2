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
# Date: 10-6-2005
#
# File: module loading, other
#		initialization functions etc.
#
*/

#include <tamtypes.h>
#include <sifrpc.h>
#include <loadfile.h>
#include <libhdd.h>
#include <libmc.h>
#include <sys/fcntl.h>	// FIO_MT_RDWR
#include <sbv_patches.h>
#include <mass_rpc.h>
#include <kernel.h>
#include <ps2ip.h>
#include <smod.h>

#include <gamepad.h>
#include <misc.h>
#include <imgscale.h>
#include <sysconf.h>
#include <mp3.h>
#include <scheduler.h>
#include <net.h>
#include <file.h>
#include <cdvd.h>
#include <GUI.h>
#include <../lib/dmaKit/dmaKit.h>
#include <iopcontrol.h>
#include <iopheap.h>

int main( int argc, char *argv[] )
{
	int		nRet, nTimeOffset;
	int		bSafeMode		= 0;

	char	szBootPart[MAX_PATH];
	char	szBootPath[MAX_PATH];
	char	szExecPath[MAX_PATH];

	// initialize dmaKit
	dmaKit_init( D_CTRL_RELE_ON, D_CTRL_MFD_OFF, D_CTRL_STS_UNSPEC,
				 D_CTRL_STD_OFF, D_CTRL_RCYC_8 );

	dmaKit_chan_init(DMA_CHANNEL_GIF);

	SetElfPath( argv[0] );
	SetBootMode( argv[0] );

	// setup loading screen
	Bootscreen_Init();
	Bootscreen_printf("SetElfPath: %s\n", GetElfPath() );

	// booting from HDD
	if( GetBootMode() == BOOT_HDD )
	{
		if( !SC_LoadConfig(0) )
		{
			Bootscreen_printf("Failed loading CONFIG.DAT\n");
			Bootscreen_printf("CONFIG.DAT must be present when booting from HDD!\n");
			SleepThread();
		}

		if( !SC_GetValueForKey_Str( "hdd_boot_part", szBootPart ) )
		{
			Bootscreen_printf("Couldn't find hdd_boot_part entry in CONFIG.DAT\n");
			SleepThread();
		}

		if( !SC_GetValueForKey_Str( "hdd_boot_path", szBootPath ) )
		{
			Bootscreen_printf("Couldn't find hdd_boot_path entry in CONFIG.DAT\n");
			SleepThread();
		}
	}

	loadModules( argv[0] );

	GP_Init();
	MC_Init();
	HDD_Init();
	USB_Init();

	if( GetBootMode() == BOOT_HDD )
	{
		// if boot partition is +MYPS2 it has already been mounted in HDD_Init
		if( (HDD_Available() == HDD_AVAIL) && (!strcmp(szBootPart, "hdd0:+MYPS2")) )
		{
			snprintf( szExecPath, sizeof(szExecPath), "pfs0:%s", szBootPath );
			SetElfPath( szExecPath );
		}
		else
		{
			// mount boot partition to pfs1
			nRet = fileXioMount( "pfs1:", szBootPart, FIO_MT_RDWR );
			if( nRet < 0 ) {
				Bootscreen_printf("Could not mount partition %s (errno %i)\n", szBootPart, nRet );
				SleepThread();
			}

			snprintf( szExecPath, sizeof(szExecPath), "pfs1:%s", szBootPath );
			SetElfPath( szExecPath );
		}

		Bootscreen_printf("SetElfPath: %s\n", GetElfPath() );
	}

	// boot in safe mode?
	if( ( GP_GetButtons() & (PAD_R1 | PAD_R2 | PAD_L1 | PAD_L2) ) == (PAD_R1 | PAD_R2 | PAD_L1 | PAD_L2) )
		bSafeMode = 1;

	// Wait for Usb device to be ready before loading config file from usb
	if( GetBootMode() == BOOT_USB )
	{
		Bootscreen_printf("Waiting for Usb...");

		//Do nothing
		while(USB_Available() != 1){}

		Bootscreen_printf("^1OK\n");
	}

	// load system config
	SC_LoadConfig(bSafeMode);

	// init ps2time library
	nTimeOffset = SC_GetValueForKey_Int( "time_timezone", NULL );
	if( SC_GetValueForKey_Int( "time_dst", NULL ) )
		nTimeOffset += 60;

	ps2time_init();
	ps2time_setTimezone( nTimeOffset );

	// mount partitions in system config mount list
	Bootscreen_printf("\tMounting partitions in mountlist: ");
	nRet = HDD_MountList();
	Bootscreen_printf("%i Errors\n", nRet);

	// initialize network
	NET_Init( argv[0] );
	FTP_Init();

	// init libsmb
	SMB_Init();

	// init the user interface
	GUI_Init();

	// init the thread scheduler
	Scheduler_Init();

	// create GUI thread
	Scheduler_BeginThread( GUI_Run, NULL );

	Scheduler_Run();

	HDD_ShutDown();
	SC_Clean();
	return 0;
}

//
// loadModules - Loads all IRX modules.
//
// CDVD module is not loaded at start up because it won't
// run if there is no disk in the drive. It is loaded later
// when it is needed. PS2IP and SMAP are loaded later after
// the config file has been loaded.
//

void loadModules( const char *path )
{
	int			ret, irx_ret;
	const char	hddarg[]	= "-o" "\0" "10" "\0" "-n" "\0" "20";
	const char	pfsarg[]	= "-m" "\0" "10" "\0" "-o" "\0" "10" "\0" "-n" "\0" "40";

	SifInitRpc(0);

	CD_Exit();

#ifndef _DEVELOPER
	ResetIOP();
#endif

	hddPreparePoweroff();

	// install sbv patch fix
	Bootscreen_printf("Installing SBV Patch...\n");

	sbv_patch_enable_lmb();
	sbv_patch_disable_prefix_check();

	Bootscreen_printf("Loading basic IRX modules...\n");

	// SIO stuff
	ret = SifLoadModule("rom0:SIO2MAN", 0, NULL);
	if( ret < 0 ) {
		Bootscreen_printf("\t^2SifLoadModule^0: failed to load SIO2MAN (%d)", ret );
		SleepThread();
	}

	// Memory Card stuff
	ret = SifLoadModule("rom0:MCMAN", 0, NULL);
	if( ret < 0 ) {
#ifdef _DEBUG
		printf("sifLoadModule mcman failed: %d\n", ret);
#endif
		Bootscreen_printf("\t^2SifLoadModule^0: failed to load MCMAN (%d)", ret );
		SleepThread();
	}

	// Memory Card stuff
	ret = SifLoadModule("rom0:MCSERV", 0, NULL);
	if( ret < 0 ) {
#ifdef _DEBUG
		printf("sifLoadModule mcserv failed: %d\n", ret);
#endif
		Bootscreen_printf("\t^2SifLoadModule^0: failed to load MCSERV (%d)", ret );
		SleepThread();
	}

	// Gamepad driver
	ret = SifLoadModule("rom0:PADMAN", 0, NULL);
	if( ret < 0 ) {
#ifdef _DEBUG_GP
		printf("sifLoadModule pad failed: %d\n", ret);
#endif
		Bootscreen_printf("\t^2SifLoadModule^0: failed to load PADMAN (%d)", ret );
		SleepThread();
	}

	// IOMANX driver (embedded in ELF)
	ret = SifExecModuleBuffer( &iomanx_irx, size_iomanx_irx, 0, NULL, &irx_ret );
	if( ret < 0 ) {
#ifdef _DEBUG
		printf("SifExecModuleBuffer iomanx failed: %d\n", ret);
#endif
		Bootscreen_printf("\t^2SifExecModuleBuffer^0: failed to load IOMANX (%d)", ret );
		SleepThread();
	}

	// fileXio driver (embedded in ELF)
	ret = SifExecModuleBuffer( &filexio_irx, size_filexio_irx, 0, NULL, &irx_ret );
	if( ret < 0 ) {
#ifdef _DEBUG
		printf("SifExecModuleBuffer filexio failed: %d\n", ret);
#endif
		Bootscreen_printf("\t^2SifExecModuleBuffer^0: failed to load FILEXIO (%d)", ret );
		SleepThread();
	}

#ifndef _DEVELOPER
//	if( bootmode != BOOT_HOST )
	{
		// DEV9 driver (embedded in ELF)
		ret = SifExecModuleBuffer( &ps2dev9_irx, size_ps2dev9_irx, 0, NULL, &irx_ret );
		if( ret < 0 ) {
#ifdef _DEBUG
			printf("SifExecModuleBuffer dev9 failed: %d\n", ret);
#endif
			Bootscreen_printf("\t^2SifExecModuleBuffer^0: failed to load PS2DEV9 (%d)", ret );
			SleepThread();
		}
	}
#endif

	// ATAD driver (embedded in ELF)
	ret = SifExecModuleBuffer( &ps2atad_irx, size_ps2atad_irx, 0, NULL, &irx_ret );
	if( ret < 0 ) {
#ifdef _DEBUG
		printf("SifExecModuleBuffer ps2atad failed: %d\n", ret);
#endif
		// fails if no hdd is deteced
		Bootscreen_printf("\t^2SifExecModuleBuffer^0: failed to load PS2ATAD (%d)\n", ret );
	}
	else
	{
		// HDD driver (embedded in ELF)
		ret = SifExecModuleBuffer( &ps2hdd_irx, size_ps2hdd_irx, sizeof(hddarg), hddarg, &ret );
		if( ret < 0 ) {
#ifdef _DEBUG
			printf("SifExecModuleBuffer ps2hdd failed: %d\n", ret);
#endif
			Bootscreen_printf("\t^2SifExecModuleBuffer^0: failed to load PS2HDD (%d)", ret );
			SleepThread();
		}

		// PS2 file system driver (embedded in ELF)
		ret = SifExecModuleBuffer( &ps2fs_irx, size_ps2fs_irx, sizeof(pfsarg), pfsarg, &ret );
		if( ret < 0 ) {
	#ifdef _DEBUG
			printf("SifExecModuleBuffer ps2fs failed: %d\n", ret);
	#endif
			Bootscreen_printf("\t^2SifExecModuleBuffer^0: failed to load PS2FS (%d)", ret );
			SleepThread();
		}
	}

	// Poweroff module (embedded in ELF)
	ret = SifExecModuleBuffer( &poweroff_irx, size_poweroff_irx, 0, NULL, &irx_ret );
	if( ret < 0 ) {
#ifdef _DEBUG
		printf("SifExecModuleBuffer poweroff failed: %d\n", ret);
#endif
		Bootscreen_printf("\t^2SifExecModuleBuffer^0: failed to load POWEROFF (%d)", ret );
		SleepThread();
	}

	// LIBSD
	ret = SifLoadModule( "rom0:LIBSD", 0, NULL );
	if( ret < 0 ) {
#ifdef _DEBUG
		printf("SifLoadModule LIBSD failed: %d\n", ret);
#endif
		Bootscreen_printf("\t^2SifLoadModule^0: failed to load LIBSD (%d)", ret );
		SleepThread();
	}

	// SjPCM module (embedded in ELF)
	ret = SifExecModuleBuffer( &sjpcm_irx, size_sjpcm_irx, 0, NULL, &irx_ret );
	if( ret < 0 ) {
#ifdef _DEBUG
		printf("SifExexModuleBuffer sjpcm failed : %d\n", ret);
#endif
		Bootscreen_printf("\t^2SifExexModuleBuffer^0: failed to load SJPCM (%d)", ret );
		SleepThread();
	}
}

//
// CD_Init - Loads the cdvd.irx module and initializes CDVD.
//

static int CDInit = 0;

void CD_Init( void )
{
	int ret, irx_ret;

	// don't bother if already initialized
	if( CDInit )
		return;

	// load CDVD module (embedded in ELF)
	ret = SifExecModuleBuffer( &cdvd_irx, size_cdvd_irx, 0, NULL, &irx_ret );
	if(ret < 0) {
#ifdef _DEBUG
		printf("SifExecModuleBuffer cdvd failed: %d\n", ret);
#endif
		SleepThread();
	}

	CDVD_Init();

	CDInit = 1;
#ifdef _DEBUG
	printf("CD_Init : CDVD initialized.\n");
#endif
}

void CD_Exit( void )
{
	CDInit = 0;
}

static char strElfPath[256];

void SetElfPath( const char *argv )
{
	char *ptr;

	strncpy( strElfPath, argv, 255 );
	strElfPath[255] = 0;

	if( !(ptr = strchr( strElfPath, ':' )) )
		return;

	ptr++; *ptr = '/';
	ptr++; *ptr = '\0';

	ptr = strchr( argv, ':' );
	ptr++;

	if( *ptr == '/' )
		ptr++;

	strncat( strElfPath, ptr, sizeof(strElfPath) );

	ptr = strrchr( strElfPath, '/' );
	if( ptr == NULL ) {
		ptr = strrchr( strElfPath, '\\' );
		if( ptr == NULL ) {
			ptr = strrchr( strElfPath, ':' );
			if( ptr == NULL ) {
				printf("ERROR\n");
			}
		}
	}

	ptr++;
	*ptr = '\0';

	if( !strncmp( strElfPath, "cdrom0", 6 ) )
	{
		char strTemp[256];

		ptr = strchr( strElfPath, ':' );

		snprintf( strTemp, sizeof(strTemp), "cdfs%s", ptr );
		strcpy( strElfPath, strTemp );

	}
}

const char *GetElfPath( void )
{
	return strElfPath;
}

static int nBootMode;

void SetBootMode( const char *path )
{
	int i;

	// figure out boot mode
	i = strcspn( path, ":" );

	if( !strncmp( path, "host", i ) ) {
		if( IOPModulePresent( "fakehost" ) )
			nBootMode = BOOT_HDD;
		else
			nBootMode = BOOT_HOST;
	}
	else if( !strncmp( path, "mc", i ) ) {
		nBootMode = BOOT_MC;
	}
	else if( !strncmp( path, "mass", i ) ) {
		nBootMode = BOOT_USB;
	}
	else if( !strncmp( path, "cdrom0", i ) || !strncmp( path, "cdfs", i ) ) {
		nBootMode = BOOT_CD;
	}
}

int GetBootMode( void )
{
	return nBootMode;
}

const char *BootMntPoint( void )
{
	char szBootPart[MAX_PATH];

	SC_GetValueForKey_Str( "hdd_boot_part", szBootPart );

	if( !strcmp( szBootPart, "hdd0:+MYPS2" ) )
		return "pfs0";

	return "pfs1";
}

//
// IOPModulePresent - Returns true if IOP module is loaded
//

int IOPModulePresent( const char *lpModuleName )
{
	smod_mod_info_t	mod_t;

	return smod_get_mod_by_name( lpModuleName, &mod_t );
}

void ResetIOP( void )
{
	Bootscreen_printf("Resetting IOP...\n");

	SifIopReset("rom0:UDNL rom0:EELOADCNF",0);
	while(!SifIopSync());

	fioExit();
	SifExitIopHeap();
	SifLoadFileExit();

	Bootscreen_printf("\tcalling SifExitRpc()...\n");
	SifExitRpc();

	Bootscreen_printf("\tcalling SifExitCmd()...\n");
	SifExitCmd();

	Bootscreen_printf("\tcalling SifInitRpc(0)...\n");
	SifInitRpc(0);

	FlushCache(0);
	FlushCache(2);
}

//
// --------------------- HDD initialization and helper functions ---------------------
//

static int	bHddAvail		= HDD_NOT_AVAIL;
static int	nHddNumMounted	= 0;
static char	lpPartNames[HDD_MAX_MOUNT][MAX_PATH + 1];

//
// HDD_Init - initializes HDD and attempts to mount partitions
//

void HDD_Init( void )
{
	int				nRet;
	iox_stat_t		info;
	char			openString[256];

	Bootscreen_printf("Checking for HDD presence: ");

	nRet = hddCheckPresent();
	if( nRet == -1 )
	{
#ifdef _DEBUG
		printf("Could not find HDD.\n");
#endif
		Bootscreen_printf("^2Not found\n");
		bHddAvail = HDD_NOT_AVAIL;
		return;
	}
	else {
		Bootscreen_printf("^1OK\n");
	}

	Bootscreen_printf("Checking HDD status: ");

	nRet = hddCheckFormatted();
	if( nRet == -1 )
	{
#ifdef _DEBUG
		printf("HDD is not formatted.\n");
#endif
		Bootscreen_printf("^2Not formatted\n");
		bHddAvail = HDD_NOT_AVAIL;
		return;
	}
	else {
		Bootscreen_printf("^1OK\n");
	}

	strcpy( openString, "hdd0:+" );
	strcat( openString, PARTITION_NAME );

	// check for partition
	nRet = fileXioGetStat( openString, &info );
	if( nRet != 0 ) {
		bHddAvail = HDD_AVAIL_NO_PART;
	}
	else
	{
		// attempt to mount myPS2 partition
		Bootscreen_printf("\tAttempting to mount +MYPS2 partition: ");

		nRet = HDD_MountPartition( "pfs0:", PARTITION_NAME );

		if( !nRet ) {
#ifdef _DEBUG
			printf("Could not mount partition %s.\n", openString);
#endif
			// there must be some problem with the partition
			Bootscreen_printf("^2FAILED\n");

			bHddAvail = HDD_NOT_AVAIL;
		}
		else {
			// everything's okay
			Bootscreen_printf("^1OK\n");

			bHddAvail = HDD_AVAIL;
		}
	}
}

//
// HDD_Available - Returns availability status of HDD.
//				   Possible return values are :
//					- HDD_NOT_AVAIL
//					- HDD_AVAIL
//					- HDD_AVAIL_NO_PART
//

int HDD_Available( void )
{
	return bHddAvail;
}

//
// HDD_GetFreeSpace - Returns amount of free HDD space in megabytes
//

int HDD_GetFreeSpace( void )
{
	t_hddInfo hddInfo;

	if( HDD_Available() == HDD_NOT_AVAIL )
		return 0;

	hddGetInfo( &hddInfo );

	return hddInfo.hddFree;
}

//
// HDD_CreatePartition - Attempts to create a new partition.
//						 Returns 1 on success, otherwise 0.
//
//						 Ensures partition size is a multiple
//						 of 128 MB.
//

int HDD_CreatePartition( char *partName, int partSize )
{
	int		nRet;
	int		nMul;

	if( HDD_Available() == HDD_NOT_AVAIL )
		return 0;

	// make sure partSize is actually a multiple of 128
	// because otherwise hddMakeFilesystem will fail.
	nMul		= partSize / 128;
	partSize	= nMul * 128;

#ifdef _DEBUG
	printf("HDD_CreatePartition : Adjusted partSize to %i MB\n", partSize );
#endif

	nRet = hddMakeFilesystem( partSize, partName, FS_GROUP_COMMON );

	if( nRet < 0 )
		return 0;

	return 1;
}

//
// HDD_MountPartition - Attempts to mount a partition.
//						Returns 1 on success, otherwise 0.
//

int HDD_MountPartition( const char *mountPoint, const char *partName )
{
	int		nRet;
	char	openString[256];

	strcpy( openString, "hdd0:+" );
	strcat( openString, partName );

	nRet = fileXioMount( mountPoint, openString, FIO_MT_RDWR );

	// could not mount partition
	if( nRet < 0 )
		return 0;

	// okay, partition is now available
	bHddAvail = HDD_AVAIL;

	return 1;
}

//
// HDD_ShutDown - Unmounts myPS2 HDD partition
//

void HDD_ShutDown( void )
{
	fileXioUmount("pfs0:");
	bHddAvail = HDD_NOT_AVAIL;
}

//
// HDD_MountList - Attempts to mount all partitions in mountlist
//

int HDD_MountList( void )
{
	char		strMntList[1024];
	char		pfs[32];
	char		*pToken;
	int			nRet, nError = 0;
	iox_stat_t	info;

	if( HDD_Available() == HDD_NOT_AVAIL )
		return 0;

	HDD_UnmountList();

	SC_GetValueForKey_Str( "part_mount_list", strMntList );

	if( !strMntList[0] )
		return 0;

	pToken = strtok( strMntList, "," );
	while( pToken ) {

		// check if partition exists
		nRet = fileXioGetStat( pToken, &info );
		if( nRet != 0 ) {
			nError++;
			pToken = strtok( NULL, "," );
			continue;
		}

		// max number of user mounted partitions
		if( nHddNumMounted >= HDD_MAX_MOUNT )
			break;

		// pfs0 and pfs1 are reserved
		snprintf( pfs, sizeof(pfs), "pfs%i:", nHddNumMounted + 2 );

		nRet = fileXioMount( pfs, pToken, FIO_MT_RDWR );

		// could not mount partition
		if( nRet < 0 ) {
			nError++;
			pToken = strtok( NULL, "," );
			continue;
		}

		strncpy( lpPartNames[ nHddNumMounted ], pToken, MAX_PATH );
		lpPartNames[ nHddNumMounted ][MAX_PATH] = 0;

		nHddNumMounted++;

		pToken = strtok( NULL, "," );
	}

	return nError;
}

//
// HDD_UnmountList - Unmounts all mounted partitions apart from +MYPS2
//					 which is never unmounted.
//

int HDD_UnmountList( void )
{
	int		i;
	char	pfs[32];

	if( HDD_Available() == HDD_NOT_AVAIL )
		return 0;

	for( i = 0; i < nHddNumMounted; i++ ) {
		// pfs0 and pfs1 can not be unmounted
		snprintf( pfs, sizeof(pfs), "pfs%i:", i + 2 );
		fileXioUmount( pfs );
	}

	nHddNumMounted = 0;
	return 1;
}

//
// HDD_NumMounted - Returns number of mounted partitions, +MYPS2 not included
//

int HDD_NumMounted( void )
{
	return nHddNumMounted;
}

//
// HDD_GetPartName - Finds Partition Name from Mountpoint
//

const char *HDD_GetPartition( const char *lpMountPoint )
{
	int	i;
	char szStr[MAX_PATH + 1], *pStr;

	if(!strcmp( lpMountPoint, "pfs0:" ) || !strcmp( lpMountPoint, "pfs0:/"))
		return "hdd0:+MYPS2";

	if(!strcmp( lpMountPoint, "pfs1:" ) || !strcmp( lpMountPoint, "pfs1:/"))
		return SC_GetValueForKey_Str( "hdd_boot_part", NULL );

	strncpy( szStr, lpMountPoint, MAX_PATH );
	szStr[MAX_PATH] = 0;

	pStr = szStr;
	while( !isdigit(*pStr) )
		pStr++;

	i = atoi(pStr) - 2;

	if( i < 0 || i >= HDD_MAX_MOUNT )
		return NULL;

	return lpPartNames[i];
}

//
// --------------------- MC initialization and helper functions ---------------------
//

int nMCInit = 0;

//
// MC_Init - initializes Memory Card
//

void MC_Init( void )
{
	int nRet;

	Bootscreen_printf("Initializing Memory Card\n");

	nRet = mcInit(MC_TYPE_MC);

	if( nRet < 0 ) {
#ifdef _DEBUG
		printf("MC_Init : failed to initialize memcard server.\n");
#endif
		Bootscreen_printf("^2Warning: Failed to initialize memcard server (%d)\n", nRet);
		nMCInit = 0;
	}

	nMCInit = 1;
}

//
// MC_Available - Returns 1 if MC in port is connected
//

int MC_Available( int nPort )
{
	int mcType, mcFree, mcFormat;
	int nRet;

	if( !nMCInit )
		return 0;

	if( nPort < 0 || nPort > 1 )
		return 0;

	mcGetInfo( nPort, 0, &mcType, &mcFree, &mcFormat );
	mcSync( 0, NULL, &nRet );

	// MC could not be found or is not properly formatted
	if( nRet <= -10 || nRet == -2 )
		return 0;

	return 1;
}

//
// --------------------- USB initialization and helper functions ---------------------
//

static int nUSBInit		= 0;
static int nUSBAvail	= 0;

const char *pUSBDPath[] =
{
	"mc0:/USBD.IRX",
	"mc0:/SYS-CONF/USBD.IRX",
	"mc0:/BOOT/USBD.IRX",
	"mc0:/BOOT/SMS/USBD.IRX",
	"mc0:/BOOT/PS2MP3/USBD.IRX",
	"mc0:/SMS/USBD.IRX",
	"mc0:/PS2MP3/USBD.IRX"
};

static void _usb_handler_connect ( void* apPkt, void* apArg )
{
	nUSBAvail = 1;
}

static void _usb_handler_disconnect( void* apPkt, void* apArg )
{
	nUSBAvail = 0;
}

//
// USB_Init - Loads modules and initializes USB
//

void USB_Init( void )
{
	int nRet, irx_ret, i;
	char strPath[256];
	FHANDLE fHandle;
	int nSize, numElems;

	Bootscreen_printf("Looking for USBD module...\n");

	numElems = sizeof(pUSBDPath) / sizeof(const char*);

	// look for file in boot directory first
	strcpy( strPath, GetElfPath() );
	strcat( strPath, "USBD.IRX" );

	fHandle = FileOpen( strPath, O_RDONLY );
	nRet	= -1;

	if( fHandle.fh >= 0 )
	{
		FileClose(fHandle);
		Bootscreen_printf("\tLoading USBD.IRX from %s: ", strPath );

		nRet = SifLoadModule( strPath, 0, NULL );
		if( nRet < 0 )
			Bootscreen_printf("^2FAILED\n");
	}

	if( nRet < 0 )
	{
		// attempt to load from one of the default locations
		for( i = 0; i < numElems; i++ )
		{
			fHandle = FileOpen( pUSBDPath[i], O_RDONLY );
			if( fHandle.fh < 0 )
				continue;

			nSize = FileSeek( fHandle, 0, SEEK_END );
			FileClose(fHandle);

			if( !nSize )
				continue;

			// found one, try to load it
			Bootscreen_printf("\tLoading USBD.IRX found at %s: ", pUSBDPath[i] );

			nRet = SifLoadModule( pUSBDPath[i], 0, NULL );
			if( nRet < 0 ) {
#ifdef _DEBUG
				printf("USB_Init: SifLoadModule %s failed: %d\n", pUSBDPath[i], nRet);
#endif
				Bootscreen_printf("^2FAILED\n");
				continue;
			}
			else {
				Bootscreen_printf("^1OK\n");
			}

			break;
		}

		// couldn't find the USBD.IRX module anywhere
		if( i == numElems )
			return;
	}

	// load USB mass module (embedded in ELF)
	Bootscreen_printf("Loading USB mass storage module: ");

	nRet = SifExecModuleBuffer( &usb_mass_irx, size_usb_mass_irx, 0, NULL, &irx_ret );
	if( nRet < 0 ) {
#ifdef _DEBUG
		printf("SifExecModuleBuffer usb_mass failed: %d\n", nRet);
#endif
		Bootscreen_printf("^2FAILED\n");
		return;
	}
	else {
		Bootscreen_printf("^1OK\n");
	}

	// init USB RPC stuff
	nRet = usb_mass_bindRpc();

	if( nRet < 0 ) {
#ifdef _DEBUG
		printf("USB_Init: usb_mass_bindRpc failed : %d\n", nRet);
#endif
		Bootscreen_printf("^2Warning^0: Failed to bind usb_mass RPC server\n");
		return;
	}
	else {
		Bootscreen_printf("usb_mass_bindRPC ^1OK^0\n");
	}

	nUSBInit = 1;

	DI();
		SifAddCmdHandler( 0x0012, _usb_handler_connect, 0 );
		SifAddCmdHandler( 0x0013, _usb_handler_disconnect, 0 );
	EI();

}

//
// USB_Available - Returns 1 if USB device in either slot
//				   is connected,
//

int USB_Available( void )
{
	return nUSBAvail;
}

//
// --------------------- Net initialization and helper functions ---------------------
//

int nNetInit = 0;
int nFTPInit = 0;

//
// NET_Init - Attempts to load PS2IP and SMAP modules
//

void NET_Init( const char *path )
{
	int ret, i, irx_ret;
#ifndef _DEVELOPER
	char params[256];
#endif
	char string[256];

	// Config.dat should have been loaded by now
	if( !SC_GetValueForKey_Int( "net_enable", NULL ) )
		return;

	Bootscreen_printf("Initializing Network modules\n");

#ifndef _DEVELOPER
	Bootscreen_printf("\tLoading PS2IP module: ");

	ret = SifExecModuleBuffer( &ps2ip_irx, size_ps2ip_irx, 0, NULL, &irx_ret );
	if( ret < 0 ) {
		Bootscreen_printf("^2FAILED\n");
		return;
	}
	else {
		Bootscreen_printf("^1OK\n");
	}

	// prepare smap parameters
	memset( params, 0, sizeof(params) );
	i = 0;

	SC_GetValueForKey_Str( "net_ip", string );
	strncpy( &params[i], string, 15 );
	i += strlen(string) + 1;

	SC_GetValueForKey_Str( "net_netmask", string );
	strncpy( &params[i], string, 15 );
	i += strlen(string) + 1;

	SC_GetValueForKey_Str( "net_gateway", string );
	strncpy( &params[i], string, 15 );
	i += strlen( string ) + 1;

	// load SMAP module
	Bootscreen_printf("\tLoading SMAP module: ");

	ret = SifExecModuleBuffer( &ps2smap_irx, size_ps2smap_irx, i, &params[0], &irx_ret );
	if( ret < 0 ) {
		Bootscreen_printf("^2FAILED\n");
		return;
	}
	else {
		Bootscreen_printf("^1OK\n");
	}
#endif

	// setup DNS server address
	Bootscreen_printf("\tInitializing DNS: ");

	SC_GetValueForKey_Str( "net_dns", string );
	ret = dnsInit( string );
	if( ret == 0 ) {
#ifdef _DEBUG
		printf("NET_Init: failed to initialize DNS!\n");
#endif
		Bootscreen_printf("^2FAILED\n");
	}
	else {
		Bootscreen_printf("^1OK\n");
	}

	// load ps2ips.irx module (rpc server for ps2ip)
	Bootscreen_printf("\tLoading PS2IPS module: ");

	ret = SifExecModuleBuffer( &ps2ips_irx, size_ps2ips_irx, 0, NULL, &irx_ret );
	if( ret < 0 ) {
#ifdef _DEBUG
		printf("SifExecModuleBuffer ps2ips failed: %d\n", ret);
#endif
		Bootscreen_printf("^2FAILED\n");
		return;
	}
	else {
		Bootscreen_printf("^1OK\n");
	}

	// init ps2ips rpc server
	Bootscreen_printf("\tInitializing PS2IP RPC server: ");

	if( ps2ip_init() < 0 ) {
#ifdef _DEBUG
		printf("NET_Init: ps2ip_init failed!\n");
#endif
		Bootscreen_printf("^2FAILED\n");
		return;
	}
	else {
		Bootscreen_printf("^1OK\n");
	}

	nNetInit = 1;

	Bootscreen_printf("\tNetwork set up with :\n");

	Bootscreen_printf("\t\tIP Address :\t");
	i = Bootscreen_GetX();
	Bootscreen_printf("%s\n", SC_GetValueForKey_Str( "net_ip", NULL ) );

	Bootscreen_printf("\t\tGW Address :\t" );
	Bootscreen_SetX(i);
	Bootscreen_printf("%s\n", SC_GetValueForKey_Str( "net_gateway", NULL ) );

	Bootscreen_printf("\t\tSubnetmask :\t" );
	Bootscreen_SetX(i);
	Bootscreen_printf("%s\n", SC_GetValueForKey_Str( "net_netmask", NULL ) );

	Bootscreen_printf("\t\tDNS Address :\t" );
	Bootscreen_SetX(i);
	Bootscreen_printf("%s\n", SC_GetValueForKey_Str( "net_dns", NULL ) );

	Bootscreen_printf("\n");
}

//
// NET_Available - Returns 1 if network was successfully initialized
//

int NET_Available( void )
{
	return nNetInit;
}

//
// FTP_Init - Attempts to load PS2FTPD module
//

void FTP_Init( void )
{
	int ret, irx_ret, i;
	char param[128];
	char list[256];

	if( !NET_Available() )
		return;

	if( !SC_GetValueForKey_Int( "ftp_enable", NULL ) )
		return;

	i = 0;
	memset( list, 0, sizeof(list) );

	strcpy( &list[i], "-port" );
	i += strlen("-port") + 1;

	SC_GetValueForKey_Str( "ftp_port", param );
	strcpy( &list[i], param );
	i += strlen(param) + 1;

	if( SC_GetValueForKey_Int( "ftp_anonymous", NULL ) ) {
		strcpy( &list[i], "-anonymous" );
		i += strlen("-anonymous") + 1;
	}
	else {
		strcpy( &list[i], "-user" );
		i += strlen("-user") + 1;

		SC_GetValueForKey_Str( "ftp_login", param );
		strcpy( &list[i], param );
		i += strlen(param) + 1;

		strcpy( &list[i], "-pass" );
		i += strlen("-pass") + 1;

		SC_GetValueForKey_Str( "ftp_password", param );
		strcpy( &list[i], param );
		i += strlen(param) + 1;
	}

	// FIXME : fix "Null Buffer Warning" in ps2ftpd.irx
	Bootscreen_printf("Loading PS2FTPD module: ");

	ret = SifExecModuleBuffer( &ps2ftpd_irx, size_ps2ftpd_irx, i, &list[0], &irx_ret );
	if( ret < 0 )
	{
#ifdef _DEBUG
		printf("SifExexModuleBuffer ps2ftpd failed: %d\n", ret);
#endif
		Bootscreen_printf("^2FAILED\n");
		return;
	}
	else {
		Bootscreen_printf("^1OK\n");
		Bootscreen_printf("FTP daemon initialized on port %i\n", SC_GetValueForKey_Int( "ftp_port", NULL ) );
	}

	nFTPInit = 1;
}

//
// FTP_Available - Returns 1 if ftp daemon was successfully initialized
//

int FTP_Available( void )
{
	return nFTPInit;
}
