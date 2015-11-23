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
#		All "worker" threads have the same priority (100).
#		The scheduler thread rotates the ready queue 
#		whenever it is woken up by the alarm interrupt.
#
#		Problems:
#		 - Lots of consecutive calls to SifCallRpc
#		   (parsing a file with fgets for example) are
#		   extremely slow while it's no problem to read
#		   the whole file into memory and then parse it
#		   there.
#
#		 - Had an exeception crash once while decoding
#		   an MP3 and copying files in the file manager.
#		   Haven't really looked into that.
#
*/

#include <scheduler.h>

#define INT_VBLANK_START	0x02

// ?
extern void *_gp;

static threadStack_t threadStack[ SCHEDULER_MAX_THREADS ];

static volatile int	numThreads		= 0;
static volatile int	schedulerThread	= 0;
static int			scheduler_sema;

// this probably isn't a very good solution
// but the alarm stuff does not work properly
int vblankHandler(int cause)
{
	// set scheduler thread back to its original priority
	iChangeThreadPriority( schedulerThread, SCHEDULER_PRIO_MAIN );

	// force scheduler thread to be rescheduled
	iRotateThreadReadyQueue( SCHEDULER_PRIO_MAIN );

	return 0;
}

//
// Scheduler_Init - Initializes Thread Scheduler
//

int Scheduler_Init( void )
{
	ee_sema_t sema;

	sema.init_count	= 1;
	sema.max_count	= 1;
	sema.option		= 0;

	scheduler_sema	= CreateSema(&sema);

	if( scheduler_sema < 0 )
		return 0;

	schedulerThread = GetThreadId();

	// just use vsync interrupt for now
	AddIntcHandler(INT_VBLANK_START, vblankHandler, 0);
	EnableIntc(INT_VBLANK_START);

	return 1;
}

//
// Scheduler_Alarm - Callback alarm function 
//

void Scheduler_Alarm( s32 id, u16 time, void *arg )
{
	// set scheduler thread back to its original priority
	iChangeThreadPriority( schedulerThread, SCHEDULER_PRIO_MAIN );

	// force scheduler thread to be rescheduled
	iRotateThreadReadyQueue( SCHEDULER_PRIO_MAIN );
}

//
// Scheduler_Run - Rotates the thread ready queue
//

void Scheduler_Run( void )
{
//	s32 alarmID;

	while(1)
	{
		// install alarm
//		alarmID = SetAlarm( SCHEDULER_TIME_SLICE, Scheduler_Alarm, 0 );

		// this should give control to one of the worker threads
		ChangeThreadPriority( schedulerThread, SCHEDULER_PRIO_THREAD + 10 );

		// this should rotate the worker threads in the ready queue
		RotateThreadReadyQueue( SCHEDULER_PRIO_THREAD );

		// release the alarm
//		ReleaseAlarm(alarmID);
	}
}

//
// Scheduler_BeginThread - Creates a new thread
//

s32 Scheduler_BeginThread( void (*thread_func)(void*), void *args )
{
	int i;
	ee_thread_t thread;
	s32 thread_id;

	WaitSema( scheduler_sema );

	if( numThreads >= SCHEDULER_MAX_THREADS )
		return -1;

	memset( &thread, 0, sizeof(thread) );

	thread.func				= (void*) thread_func;
	thread.stack_size		= SCHEDULER_STACK_SIZE;
	thread.gp_reg			= &_gp;
	thread.initial_priority	= SCHEDULER_PRIO_THREAD;

	for( i = 0; i < SCHEDULER_MAX_THREADS; i++ )
	{
		if( !threadStack[i].used )
		{
			thread.stack = threadStack[i].stack;
			break;
		}
	}

	thread_id = CreateThread( &thread );

	if( thread_id < 0 ) {
		SignalSema( scheduler_sema );
		return thread_id;
	}

	threadStack[i].used		= 1;
	threadStack[i].thread	= thread_id;

	StartThread( thread_id, args );

	numThreads++;

	SignalSema( scheduler_sema );

	return thread_id;
}

//
// Scheduler_EndThread - A thread should call this before it ends
//

void Scheduler_EndThread( void )
{
	int i;
	s32 thread_id;

	WaitSema( scheduler_sema );

	thread_id = GetThreadId();

	for( i = 0; i < SCHEDULER_MAX_THREADS; i++ )
	{
		if( threadStack[i].used )
		{
			if( threadStack[i].thread == thread_id )
				threadStack[i].used = 0;
		}
	}

	numThreads--;

	SignalSema( scheduler_sema );
}

//
// Scheduler_YieldThread - Causes the calling thread to yield execution to
//						   another thread that is ready to run.
//

void Scheduler_YieldThread( void )
{
//	RotateThreadReadyQueue( SCHEDULER_PRIO_THREAD );
	ChangeThreadPriority( schedulerThread, SCHEDULER_PRIO_MAIN );

	RotateThreadReadyQueue( SCHEDULER_PRIO_MAIN );
}


