# Microsoft Developer Studio Project File - Name="CreepingLine" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Application" 0x0101

CFG=CreepingLine - Win32 Debug Unicode Rus
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "CreepingLine.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "CreepingLine.mak" CFG="CreepingLine - Win32 Debug Unicode Rus"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "CreepingLine - Win32 Debug Unicode" (based on "Win32 (x86) Application")
!MESSAGE "CreepingLine - Win32 Release Unicode" (based on "Win32 (x86) Application")
!MESSAGE "CreepingLine - Win32 Debug Unicode Rus" (based on "Win32 (x86) Application")
!MESSAGE "CreepingLine - Win32 Release Unicode Rus" (based on "Win32 (x86) Application")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
MTL=midl.exe
RSC=rc.exe

!IF  "$(CFG)" == "CreepingLine - Win32 Debug Unicode"

# PROP BASE Use_MFC 6
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "CreepingLine___Win32_Debug_Unicode"
# PROP BASE Intermediate_Dir "CreepingLine___Win32_Debug_Unicode"
# PROP BASE Ignore_Export_Lib 0
# PROP BASE Target_Dir ""
# PROP Use_MFC 6
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "..\~Binaries\Debug"
# PROP Intermediate_Dir "..\~Intermediate\Debug\CreepingLine"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MDd /W3 /Gm /GX /ZI /Od /D "_DEBUG" /D "WIN32" /D "_WINDOWS" /D "_AFXDLL" /D "_MBCS" /D "CREEPING_LINE_PRJ" /Yu"stdafx.h" /FD /GZ /c
# ADD CPP /nologo /MDd /W3 /Gm /GX /ZI /Od /I "../Libs/appweb" /I "../Libs/appweb/include" /I "../Inc" /I "../Libs" /I "../Presenter.Engine" /I "../CustomResolution" /D "_DEBUG" /D "WIN" /D "WIN32" /D "_WINDOWS" /D "_AFXDLL" /D "_UNICODE" /D "UNICODE" /D "CREEPING_LINE_PRJ" /D "LANG_INTERFACE_ENG" /FR /Yu"stdafx.h" /FD /GZ /c
# ADD BASE MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x419 /d "_DEBUG" /d "_AFXDLL"
# ADD RSC /l 0x409 /d "_DEBUG" /d "_AFXDLL" /d "LANG_INTERFACE_ENG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 /nologo /subsystem:windows /debug /machine:I386 /pdbtype:sept
# ADD LINK32 cppunit_dll.lib CustomResolution.lib /nologo /entry:"wWinMainCRTStartup" /subsystem:windows /debug /machine:I386 /pdbtype:sept /libpath:"..\~Binaries\Debug"
# SUBTRACT LINK32 /nodefaultlib
# Begin Special Build Tool
TargetPath=\Work\APPLICA\sources\Presenter\~Binaries\Debug\CreepingLine.exe
SOURCE="$(InputPath)"
PostBuild_Desc=Run tests...
PostBuild_Cmds="$(TargetPath)" /Test
# End Special Build Tool

!ELSEIF  "$(CFG)" == "CreepingLine - Win32 Release Unicode"

# PROP BASE Use_MFC 6
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "CreepingLine___Win32_Release_Unicode"
# PROP BASE Intermediate_Dir "CreepingLine___Win32_Release_Unicode"
# PROP BASE Ignore_Export_Lib 0
# PROP BASE Target_Dir ""
# PROP Use_MFC 6
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "..\~Binaries\ReleaseLib"
# PROP Intermediate_Dir "..\~Intermediate\Release\CreepingLine"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MD /W3 /GX /O2 /D "NDEBUG" /D "WIN32" /D "_WINDOWS" /D "_AFXDLL" /D "_MBCS" /D "CREEPING_LINE_PRJ" /Yu"stdafx.h" /FD /c
# ADD CPP /nologo /MD /W3 /GX /O2 /I "../Libs/appweb" /I "../Libs/appweb/include" /I "../Inc" /I "../Libs" /I "../Presenter.Engine" /I "../CustomResolution" /D "NDEBUG" /D "WIN" /D "WIN32" /D "_WINDOWS" /D "_AFXDLL" /D "_UNICODE" /D "UNICODE" /D "CREEPING_LINE_PRJ" /D "LANG_INTERFACE_ENG" /FR /Yu"stdafx.h" /FD /c
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x419 /d "NDEBUG" /d "_AFXDLL"
# ADD RSC /l 0x409 /d "NDEBUG" /d "_AFXDLL" /d "LANG_INTERFACE_ENG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 /nologo /subsystem:windows /machine:I386
# ADD LINK32 CustomResolution.lib /nologo /entry:"wWinMainCRTStartup" /subsystem:windows /machine:I386 /libpath:"..\~Binaries\~Release Unicode"

