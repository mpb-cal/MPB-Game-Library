#ifndef __MPBMESSAGEWINDOW_H__
#define __MPBMESSAGEWINDOW_H__

#include "MPBLibrary.h"
#include "MPBList.h"

class MPBMessageWindow
{

public:

	static bool init( bool hide );
	static void cleanUp();
	static void setActive();
	static HWND getWindow() { return m_hwnd; }

	static void writeMessage();
	static void writeMessage( char* msg );
	static void clearMessageText();

private:

	static HWND								m_hwnd;
	static int								m_messageTextStart;
	static MPBList							m_messageText;		// char*

	friend LRESULT CALLBACK WndProc_MPBMessageWindow( HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam );

};


#endif



