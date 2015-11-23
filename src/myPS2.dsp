# Microsoft Developer Studio Project File - Name="myPS2" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) External Target" 0x0106

CFG=myPS2 - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "myPS2.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "myPS2.mak" CFG="myPS2 - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "myPS2 - Win32 Release" (based on "Win32 (x86) External Target")
!MESSAGE "myPS2 - Win32 Debug" (based on "Win32 (x86) External Target")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""

!IF  "$(CFG)" == "myPS2 - Win32 Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Release"
# PROP BASE Intermediate_Dir "Release"
# PROP BASE Cmd_Line "NMAKE /f myPS2.mak"
# PROP BASE Rebuild_Opt "/a"
# PROP BASE Target_File "MYPS2.ELF"
# PROP BASE Bsc_Name "myPS2.bsc"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "../vc6/Release"
# PROP Intermediate_Dir "../vc6/Release"
# PROP Cmd_Line "make"
# PROP Rebuild_Opt "clean"
# PROP Target_File "MYPS2.ELF"
# PROP Bsc_Name ""
# PROP Target_Dir ""

!ELSEIF  "$(CFG)" == "myPS2 - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Debug"
# PROP BASE Intermediate_Dir "Debug"
# PROP BASE Cmd_Line "NMAKE /f myPS2.mak"
# PROP BASE Rebuild_Opt "/a"
# PROP BASE Target_File "MYPS2.ELF"
# PROP BASE Bsc_Name "myPS2.bsc"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "../vc6/Debug"
# PROP Intermediate_Dir "../vc6/Debug"
# PROP Cmd_Line "make"
# PROP Rebuild_Opt "clean"
# PROP Target_File "MYPS2.ELF"
# PROP Bsc_Name ""
# PROP Target_Dir ""

!ENDIF 

# Begin Target

# Name "myPS2 - Win32 Release"
# Name "myPS2 - Win32 Debug"

!IF  "$(CFG)" == "myPS2 - Win32 Release"

!ELSEIF  "$(CFG)" == "myPS2 - Win32 Debug"

!ENDIF 

# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Source File

SOURCE=.\bootscreen.c
# End Source File
# Begin Source File

SOURCE=.\cdvd_rpc.c
# End Source File
# Begin Source File

SOURCE=.\charset.c
# End Source File
# Begin Source File

SOURCE=.\elf.c
# End Source File
# Begin Source File

SOURCE=.\file.c
# End Source File
# Begin Source File

SOURCE=.\gamepad.c
# End Source File
# Begin Source File

SOURCE=.\GUI.c
# End Source File
# Begin Source File

SOURCE=.\GUI_Ctrl_Button.c
# End Source File
# Begin Source File

SOURCE=.\GUI_Ctrl_Combo.c
# End Source File
# Begin Source File

SOURCE=.\GUI_Ctrl_Dirview.c
# End Source File
# Begin Source File

SOURCE=.\GUI_Ctrl_Image.c
# End Source File
# Begin Source File

SOURCE=.\GUI_Ctrl_Label.c
# End Source File
# Begin Source File

SOURCE=.\GUI_Ctrl_List.c
# End Source File
# Begin Source File

SOURCE=.\GUI_Ctrl_Progress.c
# End Source File
# Begin Source File

SOURCE=.\GUI_Ctrl_Slider.c
# End Source File
# Begin Source File

SOURCE=.\GUI_Ctrl_ThumbnailPanel.c
# End Source File
# Begin Source File

SOURCE=.\GUI_Ctrl_ToggleButton.c
# End Source File
# Begin Source File

SOURCE=.\GUI_Load.c
# End Source File
# Begin Source File

SOURCE=.\GUI_M_DlgKeyboard.c
# End Source File
# Begin Source File

SOURCE=.\GUI_M_DlgMsgBox.c
# End Source File
# Begin Source File

SOURCE=.\GUI_M_DlgPartition.c
# End Source File
# Begin Source File

SOURCE=.\GUI_M_DlgYesNo.c
# End Source File
# Begin Source File

SOURCE=.\GUI_M_Language.c
# End Source File
# Begin Source File

SOURCE=.\GUI_M_Main.c
# End Source File
# Begin Source File

SOURCE=.\GUI_M_MyFiles.c
# End Source File
# Begin Source File

SOURCE=.\GUI_M_MyMusic.c
# End Source File
# Begin Source File

SOURCE=.\GUI_M_MyPictures.c
# End Source File
# Begin Source File

SOURCE=.\GUI_M_MyPrograms.c
# End Source File
# Begin Source File

SOURCE=.\GUI_M_Network.c
# End Source File
# Begin Source File

SOURCE=.\GUI_M_Partitions.c
# End Source File
# Begin Source File

SOURCE=.\GUI_M_Radio.c
# End Source File
# Begin Source File

SOURCE=.\GUI_M_Samba.c
# End Source File
# Begin Source File

SOURCE=.\GUI_M_Settings.c
# End Source File
# Begin Source File

SOURCE=.\GUI_M_Skins.c
# End Source File
# Begin Source File

SOURCE=.\GUI_M_View.c
# End Source File
# Begin Source File

SOURCE=.\imgscale.c
# End Source File
# Begin Source File

SOURCE=.\main.c
# End Source File
# Begin Source File

SOURCE=.\mass_rpc.c
# End Source File
# Begin Source File

SOURCE=.\mp3.c
# End Source File
# Begin Source File

SOURCE=.\net.c
# End Source File
# Begin Source File

SOURCE=.\samba.c
# End Source File
# Begin Source File

SOURCE=.\scheduler.c
# End Source File
# Begin Source File

SOURCE=.\sjpcm_rpc.c
# End Source File
# Begin Source File

SOURCE=.\sysconf.c
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl"
# Begin Source File

SOURCE=.\cdvd.h
# End Source File
# Begin Source File

SOURCE=.\cdvd_rpc.h
# End Source File
# Begin Source File

SOURCE=.\charset.h
# End Source File
# Begin Source File

SOURCE=.\elf.h
# End Source File
# Begin Source File

SOURCE=.\file.h
# End Source File
# Begin Source File

SOURCE=.\gamepad.h
# End Source File
# Begin Source File

SOURCE=.\GUI.h
# End Source File
# Begin Source File

SOURCE=.\GUI_Ctrl.h
# End Source File
# Begin Source File

SOURCE=.\GUI_Lang.h
# End Source File
# Begin Source File

SOURCE=.\GUI_Menu.h
# End Source File
# Begin Source File

SOURCE=.\imgscale.h
# End Source File
# Begin Source File

SOURCE=.\mass_rpc.h
# End Source File
# Begin Source File

SOURCE=.\misc.h
# End Source File
# Begin Source File

SOURCE=.\mp3.h
# End Source File
# Begin Source File

SOURCE=.\net.h
# End Source File
# Begin Source File

SOURCE=.\samba.h
# End Source File
# Begin Source File

SOURCE=.\scheduler.h
# End Source File
# Begin Source File

SOURCE=.\sjpcm.h
# End Source File
# Begin Source File

SOURCE=.\sysconf.h
# End Source File
# End Group
# Begin Group "Resource Files"

# PROP Default_Filter "ico;cur;bmp;dlg;rc2;rct;bin;rgs;gif;jpg;jpeg;jpe"
# End Group
# End Target
# End Project
