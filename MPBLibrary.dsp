# Microsoft Developer Studio Project File - Name="MPBLibrary" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Static Library" 0x0104

CFG=MPBLibrary - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "MPBLibrary.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "MPBLibrary.mak" CFG="MPBLibrary - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "MPBLibrary - Win32 Release" (based on "Win32 (x86) Static Library")
!MESSAGE "MPBLibrary - Win32 Debug" (based on "Win32 (x86) Static Library")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
RSC=rc.exe

!IF  "$(CFG)" == "MPBLibrary - Win32 Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Release"
# PROP BASE Intermediate_Dir "Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "Release"
# PROP Intermediate_Dir "Release"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_MBCS" /D "_LIB" /YX /FD /c
# ADD CPP /nologo /MD /W3 /GR /GX /O2 /D "WIN32" /D "NDEBUG" /D "_MBCS" /D "_LIB" /FD /c
# SUBTRACT CPP /YX
# ADD BASE RSC /l 0x409 /d "NDEBUG"
# ADD RSC /l 0x409 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo

!ELSEIF  "$(CFG)" == "MPBLibrary - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "MPBLibrary___Win32_Debug"
# PROP BASE Intermediate_Dir "MPBLibrary___Win32_Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "Debug"
# PROP Intermediate_Dir "Debug"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_MBCS" /D "_LIB" /YX /FD /GZ /c
# ADD CPP /nologo /MDd /W3 /Gm /GR /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_MBCS" /D "_LIB" /FR /FD /GZ /c
# SUBTRACT CPP /YX
# ADD BASE RSC /l 0x409 /d "_DEBUG"
# ADD RSC /l 0x409 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo /out:"Debug\MPBLibraryd.lib"

!ENDIF 

# Begin Target

# Name "MPBLibrary - Win32 Release"
# Name "MPBLibrary - Win32 Debug"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Source File

SOURCE=.\mmgr.cpp
# End Source File
# Begin Source File

SOURCE=.\MPBAABB.cpp
# End Source File
# Begin Source File

SOURCE=.\MPBAnimation.cpp
# End Source File
# Begin Source File

SOURCE=.\MPBCollision.cpp
# End Source File
# Begin Source File

SOURCE=.\MPBCompositeShape.cpp
# End Source File
# Begin Source File

SOURCE=.\MPBCompositeShapeDoorWall.cpp
# End Source File
# Begin Source File

SOURCE=.\MPBCompositeShapeRoom.cpp
# End Source File
# Begin Source File

SOURCE=.\MPBCompositeShapeStairs.cpp
# End Source File
# Begin Source File

SOURCE=.\MPBCompositeShapeTube.cpp
# End Source File
# Begin Source File

SOURCE=.\MPBCompositeShapeWindowQuad.cpp
# End Source File
# Begin Source File

SOURCE=.\MPBConsoleWindow.cpp
# End Source File
# Begin Source File

SOURCE=.\MPBCountdown.cpp
# End Source File
# Begin Source File

SOURCE=.\MPBDialogBox.cpp
# End Source File
# Begin Source File

SOURCE=.\MPBDirectInput.cpp
# End Source File
# Begin Source File

SOURCE=.\MPBDirectPlay.cpp
# End Source File
# Begin Source File

SOURCE=.\MPBFrameCounter.cpp
# End Source File
# Begin Source File

SOURCE=.\MPBGameEntities.cpp
# End Source File
# Begin Source File

SOURCE=.\MPBGeometry.cpp
# End Source File
# Begin Source File

SOURCE=.\MPBGlobal.cpp
# End Source File
# Begin Source File

SOURCE=.\MPBHeightmap.cpp
# End Source File
# Begin Source File

SOURCE=.\MPBLibrary.cpp
# End Source File
# Begin Source File

SOURCE=.\MPBLine.cpp
# End Source File
# Begin Source File

SOURCE=.\MPBList.cpp
# End Source File
# Begin Source File

SOURCE=.\MPBMatrix.cpp
# End Source File
# Begin Source File

SOURCE=.\MPBMessageWindow.cpp
# End Source File
# Begin Source File

SOURCE=.\MPBOpenGL.cpp
# End Source File
# Begin Source File

SOURCE=.\MPBParticle.cpp
# End Source File
# Begin Source File

SOURCE=.\MPBParticleSystem.cpp
# End Source File
# Begin Source File

SOURCE=.\MPBPartitionTreeNode.cpp
# End Source File
# Begin Source File

SOURCE=.\MPBPerformanceMonitor.cpp
# End Source File
# Begin Source File

SOURCE=.\MPBPerlinNoise.cpp
# End Source File
# Begin Source File

SOURCE=.\MPBPhysics.cpp
# End Source File
# Begin Source File

SOURCE=.\MPBPhysicsObject.cpp
# End Source File
# Begin Source File

SOURCE=.\MPBPlane.cpp
# End Source File
# Begin Source File

SOURCE=.\MPBQuaternion.cpp
# End Source File
# Begin Source File

SOURCE=.\MPBShadow.cpp
# End Source File
# Begin Source File

