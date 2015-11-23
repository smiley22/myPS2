# myPS2 makefile

EE_OBJS_DIR	= obj
EE_BIN_DIR	= bin
EE_BIN		= MYPS2.ELF

#DEVELOPER	= 1

EE_OBJS =	main.o GUI.o GUI_Load.o	GUI_Ctrl_Button.o GUI_Ctrl_Combo.o GUI_Ctrl_Dirview.o	\
		GUI_Ctrl_Image.o GUI_Ctrl_Label.o GUI_Ctrl_List.o GUI_Ctrl_Progress.o		\
		GUI_Ctrl_Slider.o GUI_Ctrl_ThumbnailPanel.o GUI_Ctrl_ToggleButton.o		\
		GUI_M_DlgKeyboard.o GUI_M_DlgMsgBox.o GUI_M_DlgPartition.o GUI_M_DlgYesNo.o	\
		GUI_M_Language.o GUI_M_Main.o GUI_M_MyFiles.o GUI_M_MyMusic.o			\
		GUI_M_MyPictures.o GUI_M_MyPrograms.o GUI_M_Network.o GUI_M_Partitions.o	\
		GUI_M_Radio.o GUI_M_Samba.o GUI_M_Settings.o GUI_M_Skins.o GUI_M_View.o		\
		gamepad.o elf.o	imgscale.o file.o sysconf.o mp3.o scheduler.o net.o		\
		bootscreen.o mass_rpc.o cdvd_rpc.o charset.o sjpcm_rpc.o samba.o		\
		usb_mass.o iomanx.o filexio.o ps2dev9.o ps2atad.o ps2hdd.o ps2fs.o poweroff.o	\
		cdvd.o ps2ip.o ps2ips.o ps2smap.o ps2ftpd.o sjpcm.o dns.o boot_bkg.o		\
		boot_font.o boot_dat.o

EE_OBJS	:= $(EE_OBJS:%=$(EE_OBJS_DIR)/%)

EE_LIBS	=	-lc -lgslib -ldmaKit -lpad -lfileXio -ljpg -lmf	-lhdd -lpng -lm	 -lz -lmc	\
		-lpatches -lpoweroff -lmad -lsample -ldebug -lps2ip -lscew -lexpat -liconv	\
		-lps2time -lcdvd -lsmbclient

# add lib directories
EE_LDFLAGS +=	-L../lib/gsLib -L../lib/dmaKit -L../lib/libmad -L../lib/libsample -L../lib/libjpg \
		-L../lib/libexpat -L../lib/libscew -L../lib/libpng -L../lib/libz \
		-L../lib/libiconv -L../lib/libps2time -L../lib/libsmb

# add include directories
EE_INCS += -I../lib/libexpat

ifdef DEVELOPER
EE_CFLAGS += -D_DEVELOPER
endif

all: $(EE_OBJS_DIR) $(EE_BIN_DIR) $(EE_BIN)
	mv $(EE_BIN) $(EE_BIN_DIR)/$(EE_BIN)

$(EE_OBJS_DIR):
	@$(MKDIR) -p $(EE_OBJS_DIR)

$(EE_BIN_DIR):
	@$(MKDIR) -p $(EE_BIN_DIR)

$(EE_OBJS_DIR)/%.o : %.c
	$(EE_CC) $(EE_CFLAGS) $(EE_INCS) -c $< -o $@

$(EE_OBJS_DIR)/%.o : %.s
	$(EE_AS) $(EE_ASFLAGS) $< -o $@

$(EE_OBJS_DIR)/%.o : %.S
	$(EE_CC) $(EE_CFLAGS) $(EE_INCS) -c $< -o $@

$(EE_OBJS_DIR)/usb_mass.s:
	bin2s ../irx/usb_mass/usb_mass.irx $(EE_OBJS_DIR)/usb_mass.s usb_mass_irx

$(EE_OBJS_DIR)/iomanx.s:
	bin2s $(PS2SDK)/iop/irx/iomanx.irx $(EE_OBJS_DIR)/iomanx.s iomanx_irx

$(EE_OBJS_DIR)/filexio.s:
	bin2s $(PS2SDK)/iop/irx/filexio.irx $(EE_OBJS_DIR)/filexio.s filexio_irx

$(EE_OBJS_DIR)/ps2dev9.s:
	bin2s $(PS2SDK)/iop/irx/ps2dev9.irx $(EE_OBJS_DIR)/ps2dev9.s ps2dev9_irx

$(EE_OBJS_DIR)/ps2atad.s:
	bin2s $(PS2SDK)/iop/irx/ps2atad.irx $(EE_OBJS_DIR)/ps2atad.s ps2atad_irx

$(EE_OBJS_DIR)/ps2hdd.s:
	bin2s $(PS2SDK)/iop/irx/ps2hdd.irx $(EE_OBJS_DIR)/ps2hdd.s ps2hdd_irx

$(EE_OBJS_DIR)/ps2fs.s:
	bin2s $(PS2SDK)/iop/irx/ps2fs.irx $(EE_OBJS_DIR)/ps2fs.s ps2fs_irx

$(EE_OBJS_DIR)/poweroff.s:
	bin2s $(PS2SDK)/iop/irx/poweroff.irx $(EE_OBJS_DIR)/poweroff.s poweroff_irx

$(EE_OBJS_DIR)/cdvd.s:
	bin2s ../irx/cdvd/cdvd.irx $(EE_OBJS_DIR)/cdvd.s cdvd_irx

$(EE_OBJS_DIR)/ps2ip.s:
	bin2s $(PS2SDK)/iop/irx/PS2IP.IRX $(EE_OBJS_DIR)/ps2ip.s ps2ip_irx

$(EE_OBJS_DIR)/ps2ips.s:
	bin2s $(PS2SDK)/iop/irx/ps2ips.irx $(EE_OBJS_DIR)/ps2ips.s ps2ips_irx

$(EE_OBJS_DIR)/ps2smap.s:
	bin2s ../irx/ps2smap/ps2smap.irx $(EE_OBJS_DIR)/ps2smap.s ps2smap_irx

$(EE_OBJS_DIR)/ps2ftpd.s:
	bin2s ../irx/ps2ftpd/ps2ftpd.irx $(EE_OBJS_DIR)/ps2ftpd.s ps2ftpd_irx

$(EE_OBJS_DIR)/sjpcm.s:
	bin2s ../irx/sjpcm/sjpcm.irx $(EE_OBJS_DIR)/sjpcm.s sjpcm_irx

$(EE_OBJS_DIR)/dns.s:
	bin2s $(PS2SDK)/iop/irx/dns.irx $(EE_OBJS_DIR)/dns.s dns_irx

$(EE_OBJS_DIR)/boot_bkg.s:
	bin2s ../data/boot_bkg.png $(EE_OBJS_DIR)/boot_bkg.s boot_bkg

$(EE_OBJS_DIR)/boot_font.s:
	bin2s ../data/boot_font.png $(EE_OBJS_DIR)/boot_font.s boot_font

$(EE_OBJS_DIR)/boot_dat.s:
	bin2s ../data/boot_dat.dat $(EE_OBJS_DIR)/boot_dat.s boot_dat

clean:
	rm -f -r $(EE_OBJS_DIR)
	rm -f $(EE_BIN_DIR)/$(EE_BIN)
ifndef DEVELOPER
	rm -f -r $(EE_OBJS_DIR) $(EE_BIN_DIR)
endif

include $(PS2SDK)/Defs.make
include $(PS2SDK)/samples/Makefile.eeglobal
