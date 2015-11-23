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

#ifndef _GAMEPAD_H
#define _GAMEPAD_H

#include "libpad.h"

void GP_Init( void );
u32 GP_GetButtons( void );
void GP_SetPressMode( int n );
int GP_GetPressMode( void );
int waitPadReady( int port, int slot );
int initializePad( int port, int slot );

#endif