!ELSEIF  "$(CFG)" == "CreepingLine - Win32 Debug Unicode Rus"

# PROP BASE Use_MFC 6
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "CreepingLine___Win32_Debug_Unicode_Rus"
# PROP BASE Intermediate_Dir "CreepingLine___Win32_Debug_Unicode_Rus"
# PROP BASE Ignore_Export_Lib 0
# PROP BASE Target_Dir ""
# PROP Use_MFC 6
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "..\~Binaries\Debug"
# PROP Intermediate_Dir "..\~Intermediate\Debug\CreepingLine"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MDd /W3 /Gm /GX /ZI /Od /I "../Libs/appweb" /I "../Libs/appweb/include" /I "../Inc" /I "../Libs" /I "../Presenter.Engine" /D "_DEBUG" /D "WIN" /D "WIN32" /D "_WINDOWS" /D "_AFXDLL" /D "_UNICODE" /D "UNICODE" /D "CREEPING_LINE_PRJ" /FR /Yu"stdafx.h" /FD /GZ /c
# ADD CPP /nologo /MDd /W3 /Gm /GX /ZI /Od /I "../Libs/appweb" /I "../Libs/appweb/include" /I "../Inc" /I "../Libs" /I "../Presenter.Engine" /I "../CustomResolution" /D "_DEBUG" /D "WIN" /D "WIN32" /D "_WINDOWS" /D "_AFXDLL" /D "_UNICODE" /D "UNICODE" /D "CREEPING_LINE_PRJ" /D "LANG_INTERFACE_RUS" /FR /Yu"stdafx.h" /FD /GZ /c
# ADD BASE MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x409 /d "_DEBUG" /d "_AFXDLL"
# ADD RSC /l 0x409 /d "_DEBUG" /d "_AFXDLL" /d "LANG_INTERFACE_RUS"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 cppunit_dll.lib /nologo /entry:"wWinMainCRTStartup" /subsystem:windows /debug /machine:I386 /pdbtype:sept /libpath:"..\~Binaries\Debug"
# SUBTRACT BASE LINK32 /nodefaultlib
# ADD LINK32 cppunit_dll.lib CustomResolution.lib /nologo /entry:"wWinMainCRTStartup" /subsystem:windows /debug /machine:I386 /pdbtype:sept /libpath:"..\~Binaries\Debug"
# SUBTRACT LINK32 /nodefaultlib
# Begin Special Build Tool
TargetPath=\Work\APPLICA\sources\Presenter\~Binaries\Debug\CreepingLine.exe
SOURCE="$(InputPath)"
PostBuild_Desc=Run tests...
PostBuild_Cmds="$(TargetPath)" /Test
# End Special Build Tool

!ELSEIF  "$(CFG)" == "CreepingLine - Win32 Release Unicode Rus"

