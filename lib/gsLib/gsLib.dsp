# Microsoft Developer Studio Project File - Name="gsLib" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) External Target" 0x0106

CFG=gsLib - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "gsLib.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "gsLib.mak" CFG="gsLib - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "gsLib - Win32 Release" (based on "Win32 (x86) External Target")
!MESSAGE "gsLib - Win32 Debug" (based on "Win32 (x86) External Target")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""

!IF  "$(CFG)" == "gsLib - Win32 Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Release"
# PROP BASE Intermediate_Dir "Release"
# PROP BASE Cmd_Line "NMAKE /f gsLib.mak"
# PROP BASE Rebuild_Opt "/a"
# PROP BASE Target_File "gsLib.exe"
# PROP BASE Bsc_Name "gsLib.bsc"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "../../vc6/Release"
# PROP Intermediate_Dir "../../vc6/Release"
# PROP Cmd_Line "make"
# PROP Rebuild_Opt "clean"
# PROP Target_File "gsLib.a"
# PROP Bsc_Name ""
# PROP Target_Dir ""

!ELSEIF  "$(CFG)" == "gsLib - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Debug"
# PROP BASE Intermediate_Dir "Debug"
# PROP BASE Cmd_Line "NMAKE /f gsLib.mak"
# PROP BASE Rebuild_Opt "/a"
# PROP BASE Target_File "gsLib.exe"
# PROP BASE Bsc_Name "gsLib.bsc"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "../../vc6/Debug"
# PROP Intermediate_Dir "../../vc6/Debug"
# PROP Cmd_Line "make"
# PROP Rebuild_Opt "clean"
# PROP Target_File "gsLib.a"
# PROP Bsc_Name ""
# PROP Target_Dir ""

!ENDIF 

# Begin Target

# Name "gsLib - Win32 Release"
# Name "gsLib - Win32 Debug"

!IF  "$(CFG)" == "gsLib - Win32 Release"

!ELSEIF  "$(CFG)" == "gsLib - Win32 Debug"

!ENDIF 

# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Source File

SOURCE=.\gsCore.c
# End Source File
# Begin Source File

SOURCE=.\gsFont.c
# End Source File
# Begin Source File

SOURCE=.\gsMisc.c
# End Source File
# Begin Source File

SOURCE=.\gsPrimitive.c
# End Source File
# Begin Source File

SOURCE=.\gsTexture.c
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl"
# Begin Source File

SOURCE=.\gsCore.h
# End Source File
# Begin Source File

SOURCE=.\gsFont.h
# End Source File
# Begin Source File

SOURCE=.\gsLib.h
# End Source File
# Begin Source File

SOURCE=.\gsMisc.h
# End Source File
# Begin Source File

SOURCE=.\gsPrimitive.h
# End Source File
# Begin Source File

SOURCE=.\gsTexture.h
# End Source File
# End Group
# Begin Group "Resource Files"

# PROP Default_Filter "ico;cur;bmp;dlg;rc2;rct;bin;rgs;gif;jpg;jpeg;jpe"
# End Group
# End Target
# End Project
