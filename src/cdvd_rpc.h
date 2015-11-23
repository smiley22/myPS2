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

#ifndef _CDVD_RPC_H
#define _CDVD_RPC_H

// include the common definitions
#include "cdvd.h"

#ifdef __cplusplus
extern "C" {
#endif


int CDVD_Init();
int CDVD_DiskReady(int mode);
int CDVD_FindFile(const char* fname, struct TocEntry* tocEntry);
void CDVD_Stop();
int CDVD_TrayReq(int mode);
int CDVD_DiskReady(int mode);
int CDVD_GetDir(const char* pathname, const char* extensions, enum CDVD_getMode getMode, struct TocEntry tocEntry[], unsigned int req_entries, char* new_pathname);
void CDVD_FlushCache();
unsigned int CDVD_GetSize();


#ifdef __cplusplus
}
#endif


#endif // _CDVD_H
