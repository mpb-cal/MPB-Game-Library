#ifndef __MPBLIBRARY_H__
#define __MPBLIBRARY_H__


//#if _MSC_VER > 1000
//#pragma once
//#endif // _MSC_VER > 1000

//#define WIN32_LEAN_AND_MEAN		// Exclude rarely-used stuff from Windows headers

#define MPBFLOAT float

// Windows Header Files:
#ifdef WIN32
#include <windows.h>
#include <windef.h>
#include <winbase.h>
//#include <shlwapi.h>

#pragma warning (disable:4786)	// debugger long symbol names
#pragma warning (disable:4244)	// conversion from 'double' to 'float', possible loss of data
#pragma warning (disable:4305)	// truncation from 'const double' to 'float'
#endif

// unix and windows
#include <stdlib.h>
#include <stdio.h>
#include <malloc.h>
#include <memory.h>
#include <math.h>
#include <sys/stat.h>
#include <sys/types.h>
//#include <map>
//#include <stack>
//#include <queue>
//#include <list>
//#include <functional>
//#include <typeinfo>
//#include <algorithm>

// windows only
#ifdef WIN32


#include <tchar.h>
#include <gl\gl.h>
#include <gl\glu.h>
#include <gl\glext.h>
//#include <gl\glaux.h>

//#ifndef DIRECTINPUT_VERSION
//#define DIRECTINPUT_VERSION         0x0800
//#endif

#include <dinput.h>
#include <dsound.h>
#include <ddraw.h>
#include <dplay8.h>
#include <dshow.h>

#include <mmsystem.h>
//#include <winsock2.h>
#include <commctrl.h>
#include <commdlg.h>
#include <CRTDBG.H>


//#ifdef _DEBUG
//#include "mmgr.h"		// put this last in cpp files
//#else
//#include "nommgr.h"
//#endif


#else

// unix only
#include <unistd.h>
#include <pthread.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <stdlib.h>
#include <stdio.h>
#include <malloc.h>
#include <memory.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/time.h>
#include <fcntl.h>
#include <signal.h>

#endif

#include "MPBVector.h"

class MPBList;

//using namespace std;

// windows stuff for unix
#ifndef WIN32

typedef long       			LONG;
typedef unsigned long      DWORD;
typedef unsigned short     WORD;
typedef unsigned char     BYTE;

typedef struct tagRGBTRIPLE {
        BYTE    rgbtBlue;
        BYTE    rgbtGreen;
        BYTE    rgbtRed;
} RGBTRIPLE;

typedef struct tagRGBQUAD {
        BYTE    rgbBlue;
        BYTE    rgbGreen;
        BYTE    rgbRed;
        BYTE    rgbReserved;
} RGBQUAD;

typedef struct tagBITMAPINFOHEADER{
        DWORD      biSize;
        LONG       biWidth;
        LONG       biHeight;
        WORD       biPlanes;
        WORD       biBitCount;
        DWORD      biCompression;
        DWORD      biSizeImage;
        LONG       biXPelsPerMeter;
        LONG       biYPelsPerMeter;
        DWORD      biClrUsed;
        DWORD      biClrImportant;
} BITMAPINFOHEADER;

typedef struct tagBITMAPFILEHEADER {
        WORD    bfType;
        DWORD   bfSize;
        WORD    bfReserved1;
        WORD    bfReserved2;
        DWORD   bfOffBits;
} BITMAPFILEHEADER;

#define BI_RGB        0L
#define BI_RLE8       1L
#define BI_RLE4       2L
#define BI_BITFIELDS  3L

#endif


#ifndef WIN32
#define BYTE unsigned char
#define _ASSERTE(x) if (!(x)) { printf( "Assertion failed: %s (%d)\n", __FILE__, __LINE__ ); exit( -1 ); }
#endif

#ifdef _DEBUG
#define MPB_ASSERT(expr) \
	if (!(expr))\
	{\
		_ASSERTE( (expr) );\
	}
