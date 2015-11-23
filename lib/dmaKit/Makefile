# myPS2 Makefile for dmaKit
#
# Author: tk
# Date: 12-22-2005
#

EE_INCS	= -I.
EE_LIB	= libdmakit.a

EE_OBJS =	dmaCore.o \
		dmaInit.o \
		dmaSpr.o

EE_CFLAGS += -Wno-unused

all:	$(EE_LIB)
	
clean:
	rm -f *.o *.a

include $(PS2SDK)/samples/Makefile.pref
include $(PS2SDK)/samples/Makefile.eeglobal
