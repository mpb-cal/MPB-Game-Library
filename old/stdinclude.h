#ifndef __STDINCLUDE_H__
#define __STDINCLUDE_H__

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#define WIN32_LEAN_AND_MEAN		// Exclude rarely-used stuff from Windows headers


// Windows Header Files:
#include <windows.h>

#pragma warning (disable:4786)

// C RunTime Header Files
#include <stdlib.h>
#include <stdio.h>
#include <malloc.h>
#include <memory.h>
#include <tchar.h>
#include <gl\gl.h>
#include <gl\glu.h>
#include <gl\glaux.h>
#include <sys/stat.h>
#include <sys/types.h>

#ifndef DIRECTINPUT_VERSION
#define DIRECTINPUT_VERSION         0x0800
#endif
#include <dinput.h>

#include <mmsystem.h>
#include <dsound.h>
#include <math.h>
#include <vector>
#include <set>
#include <map>
#include <stack>
#include <queue>
#include <list>
#include <functional>
#include <algorithm>
#include <winsock2.h>
#include <CRTDBG.H>

using namespace std;

#endif

