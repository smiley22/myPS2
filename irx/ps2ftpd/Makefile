# myPS2 ps2ftpd Makefile
#
# Author: tk
# Date: 11-22-2005
#

IOP_OBJS	= main.o	\
		  FileSystem.o	\
		  FtpServer.o	\
		  FtpClient.o	\
		  FtpCommands.o	\
		  FtpMessages.o	\
		  imports.o

IOP_BIN		= ps2ftpd.irx

IOP_LDFLAGS += -s
IOP_INCS += -I$(PS2SDK)/iop/include

all: $(IOP_BIN)

clean:
	rm -f *.o *.s *.irx

include $(PS2SDK)/samples/Makefile.pref
include $(PS2SDK)/samples/Makefile.iopglobal

# compiling with iop-gcc 3.2.2 causes weird problems in ps2ftpd
# temporary solution is to turn off compiler optimization

IOP_CFLAGS = $(CFLAGS_TARGET) -G0 -c $(IOP_INCS) -Wall -fno-builtin

# A rule to build imports.lst.
%.o : %.lst
	echo "#include \"irx_imports.h\"" > build-imports.c
	cat $< >> build-imports.c
	$(IOP_CC) $(IOP_CFLAGS) build-imports.c -o $@
	-rm -f build-imports.c