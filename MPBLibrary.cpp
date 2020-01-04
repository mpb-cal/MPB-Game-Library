
#include "MPBLibrary.h"
#include "MPBGlobal.h"
//#include "../plib-1.6.0/sg.h"
#include "MPBPerlinNoise.h"
#include "MPBList.h"
#include "mmgr.h"

#ifdef WIN32
HANDLE					m_logFile;
CRITICAL_SECTION		m_criticalSection;
int						g_breakFlag;
HWND						g_hiddenHwnd = 0;

//#pragma comment(lib, "ws2_32.lib")
#pragma comment(lib, "dplayx.lib")
#pragma comment(lib, "dxguid.lib")
#pragma comment(lib, "openal32.lib")
#pragma comment(lib, "alut.lib")
//#pragma comment(lib, "../plib-1.6.0/sg_d.lib")
//#pragma comment(lib, "../plib-1.6.0/ul_d.lib")


void initLog();

LRESULT CALLBACK hiddenWndProc( HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam )
{
	return DefWindowProc( hWnd, message, wParam, lParam );
}

void mpb_init( HINSTANCE hInstance )
{
	g_hInstance = hInstance;
	initLog();
	InitializeCriticalSection( &m_criticalSection );

	if (g_hInstance)
	{
		// create hidden window for dialog boxes

		WNDCLASS wc;
		memset( &wc, 0, sizeof( WNDCLASS ) );
		wc.style				= 0;
		wc.lpfnWndProc		= hiddenWndProc;
		wc.cbClsExtra		= 0;
		wc.cbWndExtra		= 0;
		wc.hInstance		= g_hInstance;
		wc.hCursor			= 0;
		wc.lpszMenuName	= 0;
		wc.hbrBackground	= 0;
		wc.lpszClassName	= "MPB_HIDDEN_WINDOW";

		RegisterClass( &wc );
		g_hiddenHwnd = CreateWindow( wc.lpszClassName, "Hidden", 0, 0, 500, 500, 0, NULL, NULL, g_hInstance, 0 );
	}
}

void mpb_cleanUp()
{
	CloseHandle( m_logFile );
	DeleteCriticalSection( &m_criticalSection );
}

void MPBEnterCriticalSection()
{
	EnterCriticalSection( &m_criticalSection );
}

void MPBLeaveCriticalSection()
{
	LeaveCriticalSection( &m_criticalSection );
}

void initLog()
{
//	SINGLETON_FUNCTION;

	m_logFile = CreateFile( 
		"main.log", 
		GENERIC_WRITE, 
		0, 
		NULL, 
		CREATE_ALWAYS, 
		FILE_ATTRIBUTE_NORMAL, 
		NULL	);
}

// does not automatically add newline
void writeLog()
{
	writeLog( g_logBuf );
}

// does not automatically add newline
void writeLog( const char* message )
{
	MPB_ASSERT( m_logFile );		// call initLog() first
	DWORD written;

	MPBEnterCriticalSection();
	WriteFile(
		m_logFile,
		message,
		strlen( message ),
		&written,
		NULL
	);
	MPBLeaveCriticalSection();

//	FlushFileBuffers( m_logFile );
}

void writeLastError( const char* message )
{
	LPVOID lpMsgBuf;
	FormatMessage( 
		FORMAT_MESSAGE_ALLOCATE_BUFFER | 
			FORMAT_MESSAGE_FROM_SYSTEM | 
			FORMAT_MESSAGE_IGNORE_INSERTS,
		NULL,
		GetLastError(),
		MAKELANGID( LANG_NEUTRAL, SUBLANG_DEFAULT ),
		(LPTSTR) &lpMsgBuf,
		0,
		NULL 
	);

	writeLog( message );
	writeLog( (LPCTSTR)lpMsgBuf );
	writeLog( "\n" );
	LocalFree( lpMsgBuf );
}

void printfLastError( const char* message )
{
	LPVOID lpMsgBuf;
	FormatMessage( 
		FORMAT_MESSAGE_ALLOCATE_BUFFER | 
			FORMAT_MESSAGE_FROM_SYSTEM | 
			FORMAT_MESSAGE_IGNORE_INSERTS,
		NULL,
		GetLastError(),
		MAKELANGID( LANG_NEUTRAL, SUBLANG_DEFAULT ),
		(LPTSTR) &lpMsgBuf,
		0,
		NULL 
	);

	printf( message );
	printf( (LPCTSTR)lpMsgBuf );
	printf( "\n" );
	LocalFree( lpMsgBuf );
}