#else
#define MPB_ASSERT(expr) \
	if (!(expr))\
	{\
		sprintf( g_logBuf, "Assertion failed at %s (%d)", __FILE__, __LINE__  );\
		MessageBox( g_hwnd, g_logBuf, "MPB Error", MB_OK );\
		exit( -1 );\
	}
#endif

//#ifndef MSVC
//#define _ASSERTE(x) if ((x) == false) { asm( "int3" ); }
//#endif

const MPBFLOAT EPSILON = .001f;
#ifndef PI
const MPBFLOAT PI = 3.1415926535f;
#endif
const MPBFLOAT PI_OVER_180 = (PI / 180.0f);
const MPBFLOAT _180_OVER_PI = (180.0f / PI);
const MPBFLOAT PI_OVER_2 = (PI / 2.0f);
const MPBFLOAT _3PI_OVER_2 = (3.0f * PI / 2.0f);
const MPBFLOAT _2PI = (PI * 2.0f);

extern int							g_breakFlag;
#define BREAK_F4					_ASSERTE( !g_breakFlag ); g_breakFlag = 0;
// put this somewhere in your game loop:
#define ENABLE_BREAK_F4			if (GetAsyncKeyState( VK_F4 ) & 0x8000)\
										{\
											g_breakFlag = 1;\
										}

#define NULLIFY( x ) { if (x) delete x; x = NULL; }
#define NULLIFY_ARRAY( x ) { if (x) delete[] x; x = NULL; }

//#define VECTOR_LOOP( v, t ) {for (vector<t >::iterator itr = v.begin(); itr != v.end(); itr++)
//#define VECTOR_LOOP_CONST( v, t ) {for (vector<t >::const_iterator itr = v.begin(); itr != v.end(); itr++)
//#define SET_LOOP( v, t ) {for (set<t >::iterator itr = v.begin(); itr != v.end(); itr++)
//#define SET_LOOP_CONST( v, t ) {for (set<t >::const_iterator itr = v.begin(); itr != v.end(); itr++)
#define MAP_LOOP( v, key, value ) {for (map<key,value >::iterator itr = v.begin(); itr != v.end(); itr++)
#define MAP_LOOP_CONST( v, key, value ) {for (map<key,value >::const_iterator itr = v.begin(); itr != v.end(); itr++)
#define LIST_LOOP( v, t ) {for (list<t >::iterator itr = v.begin(); itr != v.end(); itr++)
#define LIST_LOOP_CONST( v, t ) {for (list<t >::const_iterator itr = v.begin(); itr != v.end(); itr++)

//#define DELETE_VECTOR( v, t ) VECTOR_LOOP( v, t ) { delete (*itr); } v.clear();}
//#define DELETE_SET( v, t ) SET_LOOP( v, t ) { delete (*itr); } v.clear();}
#define DELETE_MAP( v, key, value ) MAP_LOOP( v, key, value ) { delete (*itr).second; } v.clear();}


//#define VECTOR_CONTAINS( v, obj ) (find( v.begin(), v.end(), obj ) != v.end())
//#define DELETE_FROM_VECTOR( v, obj ) v.erase( find( v.begin(), v.end(), obj ) );
//#define DELETE_FROM_VECTOR_LOOP( v, t, test ) {vector<t >::iterator itr = v.begin();\
//	while (itr != v.end())\
//	{\
//		t member = *itr;\
//\
//		if (test)\
//		{\
//			delete member;\
//			itr = v.erase( itr );\
//		}\
//		else\
//		{\
//			itr++;\
//		}\
//	}}\


#define FWRITE( x ) fwrite( &x, sizeof x, 1, fp );
#define FREAD( x ) fread( &x, sizeof x, 1, fp );

// random number between .01 and 1.0 (.01 increments)
#define RANDOM_PERCENTAGE		((double)(rand() % 100 + 1) / 100.0f)
// random number between 0 and 1.0
#define RANDOM_FRACTION			((double)rand() / RAND_MAX)

