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
# Date: 12-22-2005
#
# File: EE RPC client for usb_mass.irx
#
*/

#ifndef _MASS_RPC_H
#define _MASS_RPC_H

typedef struct _fat_dir_record { // 140 bytes
	unsigned char attr;		//attributes (bits:5-Archive 4-Directory 3-Volume Label 2-System 1-Hidden 0-Read Only)
	unsigned char name[128];
	unsigned char date[4];	//D:M:Yl:Yh
	unsigned char time[3];  //H:M:S
	unsigned int  size;	//file size, 0 for directory
} fat_dir_record;


int usb_mass_bindRpc();
int usb_mass_getFirstDirentry(char* path, fat_dir_record* record);
int usb_mass_getNextDirentry(fat_dir_record* record);
int usb_mass_dumpSystemInfo();
int usb_mass_dumpDiskContent(unsigned int startSector, unsigned int sectorCount, char* fname);
int usb_mass_overwriteDiskContent(unsigned int startSector, unsigned int sectorCount, char* fname);

// tk: added (11-17-05)
int usb_mass_getConnectState( unsigned int slot );

#define USB_DIRECTORY	0x10
#define USB_VOLUME		0x08

#endif /* _MASS_RPC_H */