bool LoadWaveFile(	HINSTANCE hInstance,
							LPDIRECTSOUND& directSound, 
							LPDIRECTSOUNDBUFFER& buffer, 
							int resourceID,
							bool _3D,
							bool primaryBuffer )
{
/*
00	"RIFF"
04	filesize
08	"WAVE"
0C	"FMT "
10	header size

WAVEFORMATEX:

14	WORD wFormatTag
16	WORD nChannels
18	DWORD nSamplesPerSec
1C	DWORD nAvgBytesPerSec
20	WORD nBlockAlign
22	WORD wBitsPerSample

24	"data"
28	DWORD datasize
2C	data

(24 "fact"
28	04 00 00 00
2C	f7 0e 00 00
30	"data"
34	DWORD datasize
38	data)
*/

	unsigned char* waveFile = readResource( hInstance, resourceID, "WAVE" );
//	unsigned char* waveFile = NULL;
/*
	FILE* fp = fopen( fileName, "rb" );
	struct stat statBuf;
	stat( fileName, &statBuf );
	waveFile = new unsigned char[ statBuf.st_size ];
	fread( waveFile, sizeof unsigned char, statBuf.st_size, fp );
	fclose( fp );
*/
	if (waveFile == NULL)
	{
		writeLastError( "LoadWaveFile failed.\n" );
		return false;
	}

	int formatStart = 0x14;
	int dataStart = 0x24;

	WAVEFORMATEX* pWaveFormat = (WAVEFORMATEX*)(waveFile + formatStart);
	// find the text "data" after the waveformatex
	while (true)
	{
		if (	*(waveFile + dataStart) == 'd' &&
				*(waveFile + dataStart + 1) == 'a' &&
				*(waveFile + dataStart + 2) == 't' &&
				*(waveFile + dataStart + 3) == 'a'	)
		{
			break;
		}

		++dataStart;
		MPB_ASSERT( dataStart < 100 );
	}

	DWORD dataSize = *((DWORD*)(waveFile + dataStart + 4));
	unsigned char* lpbSoundData = waveFile + dataStart + 8;

	DSBUFFERDESC bufferDescription;
	memset( &bufferDescription, 0, sizeof(DSBUFFERDESC) );
	bufferDescription.dwSize = sizeof(DSBUFFERDESC);

	if (_3D) bufferDescription.dwFlags = DSBCAPS_CTRL3D; 
	else bufferDescription.dwFlags = DSBCAPS_CTRLPAN | DSBCAPS_CTRLVOLUME | DSBCAPS_CTRLFREQUENCY; 

	if (primaryBuffer) 
	{
		bufferDescription.dwFlags |= DSBCAPS_PRIMARYBUFFER; 
		bufferDescription.dwBufferBytes = 0; 
		bufferDescription.lpwfxFormat = NULL; 
	}
	else
	{
		bufferDescription.dwBufferBytes = dataSize; 
		bufferDescription.lpwfxFormat = pWaveFormat; 
	}

	if (directSound->CreateSoundBuffer( &bufferDescription, &buffer, NULL ) != DS_OK)
	{
		writeLastError( "CreateSoundBuffer failed.\n" );
		delete[] waveFile;
		return false;
	}

	if (primaryBuffer) 
	{
//		if (buffer->SetFormat( pWaveFormat ) != DS_OK)
//		{
//			writeLastError( "SetFormat failed.\n" );
//			delete[] waveFile;
//			return false;
//		}

		return true;
	}

	LPVOID lpbuf1 = NULL;
	LPVOID lpbuf2 = NULL;
	DWORD dwsize1 = 0;
	DWORD dwsize2 = 0;
	DWORD dwOffset = 0;

	if (buffer->Lock( dwOffset, dataSize, &lpbuf1, &dwsize1, &lpbuf2, &dwsize2, 0 ) != DS_OK)
	{
		writeLastError( "Lock failed.\n" );
		delete[] waveFile;
		return false;
	}

	CopyMemory( lpbuf1, lpbSoundData, dwsize1 );
	if (lpbuf2) CopyMemory( lpbuf2, lpbSoundData + dwsize1, dwsize2 ); 

	if (buffer->Unlock( lpbuf1, dwsize1, lpbuf2, dwsize2 ) != DS_OK)
	{
		writeLastError( "Unlock failed.\n" );
		delete[] waveFile;
		return false;
	}

	delete[] waveFile;
	return true;
}

bool LoadWaveFile(	LPDIRECTSOUND& directSound, 
							LPDIRECTSOUNDBUFFER& buffer, 
							const char* fileName,
							bool _3D,
							bool primaryBuffer )
{
	unsigned char* waveFile = readFile( fileName );

	if (waveFile == NULL)
	{
		writeLastError( "LoadWaveFile failed.\n" );
		return false;
	}

	int formatStart = 0x14;
	int dataStart = 0x24;

	WAVEFORMATEX* pWaveFormat = (WAVEFORMATEX*)(waveFile + formatStart);
	// find the text "data" after the waveformatex
	while (true)
	{
		if (	*(waveFile + dataStart) == 'd' &&
				*(waveFile + dataStart + 1) == 'a' &&
				*(waveFile + dataStart + 2) == 't' &&
				*(waveFile + dataStart + 3) == 'a'	)
		{
			break;
		}

		++dataStart;
		MPB_ASSERT( dataStart < 100 );
	}

	DWORD dataSize = *((DWORD*)(waveFile + dataStart + 4));
	unsigned char* lpbSoundData = waveFile + dataStart + 8;

	DSBUFFERDESC bufferDescription;
	memset( &bufferDescription, 0, sizeof(DSBUFFERDESC) );
	bufferDescription.dwSize = sizeof(DSBUFFERDESC);
	if (_3D) bufferDescription.dwFlags = DSBCAPS_CTRL3D; 

	if (primaryBuffer) 
	{
		bufferDescription.dwFlags |= DSBCAPS_PRIMARYBUFFER;
		bufferDescription.dwBufferBytes = 0; 
		bufferDescription.lpwfxFormat = NULL; 
	}
	else
	{
		bufferDescription.dwFlags |= 
			DSBCAPS_CTRLPOSITIONNOTIFY | DSBCAPS_CTRLPAN | DSBCAPS_CTRLVOLUME | DSBCAPS_CTRLFREQUENCY;
		bufferDescription.dwBufferBytes = dataSize; 
		bufferDescription.lpwfxFormat = pWaveFormat; 
	}

	if (directSound->CreateSoundBuffer( &bufferDescription, &buffer, NULL ) != DS_OK)
	{
		writeLastError( "CreateSoundBuffer failed.\n" );
		delete[] waveFile;
		return false;
	}

	if (primaryBuffer) 
	{
//		if (buffer->SetFormat( pWaveFormat ) != DS_OK)
//		{
//			writeLastError( "SetFormat failed.\n" );
//			delete[] waveFile;
//			return false;
//		}

		delete[] waveFile;
		return true;
	}

	LPVOID lpbuf1 = NULL;
	LPVOID lpbuf2 = NULL;
	DWORD dwsize1 = 0;
	DWORD dwsize2 = 0;
	DWORD dwOffset = 0;

	if (buffer->Lock( dwOffset, dataSize, &lpbuf1, &dwsize1, &lpbuf2, &dwsize2, 0 ) != DS_OK)
	{
		writeLastError( "Lock failed.\n" );
		delete[] waveFile;
		return false;
	}

	CopyMemory( lpbuf1, lpbSoundData, dwsize1 );
	if (lpbuf2) CopyMemory( lpbuf2, lpbSoundData + dwsize1, dwsize2 ); 

	if (buffer->Unlock( lpbuf1, dwsize1, lpbuf2, dwsize2 ) != DS_OK)
	{
		writeLastError( "Unlock failed.\n" );
		delete[] waveFile;
		return false;
	}

	delete[] waveFile;
	return true;
}


