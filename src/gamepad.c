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
# Date: 10-15-2005
#
# File: Gamepad Code.
#		Based on ps2dev pad example.
*/

#include <tamtypes.h>
#include <gamepad.h>
#include <misc.h>
#include <kernel.h>
#include <stdio.h>


// pad_dma_buf is provided by the user, one buf for each pad
// contains the pad's current state
static char		padBuf[256] __attribute__((aligned(64)));
static char		actAlign[6];
static int		actuators;

static int		GP_port;
static int		GP_slot;
static struct	padButtonStatus GP_buttons;
static u32		GP_paddata;

static int		GP_PressMode;

//
// GP_Init - Initializes the Gamepad Stuff
//

void GP_Init( void )
{
	int ret;

	Bootscreen_printf("Initializing Gamepad\n");

	padInit(0);

	GP_port = 0; // 0 -> Connector 1, 1 -> Connector 2
	GP_slot = 0; // Always zero if not using multitap

	if( (ret = padPortOpen(GP_port, GP_slot, padBuf)) == 0 ) {
#ifdef _DEBUG_GP
		printf("padOpenPort failed: %d\n", ret);
#endif
		Bootscreen_printf("\tpadOpenPort failed (%d)\n", ret );
		Bootscreen_printf("^2Program execution halted\n");
		SleepThread();
	}
    
	if( !initializePad(GP_port, GP_slot) ) {
#ifdef _DEBUG_GP
		printf("pad initalization failed!\n");
#endif
		Bootscreen_printf("\tinitializePad failed\n");
		Bootscreen_printf("^2Program execution halted\n");
		SleepThread();
	}
}

//
// GP_GetButtons - Retrieves current state of gamepad buttons.
//

u32 GP_GetButtons( void )
{
	int ret;
	
	ret = padGetState( GP_port, GP_slot );
	
	while( (ret != PAD_STATE_STABLE) && (ret != PAD_STATE_FINDCTP1) )
	{
		if(ret == PAD_STATE_DISCONN) {
#ifdef _DEBUG_GP
			printf("Pad(%d, %d) is disconnected\n", GP_port, GP_slot );
#endif
		}
		ret = padGetState( GP_port, GP_slot );
	}

	ret = padRead( GP_port, GP_slot, &GP_buttons ); // port, slot, buttons
            
	if( ret != 0 )
		GP_paddata = 0xffff ^ GP_buttons.btns;

	return GP_paddata;
}

//
// GP_SetPressMode - Enables or disables Press Mode
//

void GP_SetPressMode( int n )
{
	GP_PressMode = n;
}

int GP_GetPressMode( void )
{
	return GP_PressMode;
}

//
// waitPadReady
//

int waitPadReady(int port, int slot)
{
	int state;
	int lastState;
	char stateString[16];

	state = padGetState(port, slot);
	lastState = -1;
	
	while((state != PAD_STATE_STABLE) && (state != PAD_STATE_FINDCTP1))
	{
		if(state != lastState) {
			padStateInt2String(state, stateString);
#ifdef _DEBUG_GP
			printf("Please wait, pad(%d,%d) is in state %s\n", port, slot, stateString);
#endif
		}
		lastState = state;
		state=padGetState(port, slot);
	}
	
#ifdef _DEBUG_GP
	// Were the pad ever 'out of sync'?
	if(lastState != -1) {
		printf("Pad OK!\n");
	}
#endif
	
	return 0;
}

//
// initializePad
//

int initializePad( int port, int slot )
{
	int ret;
	int modes;
	int i;

	waitPadReady(port, slot);

	// How many different modes can this device operate in?
	// i.e. get # entrys in the modetable
	modes = padInfoMode(port, slot, PAD_MODETABLE, -1);

#ifdef _DEBUG_GP
	printf("The device has %d modes\n", modes);

	if( modes > 0 )
	{
		printf("( ");
		for (i = 0; i < modes; i++) {
			printf("%d ", padInfoMode(port, slot, PAD_MODETABLE, i));
		}
		printf(")");
	}

	printf("It is currently using mode %d\n", padInfoMode(port, slot, PAD_MODECURID, 0));
#endif

	// If modes == 0, this is not a Dual shock controller 
	// (it has no actuator engines)
	if( modes == 0 ) {
#ifdef _DEBUG_GP
		printf("This is a digital controller?\n");
#endif
		return 1;
	}

	// Verify that the controller has a DUAL SHOCK mode
	i = 0;
	do {
		if(padInfoMode(port, slot, PAD_MODETABLE, i) == PAD_TYPE_DUALSHOCK)
			break;
		i++;
	} while(i < modes);
	
	if( i >= modes ) {
#ifdef _DEBUG_GP
		printf("This is no Dual Shock controller\n");
#endif
		return 1;
	}

	// If ExId != 0x0 => This controller has actuator engines
	// This check should always pass if the Dual Shock test above passed
	ret = padInfoMode(port, slot, PAD_MODECUREXID, 0);
	if (ret == 0) {
#ifdef _DEBUG_GP
		printf("This is no Dual Shock controller??\n");
#endif
		return 1;
	}

#ifdef _DEBUG_GP
	printf("Enabling dual shock functions\n");
#endif

	// When using MMODE_LOCK, user cant change mode with Select button
	padSetMainMode(port, slot, PAD_MMODE_DUALSHOCK, PAD_MMODE_LOCK);

	waitPadReady(port, slot);
#ifdef _DEBUG_GP
	printf("infoPressMode: %d\n", padInfoPressMode(port, slot));
#endif

	waitPadReady(port, slot);
#ifdef _DEBUG_GP
	printf("enterPressMode: %d\n", padEnterPressMode(port, slot));
#endif

	waitPadReady(port, slot);
	actuators = padInfoAct(port, slot, -1, 0);
#ifdef _DEBUG_GP
	printf("# of actuators: %d\n",actuators);
#endif

	if( actuators != 0 )
	{
		actAlign[0] = 0;   // Enable small engine
		actAlign[1] = 1;   // Enable big engine
		actAlign[2] = 0xff;
		actAlign[3] = 0xff;
		actAlign[4] = 0xff;
		actAlign[5] = 0xff;

		waitPadReady(port, slot);
		padSetActAlign(port, slot, actAlign);
	}
	else {
#ifdef _DEBUG_GP
		printf("Did not find any actuators.\n");
#endif
	}

	waitPadReady(port, slot);
	return 1;
}
