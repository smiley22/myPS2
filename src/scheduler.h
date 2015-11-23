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
# Date: 11-29-2005
#
# File: Thread Scheduling
#
*/

#ifndef _SCHEDULER_H
#define _SCHEDULER_H

#include <stdio.h>
#include <string.h>
#include <kernel.h>

#define SCHEDULER_MAX_THREADS	8
#define SCHEDULER_STACK_SIZE	1024 * 1024
#define SCHEDULER_TIME_SLICE	100

#define SCHEDULER_PRIO_THREAD	100
#define SCHEDULER_PRIO_MAIN		64

typedef struct {
	char			stack[SCHEDULER_STACK_SIZE]  __attribute__(   (  aligned( 16 )  )   );
	volatile int	used;
	volatile s32	thread;
} threadStack_t;

int Scheduler_Init( void );
void Scheduler_Alarm( s32 id, u16 time, void *arg );
void Scheduler_Run( void );
s32 Scheduler_BeginThread( void (*thread_func)(void*), void *args );
void Scheduler_EndThread( void );
void Scheduler_YieldThread( void );

#endif