#else

// does not automatically add newline
void writeLog()
{
	writeLog( g_logBuf );
}

// does not automatically add newline
void writeLog( const char* message )
{
	printf( message );
}

#endif

// delete return value after calling
unsigned char* readFile( const char* path )
{
	unsigned char* data = NULL;

	FILE* fp = fopen( path, "rb" );
	sprintf( g_logBuf, "Could not find file %s", path );
	MPB_ASSERT( fp );
	struct stat statBuf;
	stat( path, &statBuf );
	data = new unsigned char[ statBuf.st_size ];
	MPB_ASSERT( data );
	fread( data, sizeof (unsigned char), statBuf.st_size, fp );
	fclose( fp );

	return data;
}

void writeFile( const char* path, unsigned char* data, int charCount )
{
	FILE* fp = fopen( path, "wb" );
	MPB_ASSERT( fp );
	MPB_ASSERT( data );
	fwrite( data, sizeof (unsigned char), charCount, fp );
	fclose( fp );
}

BITMAPFILEHEADER readBitmapFileHeader( const char* path )
{
	unsigned char* data = readFile( path );
	int i = 0;

	BITMAPFILEHEADER bfh;
	bfh.bfType			= *((WORD*)(data + 0));
	bfh.bfSize			= *((DWORD*)(data + 2));
	bfh.bfReserved1	= *((WORD*)(data + 6));
	bfh.bfReserved2	= *((WORD*)(data + 8));
	bfh.bfOffBits		= *((DWORD*)(data + 10));

	delete[] data;
	return bfh;
}

BITMAPINFOHEADER readBitmapInfoHeader( const char* path )
{
	unsigned char* data = readFile( path );
	int i = 0;

	BITMAPFILEHEADER bfh;
	bfh.bfType			= *((WORD*)(data + 0));
	bfh.bfSize			= *((DWORD*)(data + 2));
	bfh.bfReserved1	= *((WORD*)(data + 6));
	bfh.bfReserved2	= *((WORD*)(data + 8));
	bfh.bfOffBits		= *((DWORD*)(data + 10));

	i += 14;//sizeof (BITMAPFILEHEADER);

	BITMAPINFOHEADER bi;
	bi.biSize				= *((DWORD*)(data + i + 0));
	bi.biWidth				= *((LONG*)(data + i + 4));
	bi.biHeight				= *((LONG*)(data + i + 8));
	bi.biPlanes				= *((WORD*)(data + i + 12));
	bi.biBitCount			= *((WORD*)(data + i + 14));
	bi.biCompression		= *((DWORD*)(data + i + 16));
	bi.biSizeImage			= *((DWORD*)(data + i + 20));
	bi.biXPelsPerMeter	= *((LONG*)(data + i + 24));
	bi.biYPelsPerMeter	= *((LONG*)(data + i + 28));
	bi.biClrUsed			= *((DWORD*)(data + i + 32));
	bi.biClrImportant		= *((DWORD*)(data + i + 36));

	delete[] data;
	return bi;
}