// place in a function that should only be called once
#define SINGLETON_FUNCTION		static bool _called = false;\
										MPB_ASSERT( _called == false );\
										_called = true;

#ifdef WIN32
extern HWND g_hiddenHwnd;

void mpb_init( HINSTANCE hInstance );
void mpb_cleanUp();

void writeLog();
void writeLog( const char* message );
void writeLastError( const char* message );
void printfLastError( const char* message );
void writeMemoryStatus();

bool LoadWaveFile(	HINSTANCE hInstance,
							LPDIRECTSOUND& directSound,
							LPDIRECTSOUNDBUFFER& buffer,
							int resourceID,
							bool _3D,
							bool primaryBuffer );
bool LoadWaveFile(	LPDIRECTSOUND& directSound,
							LPDIRECTSOUNDBUFFER& buffer,
							const char* fileName,
							bool _3D,
							bool primaryBuffer );

BYTE* readResource( HINSTANCE hInstance, int resourceID, LPCTSTR lpType );
#else
void writeLog();
void writeLog( const char* message );
#endif

BYTE* readFile( const char* path );
void writeFile( const char* path, BYTE* data, int byteCount );
void writeBitmap256(	const char* path, 
							unsigned char* pixels, 
							int width, 
							int height,
							RGBQUAD rgbQuad[256]	);
void writeGrayscaleBitmap( const char* path, BYTE* colors, int width, int height );
void writeBitmap24(	const char* path, 
							RGBTRIPLE* colors, 
							int width, 
							int height	);
BITMAPFILEHEADER readBitmapFileHeader( const char* path );
BITMAPINFOHEADER readBitmapInfoHeader( const char* path );
BYTE* readBitmapColors( const char* path, int& width, int& height );
void readBitmapColorValues(	const char* path,
										BYTE*& reds,
										BYTE*& greens,
										BYTE*& blues,
										int& width,
										int& height	);
unsigned char* readRawFile8( const char* path, int& width, int& height );
void readBitmapPalette( const char* path, RGBQUAD palette[256] );
RGBTRIPLE* readBitmapColors24( const char* path, int& width, int& height );
void compositeBitmap(	MPBList& inFiles,
								char* outFile,
								int rows,
								int columns	);

MPBFLOAT floatABS( MPBFLOAT f );
bool floatEQ( MPBFLOAT f1, MPBFLOAT f2 );
bool floatLT( MPBFLOAT f1, MPBFLOAT f2 );
bool floatLTE( MPBFLOAT f1, MPBFLOAT f2 );
bool floatGT( MPBFLOAT f1, MPBFLOAT f2 );
bool floatGTE( MPBFLOAT f1, MPBFLOAT f2 );
MPBFLOAT remap( MPBFLOAT inHigh, MPBFLOAT outLow, MPBFLOAT outHigh, MPBFLOAT value );

MPBFLOAT radiansToDegrees( MPBFLOAT radians );
MPBFLOAT degreesToRadians( MPBFLOAT degrees );
MPBFLOAT confineAngle( MPBFLOAT degrees );

void smoothFilter( unsigned char* dataIn, unsigned char* dataOut, int width, int height );

#ifdef WIN32
void enableFPExceptions();
//void generateLine( const POINT& from, const POINT& to, vector<POINT>& pointVector );
int initWinSock();
#endif

int createServerSocket( int port );
int createClientSocket( const char* host, int port );
int createDatagramServerSocket( int port );
int createDatagramClientSocket();

void MPBEnterCriticalSection();
void MPBLeaveCriticalSection();
MPBFLOAT round( MPBFLOAT d );
MPBFLOAT lerp( MPBFLOAT x, MPBFLOAT a, MPBFLOAT b );

void centerWindow( HWND hwnd );
#define PACKVERSION(major,minor) MAKELONG(minor,major)
DWORD GetDllVersion( LPCTSTR lpszDllName );


#endif

