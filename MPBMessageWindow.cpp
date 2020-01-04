
#include "MPBGlobal.h"
#include "MPBMessageWindow.h"
#include "mmgr.h"

HWND								MPBMessageWindow::m_hwnd = 0;
int								MPBMessageWindow::m_messageTextStart;
MPBList							MPBMessageWindow::m_messageText;


LRESULT CALLBACK WndProc_MPBMessageWindow( HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam )
{
	if (message == WM_PAINT)
	{
		HDC hdc = GetDC( hwnd );
		PAINTSTRUCT ps;
		BeginPaint( hwnd, &ps );

		RECT rect;
		GetClientRect( hwnd, &rect );

		SelectObject( hdc, GetStockObject(GRAY_BRUSH) );
		Rectangle( hdc, rect.left, rect.top, rect.right, rect.bottom );

		char buf[10000] = "";
		for (int i=MPBMessageWindow::m_messageTextStart; i<MPBMessageWindow::m_messageText.size(); i++)
		{
			MPB_ASSERT( strlen( buf ) < 10000 );
			strcat( buf, (char*)MPBMessageWindow::m_messageText[i] );
		}

		DrawText( hdc, buf, strlen( buf ), &rect, 0 );

		EndPaint( hwnd, &ps );

		return 0;
	}
	else if (message == WM_KEYDOWN)
	{
		if (wParam == VK_ESCAPE) PostQuitMessage( 0 );

		return 0;
	}
	else if (message == WM_SIZE)
	{
//		RECT rect;
//		GetWindowRect( hwnd, &rect );
//		char buf[256];
//		sprintf( buf, "width=%d, height=%d\n", LOWORD(lParam), HIWORD(lParam) );
//		writeMessage( buf );
	}
	else if (message == WM_CLOSE)
	{
		ShowWindow( hwnd, SW_HIDE );

		return 0;
	}
	else if (message == WM_DESTROY)
	{
//		PostQuitMessage( 0 );

		return 0;
	}

	return DefWindowProc( hwnd, message, wParam, lParam );
}


bool MPBMessageWindow::init( bool hide )
{
//	SINGLETON_FUNCTION;

	WNDCLASSEX wcex;
	memset( &wcex, 0, sizeof(WNDCLASSEX) );
	wcex.cbSize				= sizeof(WNDCLASSEX);
	wcex.style				= CS_HREDRAW | CS_VREDRAW;
	wcex.lpfnWndProc		= (WNDPROC)WndProc_MPBMessageWindow;
	wcex.cbClsExtra		= 0;
	wcex.cbWndExtra		= 0;
	wcex.hInstance			= g_hInstance;
	wcex.hIcon				= 0;
	wcex.hCursor			= LoadCursor(NULL, IDC_ARROW);
	wcex.hbrBackground	= (HBRUSH)(COLOR_WINDOW+1);
	wcex.lpszMenuName		= 0;
	wcex.lpszClassName	= "MPBMessageWindow";
	wcex.hIconSm			= 0;

	RegisterClassEx( &wcex );

	m_hwnd = CreateWindowEx(
		0,
		"MPBMessageWindow",
		"Message Window",
		WS_OVERLAPPEDWINDOW | WS_HSCROLL | WS_VSCROLL,
		800,
		10,
		500,
		850,
		NULL,
		NULL,
		g_hInstance,
		NULL
	);

	if (!m_hwnd) 
	{ 
		writeLastError( "MPBMessageWindow::init(): CreateWindowEx()" );
		return false; 
	}

	if (!hide) ShowWindow( m_hwnd, SW_SHOWNORMAL );
	UpdateWindow( m_hwnd );

	return true;
}

void MPBMessageWindow::cleanUp()
{
	clearMessageText();

	if (m_hwnd && !DestroyWindow( m_hwnd ))
	{
	}

	m_hwnd = NULL;
}

void MPBMessageWindow::writeMessage()
{
	writeMessage( g_logBuf );
}

void MPBMessageWindow::writeMessage( char* msg )
{
	if (m_hwnd == 0) return;

	char* buf = new char[ strlen( msg ) + 2 ];
	strcpy( buf, msg );
	strcat( buf, "\n" );
	m_messageText.push_back( buf );
	if (m_messageText.size() > 1000) clearMessageText();
	m_messageTextStart = m_messageText.size() - 40;
	if (m_messageTextStart < 0) m_messageTextStart = 0;
	if (m_hwnd) 
	{
		InvalidateRect( m_hwnd, NULL, 0 );
		UpdateWindow( m_hwnd );
	}
}

void MPBMessageWindow::clearMessageText()
{
	if (m_hwnd == 0) return;

	MPB_LIST_LOOP( m_messageText, s, char* )
	{
		delete[] s;
	}

//	for (int i=0; i<m_messageText.size(); i++)
//	{
//		NULLIFY_ARRAY( m_messageText[i] );
//	}
//
	m_messageText.clear();
}