// 256-color bitmap.
// delete return value after calling.
// returns array of indexes
unsigned char* readBitmapColors( const char* path, int& width, int& height )
{
	unsigned char* data = readFile( path );
	int i = 0;

	BITMAPFILEHEADER bfh;
//	bfh = *((BITMAPFILEHEADER*)(data + i));
	bfh.bfType			= *((WORD*)(data + 0));
	bfh.bfSize			= *((DWORD*)(data + 2));
	bfh.bfReserved1	= *((WORD*)(data + 6));
	bfh.bfReserved2	= *((WORD*)(data + 8));
	bfh.bfOffBits		= *((DWORD*)(data + 10));

	i += 14;//sizeof (BITMAPFILEHEADER);

	BITMAPINFOHEADER bi;
//	bi = *((BITMAPINFOHEADER*)(data + i));
	bi.biSize				= *((DWORD*)(data + i + 0));
	bi.biWidth				= *((LONG*)(data + i + 4));
	bi.biHeight				= *((LONG*)(data + i + 8));
	bi.biPlanes				= *((WORD*)(data + i + 12));
	bi.biBitCount			= *((WORD*)(data + i + 14));
	bi.biCompression		= *((DWORD*)(data + i + 16));
	bi.biSizeImage			= *((DWORD*)(data + i + 20));
	bi.biXPelsPerMeter	= *((LONG*)(data + i + 24));
	bi.biYPelsPerMeter	= *((LONG*)(data + i + 28));
	bi.biClrUsed			= *((DWORD*)(data + i + 32));
	bi.biClrImportant		= *((DWORD*)(data + i + 36));

	i += 40;//sizeof (BITMAPINFOHEADER);

	width = bi.biWidth;
	height = bi.biHeight;
	int size = width * height;
/*	
	printf( "bfType=%d\n", bfh.bfType );
	printf( "bfSize=%d\n", bfh.bfSize );
	printf( "bfReserved1=%d\n", bfh.bfReserved1 );
	printf( "bfReserved2=%d\n", bfh.bfReserved2 );
	printf( "bfOffBits=%d\n\n", bfh.bfOffBits );

	printf( "biSize=%d\n", bi.biSize );
	printf( "biWidth=%d\n", bi.biWidth );
	printf( "biHeight=%d\n", bi.biHeight );
	printf( "biPlanes=%d\n", bi.biPlanes );
	printf( "biBitCount=%d\n", bi.biBitCount );
	printf( "biComperssion=%d\n", bi.biCompression );
	printf( "biSizeImage=%d\n", bi.biSizeImage );
	printf( "biXPelsPerMeter=%d\n", bi.biXPelsPerMeter );
	printf( "biYPelsPerMeter=%d\n", bi.biYPelsPerMeter );
	printf( "biClrUsed=%d\n", bi.biClrUsed );
	printf( "biClrImportant=%d\n", bi.biClrImportant );
*/
//	i += 256 * sizeof RGBQUAD; // skip past palette
	i = bfh.bfOffBits; // skip to pixel data

	unsigned char* colors = new unsigned char[size];

	memcpy( colors, (unsigned char*)(data + i), size );

	delete[] data;

	return colors;
}

// 256-color bitmap.
// delete reds, greens and blues after calling.
void readBitmapColorValues(	const char* path, 
										unsigned char*& reds, 
										unsigned char*& greens, 
										unsigned char*& blues, 
										int& width, 
										int& height	)
{
	unsigned char* data = readFile( path );
	int i = 0;
	
//	BITMAPFILEHEADER bfh = *((BITMAPFILEHEADER*)(data + i));
	i += sizeof (BITMAPFILEHEADER);

	BITMAPINFOHEADER bi;
	bi = *((BITMAPINFOHEADER*)(data + i));
	i += sizeof (BITMAPINFOHEADER);

	width = bi.biWidth;
	height = bi.biHeight;
	int size = width * height;

	// find size of color table
	int colors = 0;
	if (bi.biBitCount > 8)
		colors = 0;
	else if (bi.biClrUsed == 0)
		colors = 1 << bi.biBitCount;
	else
		colors = bi.biClrUsed;

	MPB_ASSERT( colors > 0 && colors <= 256 );

	// read color table
	RGBQUAD* rgbQuads = new RGBQUAD[colors];
	MPB_ASSERT( rgbQuads );
	memcpy( rgbQuads, data + i, colors * sizeof( RGBQUAD ) );
	i += colors * sizeof( RGBQUAD );

//	i += 256 * sizeof RGBQUAD; // skip past palette
//	i = bfh.bfOffBits; // skip to pixel data

	reds = new unsigned char[size];
	greens = new unsigned char[size];
	blues = new unsigned char[size];

	MPB_ASSERT( reds );
	MPB_ASSERT( greens );
	MPB_ASSERT( blues );

	int row, column;

	for (row=0; row<height; row++)
	{
		for (column=0; column<width; column++)
		{
			unsigned char color = *(data + i + (width * row + column));

			reds[width * row + column] = rgbQuads[color].rgbRed;
			greens[width * row + column] = rgbQuads[color].rgbGreen;
			blues[width * row + column] = rgbQuads[color].rgbBlue;
		}
	}

	NULLIFY_ARRAY( rgbQuads );
	NULLIFY_ARRAY( data );
}

// delete return value after calling.
// returns array of bytes
unsigned char* readRawFile8( const char* path, int& width, int& height )
{
	unsigned char* data = readFile( path );

	struct stat statBuf;
	stat( path, &statBuf );
	statBuf.st_size;

	width = (int)sqrt( (double)statBuf.st_size );
	height = width;

	return data;
}