# PROP BASE Use_MFC 6
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "CreepingLine___Win32_Release_Unicode_Rus"
# PROP BASE Intermediate_Dir "CreepingLine___Win32_Release_Unicode_Rus"
# PROP BASE Ignore_Export_Lib 0
# PROP BASE Target_Dir ""
# PROP Use_MFC 6
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "..\~Binaries\ReleaseLib"
# PROP Intermediate_Dir "..\~Intermediate\Release\CreepingLine"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MD /W3 /GX /O2 /I "../Libs/appweb" /I "../Libs/appweb/include" /I "../Inc" /I "../Libs" /I "../Presenter.Engine" /D "NDEBUG" /D "WIN" /D "WIN32" /D "_WINDOWS" /D "_AFXDLL" /D "_UNICODE" /D "UNICODE" /D "CREEPING_LINE_PRJ" /FR /Yu"stdafx.h" /FD /c
# ADD CPP /nologo /MD /W3 /GX /O2 /I "../Libs/appweb" /I "../Libs/appweb/include" /I "../Inc" /I "../Libs" /I "../Presenter.Engine" /I "../CustomResolution" /D "NDEBUG" /D "WIN" /D "WIN32" /D "_WINDOWS" /D "_AFXDLL" /D "_UNICODE" /D "UNICODE" /D "CREEPING_LINE_PRJ" /D "LANG_INTERFACE_RUS" /FR /Yu"stdafx.h" /FD /c
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x409 /d "NDEBUG" /d "_AFXDLL"
# ADD RSC /l 0x409 /d "NDEBUG" /d "_AFXDLL" /d "LANG_INTERFACE_RUS"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 /nologo /entry:"wWinMainCRTStartup" /subsystem:windows /machine:I386
# ADD LINK32 CustomResolution.lib /nologo /entry:"wWinMainCRTStartup" /subsystem:windows /machine:I386 /libpath:"..\~Binaries\~Release Unicode"

!ENDIF 

# Begin Target

# Name "CreepingLine - Win32 Debug Unicode"
# Name "CreepingLine - Win32 Release Unicode"
# Name "CreepingLine - Win32 Debug Unicode Rus"
# Name "CreepingLine - Win32 Release Unicode Rus"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Source File

SOURCE=.\BitmapBuf.cpp
# End Source File
# Begin Source File

SOURCE=..\Libs\CommandLineParser.cpp
# End Source File
# Begin Source File

SOURCE=.\CreepingLine.cpp
# End Source File
# Begin Source File

SOURCE=.\CreepingLineDoc.cpp
# End Source File
# Begin Source File

SOURCE=.\CreepingLineView.cpp
# End Source File
# Begin Source File

SOURCE=.\CreepingLineWnd.cpp
# End Source File
# Begin Source File

SOURCE=.\html2txt.cpp
# End Source File
# Begin Source File

SOURCE=.\LinePropertyPageFont.cpp
# End Source File
# Begin Source File

SOURCE=.\LinePropertyPageGeneral.cpp
# End Source File
# Begin Source File

SOURCE=.\LinePropertyPagePosition.cpp
# End Source File
# Begin Source File

SOURCE=.\LinePropertyPageWindow.cpp
# End Source File
# Begin Source File

SOURCE=.\LinePropertySheet.cpp
# End Source File
# Begin Source File

SOURCE=..\Libs\log.cpp
# End Source File
# Begin Source File

SOURCE=.\MainFrm.cpp
# End Source File
# Begin Source File

SOURCE=..\Libs\MMTimer.cpp
# End Source File
# Begin Source File

SOURCE=.\MyTracker.cpp
# End Source File
# Begin Source File

SOURCE=.\rss.cpp
# End Source File
# Begin Source File

SOURCE=..\Libs\RunTests.cpp
# End Source File
# Begin Source File

SOURCE=.\StdAfx.cpp
# ADD CPP /Yc"stdafx.h"
# End Source File
# Begin Source File

SOURCE=..\Libs\Thread.cpp
# End Source File
# Begin Source File

SOURCE=.\WindowPositionControl.cpp
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl"
# Begin Source File

SOURCE=.\BitmapBuf.h
# End Source File
# Begin Source File

SOURCE=..\Libs\CommandLineParser.h
# End Source File
# Begin Source File

