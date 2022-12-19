# Microsoft Developer Studio Project File - Name="Spy" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) External Target" 0x0106

CFG=Spy - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "Spy.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "Spy.mak" CFG="Spy - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "Spy - Win32 Release" (based on "Win32 (x86) External Target")
!MESSAGE "Spy - Win32 Debug" (based on "Win32 (x86) External Target")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath "Desktop"
# PROP WCE_FormatVersion "6.0"

!IF  "$(CFG)" == "Spy - Win32 Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Release"
# PROP BASE Intermediate_Dir "Release"
# PROP BASE Cmd_Line "NMAKE /f Spy.mak"
# PROP BASE Rebuild_Opt "/a"
# PROP BASE Target_File "Spy.exe"
# PROP BASE Bsc_Name "Spy.bsc"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "Release"
# PROP Intermediate_Dir "Release"
# PROP Cmd_Line "NMAKE /NOLOGO"
# PROP Rebuild_Opt "/a"
# PROP Target_File "Spy.exe"
# PROP Bsc_Name "Spy.bsc"
# PROP Target_Dir ""

!ELSEIF  "$(CFG)" == "Spy - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Debug"
# PROP BASE Intermediate_Dir "Debug"
# PROP BASE Cmd_Line "NMAKE /f Spy.mak"
# PROP BASE Rebuild_Opt "/a"
# PROP BASE Target_File "Spy.exe"
# PROP BASE Bsc_Name "Spy.bsc"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "Debug"
# PROP Intermediate_Dir "Debug"
# PROP Cmd_Line "nmake /nologo"
# PROP Rebuild_Opt "/a"
# PROP Target_File "Spy.exe"
# PROP Bsc_Name ""
# PROP Target_Dir ""

!ENDIF 

# Begin Target

# Name "Spy - Win32 Release"
# Name "Spy - Win32 Debug"

!IF  "$(CFG)" == "Spy - Win32 Release"

!ELSEIF  "$(CFG)" == "Spy - Win32 Debug"

!ENDIF 

# Begin Group "inc"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\inc\386.H
# End Source File
# Begin Source File

SOURCE=.\inc\387.H
# End Source File
# Begin Source File

SOURCE=.\inc\ALL.H
# End Source File
# Begin Source File

SOURCE=.\inc\BIOS.H
# End Source File
# Begin Source File

SOURCE=.\inc\CMOS.H
# End Source File
# Begin Source File

SOURCE=.\inc\COM.H
# End Source File
# Begin Source File

SOURCE=.\inc\DEBUG.H
# End Source File
# Begin Source File

SOURCE=.\inc\DISK.H
# End Source File
# Begin Source File

SOURCE=.\inc\DOS.H
# End Source File
# Begin Source File

SOURCE=.\inc\EMS.H
# End Source File
# Begin Source File

SOURCE=.\inc\GLOBALS.H
# End Source File
# Begin Source File

SOURCE=.\inc\KBD.H
# End Source File
# Begin Source File

SOURCE=.\inc\LIB.H
# End Source File
# Begin Source File

SOURCE=.\inc\PARSE.H
# End Source File
# Begin Source File

SOURCE=.\inc\PIC.H
# End Source File
# Begin Source File

SOURCE=.\inc\SYM.H
# End Source File
# Begin Source File

SOURCE=.\inc\TIMER.H
# End Source File
# Begin Source File

SOURCE=.\inc\TYPES.H
# End Source File
# Begin Source File

SOURCE=.\inc\VIDEO.H
# End Source File
# Begin Source File

SOURCE=.\inc\X86.H
# End Source File
# End Group
# Begin Group "lib"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\lib\GETS.C
# End Source File
# Begin Source File

SOURCE=.\lib\PRINTF.C
# End Source File
# Begin Source File

SOURCE=.\lib\SPRINTF.C
# End Source File
# Begin Source File

SOURCE=.\lib\STRING.C
# End Source File
# End Group
# Begin Source File

SOURCE=.\386INIT.ASM
# End Source File
# Begin Source File

SOURCE=.\386TRAP.ASM
# End Source File
# Begin Source File

SOURCE=.\COMIO.ASM
# End Source File
# Begin Source File

SOURCE=.\DOSDUMP.C
# End Source File
# Begin Source File

SOURCE=.\INTPARSE.C
# End Source File
# Begin Source File

SOURCE=.\MAIN.C
# End Source File
# Begin Source File

SOURCE=.\MAKEFILE
# End Source File
# Begin Source File

SOURCE=.\MEMMGR.C
# End Source File
# Begin Source File

SOURCE=.\REBOOT.ASM
# End Source File
# Begin Source File

SOURCE=.\V86INT.ASM
# End Source File
# Begin Source File

SOURCE=.\V86IO.ASM
# End Source File
# Begin Source File

SOURCE=.\VIDMGR.C
# End Source File
# Begin Source File

SOURCE=.\VIDSAVE.ASM
# End Source File
# Begin Source File

SOURCE=.\X86DEBUG.C
# End Source File
# End Target
# End Project