// 24-bit bitmap.
// delete return value after calling.
RGBTRIPLE* readBitmapColors24( const char* path, int& width, int& height )
{
	unsigned char* data = readFile( path );
	int i = 0;

	BITMAPFILEHEADER bfh;
//	bfh = *((BITMAPFILEHEADER*)(data + i));
	bfh.bfType			= *((WORD*)(data + 0));
	bfh.bfSize			= *((DWORD*)(data + 2));
	bfh.bfReserved1	= *((WORD*)(data + 6));
	bfh.bfReserved2	= *((WORD*)(data + 8));
	bfh.bfOffBits		= *((DWORD*)(data + 10));

	i += 14;//sizeof (BITMAPFILEHEADER);

	BITMAPINFOHEADER bi;
//	bi = *((BITMAPINFOHEADER*)(data + i));
	bi.biSize				= *((DWORD*)(data + i + 0));
	bi.biWidth				= *((LONG*)(data + i + 4));
	bi.biHeight				= *((LONG*)(data + i + 8));
	bi.biPlanes				= *((WORD*)(data + i + 12));
	bi.biBitCount			= *((WORD*)(data + i + 14));
	bi.biCompression		= *((DWORD*)(data + i + 16));
	bi.biSizeImage			= *((DWORD*)(data + i + 20));
	bi.biXPelsPerMeter	= *((LONG*)(data + i + 24));
	bi.biYPelsPerMeter	= *((LONG*)(data + i + 28));
	bi.biClrUsed			= *((DWORD*)(data + i + 32));
	bi.biClrImportant		= *((DWORD*)(data + i + 36));

	i += 40;//sizeof (BITMAPINFOHEADER);

	width = bi.biWidth;
	height = bi.biHeight;
	int size = width * height;

	i = bfh.bfOffBits; // skip to pixel data

	RGBTRIPLE* triples = new RGBTRIPLE[size];

	memcpy( triples, (RGBTRIPLE*)(data + i), size * sizeof (RGBTRIPLE) );

	delete[] data;

	return triples;
}

// 256-color bitmap.
void readBitmapPalette( const char* path, RGBQUAD palette[256] )
{
	unsigned char* data = readFile( path );
	int i = 0;
	
	i += sizeof (BITMAPFILEHEADER);

	BITMAPINFOHEADER bi;
	bi = *((BITMAPINFOHEADER*)(data + i));
	i += sizeof (BITMAPINFOHEADER);

	int size = bi.biWidth * bi.biHeight;

	// find size of color table
	int colors = 0;
	if (bi.biBitCount > 8)
		colors = 0;
	else if (bi.biClrUsed == 0)
		colors = 1 << bi.biBitCount;
	else
		colors = bi.biClrUsed;

//	MPB_ASSERT( colors > 0 && colors <= 256 );
	MPB_ASSERT( colors == 256 );

	// read color table
	memcpy( palette, data + i, colors * sizeof( RGBQUAD ) );
	i += colors * sizeof( RGBQUAD );

	NULLIFY_ARRAY( data );
}

void writeBitmap24(	const char* path, 
							RGBTRIPLE* colors, 
							int width, 
							int height	)
{
	int size = width * height;

	int rc;
	FILE* fp = fopen( path, "wb" );
	MPB_ASSERT( fp );

	BITMAPFILEHEADER bfh;

	bfh.bfType = 0x4D42;
	bfh.bfSize = (DWORD)(sizeof(BITMAPFILEHEADER) + 40 + size * 3);
	bfh.bfReserved1 = 0;
	bfh.bfReserved2 = 0;
	bfh.bfOffBits = 14 + 40;

	rc = fwrite( &bfh, sizeof( unsigned char ), sizeof( bfh ), fp );
	MPB_ASSERT( rc == 14 );

	BITMAPINFOHEADER bih;

	bih.biSize = 40;
	bih.biWidth = width;
	bih.biHeight = height;
	bih.biPlanes = 1;
	bih.biBitCount = 24;
	bih.biCompression = BI_RGB;
	bih.biSizeImage = 0;
	bih.biXPelsPerMeter = 0;
	bih.biYPelsPerMeter = 0;
	bih.biClrUsed = 0;
	bih.biClrImportant = 0;

	rc = fwrite( &bih, sizeof( unsigned char ), sizeof( bih ), fp );
	MPB_ASSERT( rc == 40 );

	rc = fwrite( colors, sizeof (RGBTRIPLE), size, fp );
	MPB_ASSERT( rc == size );

	fclose( fp );
}

#ifdef WIN32

// 24-bit bitmaps
void compositeBitmap(	MPBList& inFiles,	// char*
								char* outFile,
								int rows,
								int columns	)
{
//	MPB_ASSERT( 0 );
	int width;
	int height;
	RGBTRIPLE* tmp = readBitmapColors24( (char*)inFiles[0], width, height );
	delete[] tmp;
	int compWidth = width * columns;
	int compHeight = height * rows;
	RGBTRIPLE* compPixels = new RGBTRIPLE[ compWidth * compHeight ];

	RGBTRIPLE* colors = NULL;
	int column = 0;
	int row = 0;
	MPB_LIST_LOOP( inFiles, file, char* )
	{
		colors = readBitmapColors24( file, width, height );
		MPB_ASSERT( colors );

		for (int i=0; i<height; i++)
		{
			for (int j=0; j<width; j++)
			{
				compPixels[(i + height * row) * compWidth + (j + width * column)] = colors[i * width + j];
			}
		}

		delete[] colors;

		if (++column == columns)
		{
			column = 0;
			++row;
		}
	}

//	colors = readBitmapColors24( inFiles[0], width, height );
//	writeBitmap24( outFile, colors, width, height );
	writeBitmap24( outFile, compPixels, compWidth, compHeight );
}