SOURCE=.\CreepingLine.h
# End Source File
# Begin Source File

SOURCE=.\CreepingLineDoc.h
# End Source File
# Begin Source File

SOURCE=.\CreepingLineView.h
# End Source File
# Begin Source File

SOURCE=.\CreepingLineWnd.h
# End Source File
# Begin Source File

SOURCE=.\html2txt.h
# End Source File
# Begin Source File

SOURCE=.\LinePropertyPageFont.h
# End Source File
# Begin Source File

SOURCE=.\LinePropertyPageGeneral.h
# End Source File
# Begin Source File

SOURCE=.\LinePropertyPagePosition.h
# End Source File
# Begin Source File

SOURCE=.\LinePropertyPageWindow.h
# End Source File
# Begin Source File

SOURCE=.\LinePropertySheet.h
# End Source File
# Begin Source File

SOURCE=.\MainFrm.h
# End Source File
# Begin Source File

SOURCE=..\Libs\MMTimer.h
# End Source File
# Begin Source File

SOURCE=.\MyTracker.h
# End Source File
# Begin Source File

SOURCE=.\rss.h
# End Source File
# Begin Source File

SOURCE=.\StdAfx.h
# End Source File
# Begin Source File

SOURCE=.\WindowPositionControl.h
# End Source File
# End Group
# Begin Group "Resource Files"

# PROP Default_Filter "ico;cur;bmp;dlg;rc2;rct;bin;rgs;gif;jpg;jpeg;jpe"
# Begin Group "Other"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\res\CreepingLine.ico
# End Source File
# Begin Source File

SOURCE=.\res\CreepingLine.rc2
# End Source File
# Begin Source File

SOURCE=.\res\CreepingLineDoc.ico
# End Source File
# Begin Source File

SOURCE=.\res\Toolbar.bmp
# End Source File
# End Group
# Begin Source File

SOURCE=".\CreepingLine-eng.rc"

!IF  "$(CFG)" == "CreepingLine - Win32 Debug Unicode"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "CreepingLine - Win32 Release Unicode"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "CreepingLine - Win32 Debug Unicode Rus"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "CreepingLine - Win32 Release Unicode Rus"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=".\CreepingLine-rus.rc"

!IF  "$(CFG)" == "CreepingLine - Win32 Debug Unicode"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "CreepingLine - Win32 Release Unicode"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "CreepingLine - Win32 Debug Unicode Rus"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "CreepingLine - Win32 Release Unicode Rus"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\CreepingLine.rc
# End Source File
# Begin Source File

SOURCE=.\Resource.h
# End Source File
# End Group
# Begin Group "Libs"

# PROP Default_Filter ""
# Begin Group "Lua"

# PROP Default_Filter ""
# Begin Group "lib"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\Libs\Lua\lauxlib.c
# SUBTRACT CPP /YX /Yc /Yu
# End Source File
# Begin Source File

SOURCE=..\Libs\Lua\lbaselib.c
# SUBTRACT CPP /YX /Yc /Yu
# End Source File
# Begin Source File

SOURCE=..\Libs\Lua\ldblib.c
# SUBTRACT CPP /YX /Yc /Yu
# End Source File
# Begin Source File

SOURCE=..\Libs\Lua\liolib.c
# SUBTRACT CPP /YX /Yc /Yu
# End Source File
# Begin Source File

SOURCE=..\Libs\Lua\lmathlib.c
# SUBTRACT CPP /YX /Yc /Yu
# End Source File
# Begin Source File

SOURCE=..\Libs\Lua\loadlib.c
# SUBTRACT CPP /YX /Yc /Yu
# End Source File
# Begin Source File

SOURCE=..\Libs\Lua\lstrlib.c
# SUBTRACT CPP /YX /Yc /Yu
# End Source File
# Begin Source File

SOURCE=..\Libs\Lua\ltablib.c
# SUBTRACT CPP /YX /Yc /Yu
# End Source File
# End Group
# Begin Source File

