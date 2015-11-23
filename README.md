# myPS2
A media viewer and file manager for the PlayStation 2

### License
myPS2 is licensed under the GPL. See the file LICENSE included in this
distribution for licensing terms.

### Credits
Copyright © 2005-2006 Torben Könke

### Compiling
You need to have PS2SDK to compile myPS2.

You will have to download these 2 modules before compiling:
	
	CDVD.IRX
	Can be found on ps2dev.org under "Projects"
	LIBCDVD: CD+DVD Filing System for PS2 1.15
	Place it under /irx/cdvd/

	SJPCM.IRX
	Can be found on ps2dev.org under "Sample Code"
	SjPCM v2.1 - PCM streaming library for the PS2)
	Place it under /irx/sjpcm/

	usb_mass.irx
	Can be found in PS2Dev Svn under /ps2/usb_mass/.
	Place it under /irx/usb_mass/

You will also have to checkout ps2eth from SVN (/ps2/ps2eth/) and
compile it to build the ps2smap.irx module (It's also included in the
PS2Link v1.24 download) and place it under /irx/ps2smap/.

I have made some changes to the PS2IPS module. You can find the changed
files under /ps2sdk/ps2sdksrc/.

To compile just 'make' the makefile in the main directory and it should put
out a MYPS2.ELF in the same directory.

A workspace for compiling from Visual C++ 6.0 can be found in /vc6/.