// 256-color bitmap
void writeBitmap256(	const char* path, 
							unsigned char* pixels, 
							int width, 
							int height,
							RGBQUAD rgbQuad[256]	)
{
	int size = width * height;

	int rc;
	FILE* fp = fopen( path, "wb" );
	MPB_ASSERT( fp );

	BITMAPFILEHEADER bfh;

	bfh.bfType = 0x4D42;
	bfh.bfSize = (DWORD)(sizeof(BITMAPFILEHEADER) + 40 + 256 * sizeof(RGBQUAD) + size);
	bfh.bfReserved1 = 0;
	bfh.bfReserved2 = 0;
	bfh.bfOffBits = 1078;

	rc = fwrite( &bfh, sizeof( unsigned char ), sizeof( bfh ), fp );
	MPB_ASSERT( rc == 14 );

	BITMAPINFOHEADER bih;

	bih.biSize = 40;
	bih.biWidth = width;
	bih.biHeight = height;
	bih.biPlanes = 1;
	bih.biBitCount = 8;
	bih.biCompression = BI_RGB;
	bih.biSizeImage = size;
	bih.biXPelsPerMeter = 0;
	bih.biYPelsPerMeter = 0;
	bih.biClrUsed = 0;//256;
	bih.biClrImportant = 0;

	rc = fwrite( &bih, sizeof( unsigned char ), sizeof( bih ), fp );
	MPB_ASSERT( rc == 40 );

	rc = fwrite( rgbQuad, sizeof( rgbQuad[0] ), 256, fp );
	MPB_ASSERT( rc == 256 );

	rc = fwrite( pixels, sizeof (unsigned char), size, fp );
	MPB_ASSERT( rc == size );

	fclose( fp );
}

// 256-color bitmap
void writeGrayscaleBitmap( const char* path, unsigned char* pixels, int width, int height )
{
	RGBQUAD rgbQuad[256];
	for (int i=0; i<256; i++)
	{
		rgbQuad[i].rgbBlue = i;
		rgbQuad[i].rgbGreen = i;
		rgbQuad[i].rgbRed = i;
		rgbQuad[i].rgbReserved = 0;
	}

	writeBitmap256( path, pixels, width, height, rgbQuad );
}

// takes a value in the range 0 - inHigh
// and remaps it to the range outLow - outHigh
MPBFLOAT remap( MPBFLOAT inHigh, MPBFLOAT outLow, MPBFLOAT outHigh, MPBFLOAT value )
{
	MPBFLOAT outScale = outHigh - outLow;
	MPBFLOAT relativeValue = value / inHigh;
	return outLow + relativeValue * outScale;
}

unsigned char* readResource( HINSTANCE hInstance, int resourceID, LPCTSTR lpType )
{
	HRSRC hRsrc = FindResourceEx(
		hInstance,
		lpType,
		MAKEINTRESOURCE( resourceID ),
		MAKELANGID( LANG_NEUTRAL, SUBLANG_NEUTRAL )
	);

	if (hRsrc == NULL)
	{
		writeLastError( "FindResourceEx failed.\n" );
		return NULL;
	}

	HGLOBAL hGlobal = LoadResource( NULL, hRsrc );
	if (hGlobal == NULL)
	{
		writeLastError( "LoadResource failed.\n" );
		return NULL;
	}

	DWORD dwSize = SizeofResource( hInstance, hRsrc );
	unsigned char* pResource = (unsigned char*)LockResource( hGlobal );
	if (pResource == NULL)
	{
		writeLastError( "LockResource failed.\n" );
		return NULL;
	}

	unsigned char* pBytes = new unsigned char[ dwSize ];
	if (pBytes == NULL)
	{
		writeLastError( "readResource: memory allocation failed.\n" );
		return NULL;
	}

	CopyMemory( pBytes, pResource, dwSize );

	return pBytes;
}

#endif

MPBFLOAT floatABS( MPBFLOAT f )
{
	if (f < 0) return -f;
	return f;
}

bool floatEQ( MPBFLOAT f1, MPBFLOAT f2 )
{
	if (floatABS( f1 - f2 ) < EPSILON) return true;
	return false;
}

bool floatLT( MPBFLOAT f1, MPBFLOAT f2 )
{
	if (floatEQ( f1, f2 )) return false;
	if (f1 < f2) return true;
	return false;
}

bool floatLTE( MPBFLOAT f1, MPBFLOAT f2 )
{
	if (floatEQ( f1, f2 )) return true;
	if (f1 < f2) return true;
	return false;
}

bool floatGT( MPBFLOAT f1, MPBFLOAT f2 )
{
	if (floatEQ( f1, f2 )) return false;
	if (f1 > f2) return true;
	return false;
}

bool floatGTE( MPBFLOAT f1, MPBFLOAT f2 )
{
	if (floatEQ( f1, f2 )) return true;
	if (f1 > f2) return true;
	return false;
}

MPBFLOAT radiansToDegrees( MPBFLOAT radians )
{
	return radians * _180_OVER_PI;
}

MPBFLOAT degreesToRadians( MPBFLOAT degrees )
{
	return degrees * PI_OVER_180;
}

// converts angle to be between 0 and 360
MPBFLOAT confineAngle( MPBFLOAT degrees )
{
	MPBFLOAT d = degrees;
	if (degrees > 0)
	{
		degrees -= 360 * (int)(degrees / 360);
	}
	else if (degrees < 0)
	{
		degrees -= 360 * (int)(degrees / 360);
		degrees += 360;
	}

//	while (degrees >= 360) degrees -= 360;
//	while (degrees < 0) degrees += 360;

	MPB_ASSERT( degrees <= 360 );
	MPB_ASSERT( degrees >= 0 );
	return degrees;
}