SOURCE=.\MPBShape.cpp
# End Source File
# Begin Source File

SOURCE=.\MPBShapeBall.cpp
# End Source File
# Begin Source File

SOURCE=.\MPBShapeBone.cpp
# End Source File
# Begin Source File

SOURCE=.\MPBShapeBox.cpp
# End Source File
# Begin Source File

SOURCE=.\MPBShapeExtrusion.cpp
# End Source File
# Begin Source File

SOURCE=.\MPBShapeFrustum.cpp
# End Source File
# Begin Source File

SOURCE=.\MPBShapeQuad.cpp
# End Source File
# Begin Source File

SOURCE=.\MPBShapeTube.cpp
# End Source File
# Begin Source File

SOURCE=.\MPBSound.cpp
# End Source File
# Begin Source File

SOURCE=.\MPBSoundManager.cpp
# End Source File
# Begin Source File

SOURCE=.\MPBStreamingSound.cpp
# End Source File
# Begin Source File

SOURCE=.\MPBTexture.cpp
# End Source File
# Begin Source File

SOURCE=.\MPBTexturedPlane.cpp
# End Source File
# Begin Source File

SOURCE=.\MPBTextureGenerator.cpp
# End Source File
# Begin Source File

SOURCE=.\MPBTriangle.cpp
# End Source File
# Begin Source File

SOURCE=.\MPBTriangleBinaryTreeNode.cpp
# End Source File
# Begin Source File

SOURCE=.\MPBTriStrip.cpp
# End Source File
# Begin Source File

SOURCE=.\MPBVector.cpp
# End Source File
# Begin Source File

SOURCE=.\MPBVectorList.cpp
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl"
# Begin Source File

SOURCE=.\mmgr.h
# End Source File
# Begin Source File

SOURCE=.\MPBAABB.h
# End Source File
# Begin Source File

SOURCE=.\MPBAnimation.h
# End Source File
# Begin Source File

SOURCE=.\MPBCollision.h
# End Source File
# Begin Source File

SOURCE=.\MPBCompositeShape.h
# End Source File
# Begin Source File

SOURCE=.\MPBCompositeShapeDoorWall.h
# End Source File
# Begin Source File

SOURCE=.\MPBCompositeShapeRoom.h
# End Source File
# Begin Source File

SOURCE=.\MPBCompositeShapeStairs.h
# End Source File
# Begin Source File

SOURCE=.\MPBCompositeShapeTube.h
# End Source File
# Begin Source File

SOURCE=.\MPBCompositeShapeWindowQuad.h
# End Source File
# Begin Source File

SOURCE=.\MPBConsoleWindow.h
# End Source File
# Begin Source File

SOURCE=.\MPBControlPanel.h
# End Source File
# Begin Source File

SOURCE=.\MPBCountdown.h
# End Source File
# Begin Source File

SOURCE=.\MPBDialogBox.h
# End Source File
# Begin Source File

SOURCE=.\MPBDirectInput.h
# End Source File
# Begin Source File

SOURCE=.\MPBDirectPlay.h
# End Source File
# Begin Source File

SOURCE=.\MPBFrameCounter.h
# End Source File
# Begin Source File

SOURCE=.\MPBGameEntities.h
# End Source File
# Begin Source File

SOURCE=.\MPBGeometry.h
# End Source File
# Begin Source File

SOURCE=.\MPBGlobal.h
# End Source File
# Begin Source File

SOURCE=.\MPBHeightmap.h
# End Source File
# Begin Source File

SOURCE=.\MPBLibrary.h
# End Source File
# Begin Source File

SOURCE=.\MPBLine.h
# End Source File
# Begin Source File

SOURCE=.\MPBList.h
# End Source File
# Begin Source File

SOURCE=.\MPBMatrix.h
# End Source File
# Begin Source File

SOURCE=.\MPBMessageWindow.h
# End Source File
# Begin Source File

SOURCE=.\MPBOpenGL.h
# End Source File
# Begin Source File

SOURCE=.\MPBParticle.h
# End Source File
# Begin Source File

SOURCE=.\MPBParticleSystem.h
# End Source File
# Begin Source File

SOURCE=.\MPBPartitionTreeNode.h
# End Source File
# Begin Source File

SOURCE=.\MPBPerformanceMonitor.h
# End Source File
# Begin Source File

SOURCE=.\MPBPerlinNoise.h
# End Source File
# Begin Source File

SOURCE=.\MPBPhysics.h
# End Source File
# Begin Source File

SOURCE=.\MPBPhysicsObject.h
# End Source File
# Begin Source File

SOURCE=.\MPBPlane.h
# End Source File
# Begin Source File

SOURCE=.\MPBQuaternion.h
# End Source File
# Begin Source File

SOURCE=.\MPBShadow.h
# End Source File
# Begin Source File

SOURCE=.\MPBShape.h
# End Source File
# Begin Source File

SOURCE=.\MPBShapeBall.h
# End Source File
# Begin Source File

SOURCE=.\MPBShapeBone.h
# End Source File
# Begin Source File