SOURCE=..\Libs\Lua\lapi.c
# SUBTRACT CPP /YX /Yc /Yu
# End Source File
# Begin Source File

SOURCE=..\Libs\Lua\lapi.h
# End Source File
# Begin Source File

SOURCE=..\Libs\Lua\lauxlib.h
# End Source File
# Begin Source File

SOURCE=..\Libs\Lua\lcode.c
# SUBTRACT CPP /YX /Yc /Yu
# End Source File
# Begin Source File

SOURCE=..\Libs\Lua\lcode.h
# End Source File
# Begin Source File

SOURCE=..\Libs\Lua\ldebug.c
# SUBTRACT CPP /YX /Yc /Yu
# End Source File
# Begin Source File

SOURCE=..\Libs\Lua\ldebug.h
# End Source File
# Begin Source File

SOURCE=..\Libs\Lua\ldo.c
# SUBTRACT CPP /YX /Yc /Yu
# End Source File
# Begin Source File

SOURCE=..\Libs\Lua\ldo.h
# End Source File
# Begin Source File

SOURCE=..\Libs\Lua\ldump.c
# SUBTRACT CPP /YX /Yc /Yu
# End Source File
# Begin Source File

SOURCE=..\Libs\Lua\lfunc.c
# SUBTRACT CPP /YX /Yc /Yu
# End Source File
# Begin Source File

SOURCE=..\Libs\Lua\lfunc.h
# End Source File
# Begin Source File

SOURCE=..\Libs\Lua\lgc.c
# SUBTRACT CPP /YX /Yc /Yu
# End Source File
# Begin Source File

SOURCE=..\Libs\Lua\lgc.h
# End Source File
# Begin Source File

SOURCE=..\Libs\Lua\linit.c
# SUBTRACT CPP /YX /Yc /Yu
# End Source File
# Begin Source File

SOURCE=..\Libs\Lua\llex.c
# SUBTRACT CPP /YX /Yc /Yu
# End Source File
# Begin Source File

SOURCE=..\Libs\Lua\llex.h
# End Source File
# Begin Source File

SOURCE=..\Libs\Lua\llimits.h
# End Source File
# Begin Source File

SOURCE=..\Libs\Lua\lmem.c
# SUBTRACT CPP /YX /Yc /Yu
# End Source File
# Begin Source File

SOURCE=..\Libs\Lua\lmem.h
# End Source File
# Begin Source File

SOURCE=..\Libs\Lua\lobject.c
# SUBTRACT CPP /YX /Yc /Yu
# End Source File
# Begin Source File

SOURCE=..\Libs\Lua\lobject.h
# End Source File
# Begin Source File

SOURCE=..\Libs\Lua\lopcodes.c
# SUBTRACT CPP /YX /Yc /Yu
# End Source File
# Begin Source File

SOURCE=..\Libs\Lua\lopcodes.h
# End Source File
# Begin Source File

SOURCE=..\Libs\Lua\loslib.c
# SUBTRACT CPP /YX /Yc /Yu
# End Source File
# Begin Source File

SOURCE=..\Libs\Lua\lparser.c
# SUBTRACT CPP /YX /Yc /Yu
# End Source File
# Begin Source File

SOURCE=..\Libs\Lua\lparser.h
# End Source File
# Begin Source File

SOURCE=..\Libs\Lua\lstate.c
# SUBTRACT CPP /YX /Yc /Yu
# End Source File
# Begin Source File

SOURCE=..\Libs\Lua\lstate.h
# End Source File
# Begin Source File

SOURCE=..\Libs\Lua\lstring.c
# SUBTRACT CPP /YX /Yc /Yu
# End Source File
# Begin Source File

SOURCE=..\Libs\Lua\lstring.h
# End Source File
# Begin Source File

SOURCE=..\Libs\Lua\ltable.c
# SUBTRACT CPP /YX /Yc /Yu
# End Source File
# Begin Source File

SOURCE=..\Libs\Lua\ltable.h
# End Source File
# Begin Source File

