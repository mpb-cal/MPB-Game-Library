
#include "MPBGlobal.h"
#include "MPBConsoleWindow.h"
#include "MPBList.h"
#include "mmgr.h"

HWND							MPBConsoleWindow::m_hwnd = 0;
int							MPBConsoleWindow::m_index = 0;

MPBList				f_consoleText;		// char*

const int CONSOLE_LINES = 100;

LRESULT CALLBACK WndProc_MPBConsoleWindow( HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam )
{
	int wmId, wmEvent;
	PAINTSTRUCT ps;
	HDC hdc;
	HFONT hfnt;
	HFONT hOldFont;

	if (message == WM_COMMAND)
	{
		wmId    = LOWORD(wParam);
		wmEvent = HIWORD(wParam);
	}
	else if (message == WM_KEYDOWN)
	{
	}
	else if (message == WM_KEYUP)
	{
	}
	else if (message == WM_ACTIVATE)
	{
	}
	else if (message == WM_SIZE)
	{
	}
	else if (message == WM_PAINT)
	{
		hdc = BeginPaint( hwnd, &ps );

		hfnt = (HFONT)GetStockObject( ANSI_FIXED_FONT );
		hOldFont = (HFONT)SelectObject( hdc, hfnt );

		RECT rt;
		GetClientRect( hwnd, &rt );

		MPB_LIST_LOOP( f_consoleText, line, char* )
		{
			DrawText( hdc, line, strlen(line), &rt, DT_LEFT );
			rt.top += 15;
			rt.bottom += 15;
		}

		SelectObject( hdc, hOldFont );
		EndPaint( hwnd, &ps );
	}
	else if (message == WM_DESTROY)
	{
//		PostQuitMessage( 0 );
	}
	else
	{
		return DefWindowProc( hwnd, message, wParam, lParam );
	}

	return 0;
}


bool MPBConsoleWindow::init()
{
//	SINGLETON_FUNCTION;

	for (int i=0; i<CONSOLE_LINES; i++) 
	{
		char* line = new char[1000];
		strcpy( line, "" );
		f_consoleText.push_back( line );
	}

	WNDCLASSEX wcex;
	memset( &wcex, 0, sizeof(WNDCLASSEX) );
	wcex.cbSize				= sizeof(WNDCLASSEX);
	wcex.style				= CS_HREDRAW | CS_VREDRAW | CS_OWNDC;
	wcex.lpfnWndProc		= (WNDPROC)WndProc_MPBConsoleWindow;
	wcex.cbClsExtra		= 0;
	wcex.cbWndExtra		= 0;
	wcex.hInstance			= g_hInstance;
	wcex.hIcon				= 0;
	wcex.hCursor			= 0;
	wcex.hbrBackground	= (HBRUSH)(COLOR_WINDOW+1);
	wcex.lpszMenuName		= 0;
	wcex.lpszClassName	= "MPBConsoleWindow";
	wcex.hIconSm			= 0;

	RegisterClassEx( &wcex );

	DWORD dwStyle;
	DWORD dwExStyle;

	dwExStyle = WS_EX_APPWINDOW | WS_EX_WINDOWEDGE;
	dwStyle = WS_OVERLAPPEDWINDOW | WS_CLIPSIBLINGS | WS_CLIPCHILDREN;

	m_hwnd = CreateWindowEx(
		dwExStyle,
		"MPBConsoleWindow",
		"Console Window",
		dwStyle,
		0,
		600,
		1200,
		200,
		NULL,
		NULL,
		g_hInstance,
		NULL
	);

	if (!m_hwnd) 
	{ 
		writeLastError( "MPBConsoleWindow::init(): CreateWindowEx()" );
		return false; 
	}

	ShowWindow( m_hwnd, SW_SHOWNORMAL );
	UpdateWindow( m_hwnd );

	return true;
}

void MPBConsoleWindow::cleanUp()
{
	if (m_hwnd == 0) return;

	DestroyWindow( m_hwnd );

	MPB_LIST_LOOP( f_consoleText, s, char* )
	{
		delete[] s;
	}

	f_consoleText.clear();

	m_hwnd = NULL;
}

void MPBConsoleWindow::startText()
{
	m_index = 0;
}

void MPBConsoleWindow::endText()
{
	if (m_hwnd == 0) return;
	InvalidateRect( m_hwnd, NULL, TRUE );
}

char* MPBConsoleWindow::nextLine()
{
	MPB_ASSERT( m_hwnd );
	return (char*)f_consoleText[ m_index++ ];
}