SOURCE=.\MPBShapeBox.h
# End Source File
# Begin Source File

SOURCE=.\MPBShapeExtrusion.h
# End Source File
# Begin Source File

SOURCE=.\MPBShapeFrustum.h
# End Source File
# Begin Source File

SOURCE=.\MPBShapeQuad.h
# End Source File
# Begin Source File

SOURCE=.\MPBShapeTube.h
# End Source File
# Begin Source File

SOURCE=.\MPBSound.h
# End Source File
# Begin Source File

SOURCE=.\MPBSoundManager.h
# End Source File
# Begin Source File

SOURCE=.\MPBStreamingSound.h
# End Source File
# Begin Source File

SOURCE=.\MPBStreamingSounds.h
# End Source File
# Begin Source File

SOURCE=.\MPBTexture.h
# End Source File
# Begin Source File

SOURCE=.\MPBTexturedPlane.h
# End Source File
# Begin Source File

SOURCE=.\MPBTextureGenerator.h
# End Source File
# Begin Source File

SOURCE=.\MPBTriangle.h
# End Source File
# Begin Source File

SOURCE=.\MPBTriangleBinaryTreeNode.h
# End Source File
# Begin Source File

SOURCE=.\MPBTriStrip.h
# End Source File
# Begin Source File

SOURCE=.\MPBVector.h
# End Source File
# Begin Source File

SOURCE=.\MPBVectorList.h
# End Source File
# Begin Source File

SOURCE=.\nommgr.h
# End Source File
# End Group
# Begin Group "PortAudio"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\PortAudio\Circuit_Helicopter.cpp
# End Source File
# Begin Source File

SOURCE=.\PortAudio\Circuit_Rain.cpp
# End Source File
# Begin Source File

SOURCE=.\PortAudio\Circuit_RainPlops.cpp
# End Source File
# Begin Source File

SOURCE=.\PortAudio\Circuit_Reverb.cpp
# End Source File
# Begin Source File

SOURCE=.\PortAudio\Circuit_Rocket.cpp
# End Source File
# Begin Source File

SOURCE=.\PortAudio\Circuit_SonarPing.cpp
# End Source File
# Begin Source File

SOURCE=.\PortAudio\Circuit_Wind.cpp
# End Source File
# Begin Source File

SOURCE=.\PortAudio\pa_lib.c
# End Source File
# Begin Source File

SOURCE=.\PortAudio\pa_trace.c
# End Source File
# Begin Source File

SOURCE=.\PortAudio\pa_win_wmme.c
# End Source File
# Begin Source File

SOURCE=.\PortAudio\PortAudioDevice.cpp
# End Source File
# Begin Source File

SOURCE=.\PortAudio\Unit_Add.cpp
# End Source File
# Begin Source File

SOURCE=.\PortAudio\Unit_AllpassDelay.cpp
# End Source File
# Begin Source File

SOURCE=.\PortAudio\Unit_BandpassFilter.cpp
# End Source File
# Begin Source File

SOURCE=.\PortAudio\Unit_BiquadFilter.cpp
# End Source File
# Begin Source File

SOURCE=.\PortAudio\Unit_CombFilter.cpp
# End Source File
# Begin Source File

SOURCE=.\PortAudio\Unit_Delay.cpp
# End Source File
# Begin Source File

SOURCE=.\PortAudio\Unit_Filter.cpp
# End Source File
# Begin Source File

SOURCE=.\PortAudio\Unit_HighpassFilter.cpp
# End Source File
# Begin Source File

SOURCE=.\PortAudio\Unit_ImpulseOscillator.cpp
# End Source File
# Begin Source File

SOURCE=.\PortAudio\Unit_Integrator.cpp
# End Source File
# Begin Source File

SOURCE=.\PortAudio\Unit_InterpolatingDelay.cpp
# End Source File
# Begin Source File

SOURCE=.\PortAudio\Unit_LowpassCombFilter.cpp
# End Source File
# Begin Source File

SOURCE=.\PortAudio\Unit_LowPassFilter.cpp
# End Source File
# Begin Source File

SOURCE=.\PortAudio\Unit_MixingBus.cpp
# End Source File
# Begin Source File

SOURCE=.\PortAudio\Unit_Multiply.cpp
# End Source File
# Begin Source File

SOURCE=.\PortAudio\Unit_Oscillator.cpp
# End Source File
# Begin Source File

SOURCE=.\PortAudio\Unit_PoissonTrigger.cpp
# End Source File
# Begin Source File

SOURCE=.\PortAudio\Unit_RedNoise.cpp
# End Source File
# Begin Source File

SOURCE=.\PortAudio\Unit_SineOscillator.cpp
# End Source File
# Begin Source File

SOURCE=.\PortAudio\Unit_SlewRateLimiter.cpp
# End Source File
# Begin Source File

SOURCE=.\PortAudio\Unit_WhiteNoise.cpp
# End Source File
# Begin Source File

SOURCE=.\PortAudio\UnitGen.cpp
# End Source File
# End Group
# End Target
# End Project