SOURCE=..\Libs\Lua\ltm.c
# SUBTRACT CPP /YX /Yc /Yu
# End Source File
# Begin Source File

SOURCE=..\Libs\Lua\ltm.h
# End Source File
# Begin Source File

SOURCE=..\Libs\Lua\lua.h
# End Source File
# Begin Source File

SOURCE=..\Libs\Lua\luac.c
# SUBTRACT CPP /YX /Yc /Yu
# End Source File
# Begin Source File

SOURCE=..\Libs\Lua\lualib.h
# End Source File
# Begin Source File

SOURCE=..\Libs\Lua\lundump.c
# SUBTRACT CPP /YX /Yc /Yu
# End Source File
# Begin Source File

SOURCE=..\Libs\Lua\lundump.h
# End Source File
# Begin Source File

SOURCE=..\Libs\Lua\lvm.c
# SUBTRACT CPP /YX /Yc /Yu
# End Source File
# Begin Source File

SOURCE=..\Libs\Lua\lvm.h
# End Source File
# Begin Source File

SOURCE=..\Libs\Lua\lzio.c
# SUBTRACT CPP /YX /Yc /Yu
# End Source File
# Begin Source File

SOURCE=..\Libs\Lua\lzio.h
# End Source File
# Begin Source File

SOURCE=..\Libs\Lua\print.c
# SUBTRACT CPP /YX /Yc /Yu
# End Source File
# End Group
# Begin Source File

SOURCE=..\Libs\BitmapEx.cpp
# End Source File
# Begin Source File

SOURCE=..\Libs\BitmapEx.h
# End Source File
# Begin Source File

SOURCE=..\Libs\CLua.cpp
# End Source File
# Begin Source File

SOURCE=..\Libs\ColorBox.cpp
# End Source File
# Begin Source File

SOURCE=..\Libs\ColorBox.h
# End Source File
# Begin Source File

SOURCE=..\Libs\CreepingLineInfo.cpp
# End Source File
# Begin Source File

SOURCE=..\Libs\CreepingLineInfo.h
# End Source File
# Begin Source File

SOURCE=..\Libs\FileOperation.cpp
# End Source File
# Begin Source File

SOURCE=..\Libs\FileOperation.h
# End Source File
# Begin Source File

SOURCE=..\Libs\MakeWindowTransparent.cpp
# End Source File
# Begin Source File

SOURCE=..\Libs\MakeWindowTransparent.h
# End Source File
# Begin Source File

SOURCE=..\Libs\md5.c
# SUBTRACT CPP /YX /Yc /Yu
# End Source File
# Begin Source File

SOURCE=..\Libs\md5.h
# End Source File
# Begin Source File

SOURCE=..\Libs\MonCustomRes.cpp
# End Source File
# Begin Source File

SOURCE=..\Libs\MonCustomRes.h
# End Source File
# Begin Source File

SOURCE=..\Libs\MonResDatas.cpp
# End Source File
# Begin Source File

SOURCE=..\Libs\MonResDatas.h
# End Source File
# Begin Source File

SOURCE=..\Libs\SerializeStruct.cpp
# End Source File
# Begin Source File

SOURCE=..\Libs\SerializeStruct.h
# End Source File
# Begin Source File

SOURCE=..\Libs\Translate.cpp
# End Source File
# Begin Source File

SOURCE=..\Libs\Translate.h
# End Source File
# Begin Source File

SOURCE=..\Libs\WebGrab.cpp
# End Source File
# Begin Source File

SOURCE=..\Libs\WebGrab.h
# End Source File
# End Group
# Begin Group "Inc"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\Inc\CommonApi.h
# End Source File
# End Group
# Begin Group "Presenter"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\Presenter\PresenterRegistry.h
# End Source File
# End Group
# Begin Group "ConfigurationWizard"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\ConfigurationWizard\ConfigurationWizardExt.h
# End Source File
# End Group
# Begin Source File

SOURCE=.\CreepingLine.reg
# End Source File
# End Target
# End Project
