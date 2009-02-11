# Microsoft Developer Studio Project File - Name="sms_sdl" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Console Application" 0x0103

CFG=sms_sdl - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "sms_sdl.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "sms_sdl.mak" CFG="sms_sdl - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "sms_sdl - Win32 Release" (based on "Win32 (x86) Console Application")
!MESSAGE "sms_sdl - Win32 Debug" (based on "Win32 (x86) Console Application")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
RSC=rc.exe

!IF  "$(CFG)" == "sms_sdl - Win32 Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Release"
# PROP BASE Intermediate_Dir "Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "Release"
# PROP Intermediate_Dir "Release"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_CONSOLE" /D "_MBCS" /YX /FD /c
# ADD CPP /nologo /MD /W3 /Ox /D "NDEBUG" /D "WIN32" /D "_CONSOLE" /D "_MBCS" /D "LSB_FIRST" /D "USE_ZLIB" /YX /FD /I../.. /I../../cpu /I../../sound /c
# ADD BASE RSC /l 0x40c /d "NDEBUG"
# ADD RSC /l 0x40c /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:console /machine:I386
# ADD LINK32 sdlmain.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib winmm.lib sdl.lib zlib.lib /nologo /subsystem:console /machine:I386 /out:"sms_sdl.exe"
# SUBTRACT LINK32 /profile /debug /nodefaultlib

!ELSEIF  "$(CFG)" == "sms_sdl - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Debug"
# PROP BASE Intermediate_Dir "Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "Debug"
# PROP Intermediate_Dir "Debug"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_CONSOLE" /D "_MBCS" /YX /FD /GZ /c
# ADD CPP /nologo /MTd /W3 /Gm /GX /ZI /Od /D "_DEBUG" /D "WIN32" /D "_CONSOLE" /D "_MBCS" /D "LSB_FIRST" /D "USE_ZLIB" /YX /FD /GZ /I../.. /I../../cpu /I../../sound /c
# ADD BASE RSC /l 0x40c /d "_DEBUG"
# ADD RSC /l 0x40c /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:console /debug /machine:I386 /pdbtype:sept
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib winmm.lib sdl.lib zlib.lib /nologo /subsystem:console /profile /debug /machine:I386 /out:"sms_sdl.exe"
# SUBTRACT LINK32 /nodefaultlib

!ENDIF 

# Begin Target

# Name "sms_sdl - Win32 Release"
# Name "sms_sdl - Win32 Debug"
# Begin Group "core"

# PROP Default_Filter ""
# Begin Group "cpu"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\..\cpu\cpuintrf.h
# End Source File
# Begin Source File

SOURCE=..\..\cpu\osd_cpu.h
# End Source File
# Begin Source File

SOURCE=..\..\cpu\z80.c
# End Source File
# Begin Source File

SOURCE=..\..\cpu\z80.h
# End Source File
# Begin Source File

SOURCE=..\..\cpu\z80daa.h
# End Source File
# End Group
# Begin Group "sound"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\..\sound\2413tone.h
# End Source File
# Begin Source File

SOURCE=..\..\sound\emu2413.c
# End Source File
# Begin Source File

SOURCE=..\..\sound\emu2413.h
# End Source File
# Begin Source File

SOURCE=..\..\sound\sn76496.c
# End Source File
# Begin Source File

SOURCE=..\..\sound\sn76496.h
# End Source File
# Begin Source File

SOURCE=..\..\sound\vrc7tone.h
# End Source File
# End Group
# Begin Source File

SOURCE=..\..\render.c
# End Source File
# Begin Source File

SOURCE=..\..\render.h
# End Source File
# Begin Source File

SOURCE=..\..\shared.h
# End Source File
# Begin Source File

SOURCE=..\..\sms.c
# End Source File
# Begin Source File

SOURCE=..\..\sms.h
# End Source File
# Begin Source File

SOURCE=..\..\system.c
# End Source File
# Begin Source File

SOURCE=..\..\system.h
# End Source File
# Begin Source File

SOURCE=..\..\types.h
# End Source File
# Begin Source File

SOURCE=..\..\vdp.c
# End Source File
# Begin Source File

SOURCE=..\..\vdp.h
# End Source File
# End Group
# Begin Group "sdl"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\filters.c
# End Source File
# Begin Source File

SOURCE=..\filters.h
# End Source File
# Begin Source File

SOURCE=..\main.c
# End Source File
# Begin Source File

SOURCE=..\saves.c
# End Source File
# Begin Source File

SOURCE=..\saves.h
# End Source File
# Begin Source File

SOURCE=..\sdlsms.c
# End Source File
# Begin Source File

SOURCE=..\sdlsms.h
# End Source File
# Begin Source File

SOURCE=..\unzip.c
# End Source File
# Begin Source File

SOURCE=..\unzip.h
# End Source File
# End Group
# Begin Source File

SOURCE=..\README.TXT
# End Source File
# End Target
# End Project