#ifdef WIN32
void writeMemoryStatus()
{
	MEMORYSTATUS ms;
	GlobalMemoryStatus( &ms );

	char buf[1000];

	sprintf( buf, "GlobalMemoryStatus:\n" );
	writeLog( buf );

	sprintf( buf, "%d MemoryLoad \n", ms.dwMemoryLoad );
	writeLog( buf );
	sprintf( buf, "%03d,%03d,%03d,%03d TotalPhys \n", 
//		ms.dwTotalPhys, 
		ms.dwTotalPhys / 1000000000 % 1000, 
		ms.dwTotalPhys / 1000000 % 1000, 
		ms.dwTotalPhys / 1000 % 1000, 
		ms.dwTotalPhys / 1 % 1000 );
	writeLog( buf );
	sprintf( buf, "%03d,%03d,%03d,%03d AvailPhys \n", 
		//ms.dwAvailPhys );
		ms.dwAvailPhys / 1000000000 % 1000, 
		ms.dwAvailPhys / 1000000 % 1000, 
		ms.dwAvailPhys / 1000 % 1000, 
		ms.dwAvailPhys / 1 % 1000 );
	writeLog( buf );
	sprintf( buf, "%03d,%03d,%03d,%03d TotalPageFile \n", 
		//ms.dwTotalPageFile );
		ms.dwTotalPageFile / 1000000000 % 1000, 
		ms.dwTotalPageFile / 1000000 % 1000, 
		ms.dwTotalPageFile / 1000 % 1000, 
		ms.dwTotalPageFile / 1 % 1000 );
	writeLog( buf );
	sprintf( buf, "%03d,%03d,%03d,%03d AvailPageFile \n", 
		//ms.dwAvailPageFile );
		ms.dwAvailPageFile / 1000000000 % 1000, 
		ms.dwAvailPageFile / 1000000 % 1000, 
		ms.dwAvailPageFile / 1000 % 1000, 
		ms.dwAvailPageFile / 1 % 1000 );
	writeLog( buf );
	sprintf( buf, "%03d,%03d,%03d,%03d TotalVirtual \n", 
		//ms.dwTotalVirtual );
		ms.dwTotalVirtual / 1000000000 % 1000, 
		ms.dwTotalVirtual / 1000000 % 1000, 
		ms.dwTotalVirtual / 1000 % 1000, 
		ms.dwTotalVirtual / 1 % 1000 );
	writeLog( buf );
	sprintf( buf, "%03d,%03d,%03d,%03d AvailVirtual \n", 
		//ms.dwAvailVirtual );
		ms.dwAvailVirtual / 1000000000 % 1000, 
		ms.dwAvailVirtual / 1000000 % 1000, 
		ms.dwAvailVirtual / 1000 % 1000, 
		ms.dwAvailVirtual / 1 % 1000 );
	writeLog( buf );
}
#endif


///////////////////////////////////////////////////////////
// socket functions

#if 0

#ifdef WIN32
int initWinSock()
{
	WORD wVersionRequested;
	WSADATA wsaData;
	int err;

	wVersionRequested = MAKEWORD( 2, 2 );

	err = WSAStartup( wVersionRequested, &wsaData );
	if ( err != 0 ) {
		/* Tell the user that we could not find a usable */
		/* WinSock DLL.                                  */
		return -1;
	}

	/* Confirm that the WinSock DLL supports 2.2.*/
	/* Note that if the DLL supports versions greater    */
	/* than 2.2 in addition to 2.2, it will still return */
	/* 2.2 in wVersion since that is the version we      */
	/* requested.                                        */

	if (	LOBYTE( wsaData.wVersion ) != 2 ||
			HIBYTE( wsaData.wVersion ) != 2 ) {
		/* Tell the user that we could not find a usable */
		/* WinSock DLL.                                  */
		WSACleanup( );
		return -1; 
	}

	/* The WinSock DLL is acceptable. Proceed. */
	return 0;
}
#endif

// non-blocking socket
int createServerSocket( int port )
{
	int s = socket( AF_INET, SOCK_STREAM, 0 );
	if (s == -1)
	{
		return -1;
	}

	int reuse_addr = 1;
	setsockopt( s, SOL_SOCKET, SO_REUSEADDR, (char*)&reuse_addr, sizeof reuse_addr );

	sockaddr_in addr_in;

	addr_in.sin_family = AF_INET;
	addr_in.sin_port = htons( port );
	addr_in.sin_addr.s_addr = htonl( INADDR_ANY );

	if (bind( 
		s, 
		(sockaddr*)&addr_in, 
		sizeof(addr_in) ) == -1)
	{
		return -1;
	}

	if (listen( s, 5 ) == -1)
	{
		return -1;
	}

#ifdef WIN32
	unsigned long arg = 1;
	ioctlsocket( s, FIONBIO, &arg );
#else
	fcntl( s, F_SETFL, O_NONBLOCK );
#endif

	return s;
}

// non-blocking socket
// host is "1.2.3.4"
int createClientSocket( const char* host, int port )
{
	int s = socket( AF_INET, SOCK_STREAM, 0 );
	if (s == -1)
	{
		return -1;
	}

	sockaddr_in addr_in;

	addr_in.sin_family = AF_INET;
	addr_in.sin_port = htons( port );
	addr_in.sin_addr.s_addr = inet_addr( host );

	if (connect( 
		s, 
		(sockaddr*)&addr_in, 
		sizeof(addr_in) ) == -1)
	{
		return -1;
	}

#ifdef WIN32
	unsigned long arg = 1;
	ioctlsocket( s, FIONBIO, &arg );
#else
	fcntl( s, F_SETFL, O_NONBLOCK );
#endif

	return s;
}

