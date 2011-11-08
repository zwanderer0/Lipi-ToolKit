# Microsoft Developer Studio Project File - Name="utils" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Static Library" 0x0104

CFG=utils - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "utils.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "utils.mak" CFG="utils - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "utils - Win32 Release" (based on "Win32 (x86) Static Library")
!MESSAGE "utils - Win32 Debug" (based on "Win32 (x86) Static Library")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName "utils"
# PROP Scc_LocalPath "..\..\.."
CPP=cl.exe
RSC=rc.exe

!IF  "$(CFG)" == "utils - Win32 Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Release"
# PROP BASE Intermediate_Dir "Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "..\..\..\..\lib"
# PROP Intermediate_Dir "Release"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_MBCS" /D "_LIB" /YX /FD /c
# ADD CPP /nologo /MD /W3 /GX /O2 /I "..\..\..\..\include" /I "..\..\..\..\reco\shaperec\common" /D "WIN32" /D "NDEBUG" /D "_MBCS" /D "_LIB" /YX /FD /c
# ADD BASE RSC /l 0x409 /d "NDEBUG"
# ADD RSC /l 0x409 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo

!ELSEIF  "$(CFG)" == "utils - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Debug"
# PROP BASE Intermediate_Dir "Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "..\..\..\..\lib"
# PROP Intermediate_Dir "Debug"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_MBCS" /D "_LIB" /YX /FD /GZ /c
# ADD CPP /nologo /MDd /w /W0 /Gm /GX /ZI /Od /I "..\..\..\..\include" /I "..\..\..\..\reco\shaperec\common" /D "WIN32" /D "_DEBUG" /D "_MBCS" /D "_LIB" /FR /YX /FD /GZ /c
# ADD BASE RSC /l 0x409 /d "_DEBUG"
# ADD RSC /l 0x409 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo

!ENDIF 

# Begin Target

# Name "utils - Win32 Release"
# Name "utils - Win32 Debug"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Source File

SOURCE=..\..\LTKCheckSumGenerate.cpp
# End Source File
# Begin Source File

SOURCE=..\..\LTKConfigFileReader.cpp
# End Source File
# Begin Source File

SOURCE=..\..\LTKErrors.cpp
# End Source File
# Begin Source File

SOURCE=..\..\LTKImageWriter.cpp
# End Source File
# Begin Source File

SOURCE=..\..\LTKInkFileReader.cpp
# End Source File
# Begin Source File

SOURCE=..\..\LTKInkFileWriter.cpp
# End Source File
# Begin Source File

SOURCE=..\..\LTKInkUtils.cpp
# End Source File
# Begin Source File

SOURCE=..\..\LTKLoggerUtil.cpp
# End Source File
# Begin Source File

SOURCE=..\..\LTKOSUtilFactory.cpp
# End Source File
# Begin Source File

SOURCE=..\..\LTKStrEncoding.cpp
# End Source File
# Begin Source File

SOURCE=..\..\LTKStringUtil.cpp
# End Source File
# Begin Source File

SOURCE=..\..\LTKVersionCompatibilityCheck.cpp
# End Source File
# Begin Source File

SOURCE=..\..\LTKWindowsUtil.cpp
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl"
# Begin Source File

SOURCE=..\..\LTKCheckSumGenerate.h
# End Source File
# Begin Source File

SOURCE=..\..\LTKConfigFileReader.h
# End Source File
# Begin Source File

SOURCE=..\..\LTKErrors.h
# End Source File
# Begin Source File

SOURCE=..\..\LTKHierarchicalClustering.h
# End Source File
# Begin Source File

SOURCE=..\..\LTKImageWriter.h
# End Source File
# Begin Source File

SOURCE=..\..\LTKInkFileReader.h
# End Source File
# Begin Source File

SOURCE=..\..\LTKInkFileWriter.h
# End Source File
# Begin Source File

SOURCE=..\..\LTKInkUtils.h
# End Source File
# Begin Source File

SOURCE=..\..\LTKLoggerUtil.h
# End Source File
# Begin Source File

SOURCE=..\..\LTKStrEncoding.h
# End Source File
# Begin Source File

SOURCE=..\..\LTKStringUtil.h
# End Source File
# Begin Source File

SOURCE=..\..\LTKVersionCompatibilityCheck.h
# End Source File
# Begin Source File

SOURCE=..\..\LTKWindowsUtil.h
# End Source File
# End Group
# End Target
# End Project
