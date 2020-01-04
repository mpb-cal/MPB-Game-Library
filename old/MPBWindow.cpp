
#include "MPBWindow.h"
#include "mmgr.h"



LRESULT CALLBACK WndProc( HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam );

void MPBWindow::init( HINSTANCE hInstance )
{
	WNDCLASSEX wcex;
	memset( &wcex, 0, sizeof( WNDCLASSEX ) );

	wcex.cbSize				= sizeof(WNDCLASSEX);
	wcex.style				= CS_HREDRAW | CS_VREDRAW;// |  CS_OWNDC;
	wcex.lpfnWndProc		= WndProc;
	wcex.cbClsExtra		= 0;
	wcex.cbWndExtra		= 0;
	wcex.hInstance			= hInstance;
	wcex.hCursor			= 0;//LoadCursor(NULL, IDC_ARROW);
	wcex.lpszMenuName		= 0;//(LPCSTR)IDR_MENU1;
	wcex.hbrBackground	= 0;//(HBRUSH)(COLOR_WINDOW+1);
	wcex.lpszClassName	= "MPB_WINDOW";

	RegisterClassEx( &wcex );

	DWORD dwStyle;
	DWORD dwExStyle;

	dwExStyle = 0;//WS_EX_APPWINDOW | WS_EX_WINDOWEDGE;
	dwStyle = WS_OVERLAPPEDWINDOW | WS_CLIPSIBLINGS | WS_CLIPCHILDREN;

	g_hwnd = CreateWindowEx(
		dwExStyle,
		wcex.lpszClassName,
		"MPBWindow",
		dwStyle,
		0,
		0,
		CW_USEDEFAULT,
		CW_USEDEFAULT,
		NULL,
		NULL,
		hInstance,
		NULL
	);

	MPB_ASSERT( g_hwnd );

	ShowWindow( g_hwnd, SW_SHOWNORMAL );
	UpdateWindow( g_hwnd );
}

LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	if (message == WM_COMMAND)
	{
		int wmId    = LOWORD(wParam); 
		int wmEvent = HIWORD(wParam); 

		// Parse the menu selections:
//		if (wmId == ID_FILE_EXIT)
//		{
//		}
//		else 
		{
			return DefWindowProc( hWnd, message, wParam, lParam );
		}
	}
	else if (message == WM_CREATE)
	{
	}
	else if (message == WM_SIZE)
	{
	}
	else if (message == WM_ACTIVATE)
	{
	}
	else if (message == WM_PAINT)
	{
		PAINTSTRUCT ps;
		HDC hdc = BeginPaint( hWnd, &ps );

		RECT r;
		GetClientRect( hWnd, &r );
//		DrawText( hdc, szHello, strlen(szHello), &r, DT_CENTER );

		EndPaint( hWnd, &ps );
	}
	else if (message == WM_KEYDOWN)
	{
		if (wParam == VK_ESCAPE) 
		{
		}
	}
	else if (message == WM_DESTROY)
	{
		PostQuitMessage(0);
	}
	else 
	{
		return DefWindowProc( hWnd, message, wParam, lParam );
	}

	return 0;
}

WPARAM MPBWindow::messageLoop()
{
	MSG msg;
	while (TRUE)
	{
		if (PeekMessage( &msg, NULL, 0, 0, PM_REMOVE ))
		{
//			if (!IsWindow( m_hDialog ) || !IsDialogMessage( m_hDialog, &msg ))
			{
				if (msg.message == WM_QUIT) break;
				TranslateMessage(&msg);
				DispatchMessage(&msg);
			}
		}
		else
		{
		}
	}

	return msg.wParam;
}