// non-blocking socket
int createDatagramServerSocket( int port )
{
	int s = socket( AF_INET, SOCK_DGRAM, 0 );
	if (s == -1)
	{
		return -1;
	}

	int reuse_addr = 1;
	setsockopt( s, SOL_SOCKET, SO_REUSEADDR, (char*)&reuse_addr, sizeof reuse_addr );

	sockaddr_in addr_in;

	addr_in.sin_family = AF_INET;
	addr_in.sin_port = htons( port );
	addr_in.sin_addr.s_addr = htonl( INADDR_ANY );

	if (bind( 
		s, 
		(sockaddr*)&addr_in, 
		sizeof(addr_in) ) == -1)
	{
		return -1;
	}

#ifdef WIN32
	unsigned long arg = 1;
	ioctlsocket( s, FIONBIO, &arg );
#else
	fcntl( s, F_SETFL, O_NONBLOCK );
#endif

	return s;
}

// non-blocking socket
int createDatagramClientSocket()
{
	int s = socket( AF_INET, SOCK_DGRAM, 0 );
	if (s == -1)
	{
		return -1;
	}

	int reuse_addr = 1;
	setsockopt( s, SOL_SOCKET, SO_REUSEADDR, (char*)&reuse_addr, sizeof reuse_addr );

#ifdef WIN32
	unsigned long arg = 1;
	ioctlsocket( s, FIONBIO, &arg );
#else
	fcntl( s, F_SETFL, O_NONBLOCK );
#endif

	return s;
}

#endif

///////////////////////////////////////////////////////////

BYTE smoothFilterDataValue( BYTE* data, int x, int y, int width, int height )
{
	if (	x < 0 ||
			y < 0 ||
			x >= width ||
			y >= height	)
		return 0;

	return data[y * width + x];
}

void smoothFilter( unsigned char* dataIn, unsigned char* dataOut, int width, int height )
{

	for (int y=0; y<height; y++)
	{
		for (int x=0; x<width; x++)
		{
			int cornerAverage = 0;
			cornerAverage += smoothFilterDataValue( dataIn, x - 1, y - 1, width, height );
			cornerAverage += smoothFilterDataValue( dataIn, x + 1, y - 1, width, height );
			cornerAverage += smoothFilterDataValue( dataIn, x - 1, y + 1, width, height );
			cornerAverage += smoothFilterDataValue( dataIn, x + 1, y + 1, width, height );
			cornerAverage /= 16;

			int sideAverage = 0;
			sideAverage += smoothFilterDataValue( dataIn, x - 1, y, width, height );
			sideAverage += smoothFilterDataValue( dataIn, x + 1, y, width, height );
			sideAverage += smoothFilterDataValue( dataIn, x, y - 1, width, height );
			sideAverage += smoothFilterDataValue( dataIn, x, y + 1, width, height );
			sideAverage /= 8;

			int center = smoothFilterDataValue( dataIn, x, y, width, height );
			center /= 4;

			dataOut[y * width + x] = cornerAverage + sideAverage + center;
		}
	}
}

#ifdef WIN32

// from GP Gems 3
void enableFPExceptions()
{
	int i = _controlfp( 0, 0 );
	i &= ~(EM_ZERODIVIDE | EM_OVERFLOW | EM_INVALID );
	_controlfp( i, MCW_EM );

	if (0)	// test
	{
		volatile MPBFLOAT x = 1.0f;
		volatile MPBFLOAT y = 0.0f;
		for (int i=0; i<10; i++)
		{
			x = x/y;
		}
	}
}

#endif


MPBFLOAT round( MPBFLOAT d )
{
	MPBFLOAT r = d; 
	if (d - (int)d > .5f) r = ceil( d );
	return r;
}

// x should be between 0.0 and 1.0
MPBFLOAT lerp( MPBFLOAT x, MPBFLOAT a, MPBFLOAT b )
{
	return a * (1.0f - x) + b * x;
}

void centerWindow( HWND hwnd )
{
	UINT nScreenWidth = ::GetSystemMetrics(SM_CXSCREEN);
	UINT nScreenHeight = ::GetSystemMetrics(SM_CYSCREEN);

	// Determine dialog box dimensions
	RECT r;
	GetWindowRect( hwnd, &r );
	int width = r.right - r.left;
	int height = r.bottom - r.top;

	// Center the window on the screen
	SetWindowPos( 
		hwnd, 
		NULL, 
		(nScreenWidth / 2) - (width / 2),
		(nScreenHeight / 2) - (height / 2),
		0,
		0,
		SWP_NOSIZE | SWP_NOZORDER
	);
}

/*
DWORD GetDllVersion( LPCTSTR lpszDllName )
{
	DWORD dwVersion = 0;

	HINSTANCE hinstDll = LoadLibrary( lpszDllName );

	if (hinstDll)
	{
		DLLGETVERSIONPROC pDllGetVersion;

		pDllGetVersion = (DLLGETVERSIONPROC)GetProcAddress( hinstDll, "DllGetVersion" );

		// Because some DLLs may not implement this function, you
		// must test for it explicitly. Depending on the particular 
		// DLL, the lack of a DllGetVersion function may
		// be a useful indicator of the version.

		if (pDllGetVersion)
		{
			DLLVERSIONINFO dvi;
			HRESULT hr;

			ZeroMemory( &dvi, sizeof(dvi) );
			dvi.cbSize = sizeof(dvi);

			hr = (*pDllGetVersion)(&dvi);

			if (SUCCEEDED(hr))
			{
				dwVersion = PACKVERSION( dvi.dwMajorVersion, dvi.dwMinorVersion );
			}
		}

		FreeLibrary( hinstDll );
	}

	return dwVersion;
}
*/






