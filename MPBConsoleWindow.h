#ifndef __MPBCONSOLEWINDOW_H__
#define __MPBCONSOLEWINDOW_H__

#include "MPBLibrary.h"

class MPBConsoleWindow
{

public:

	static bool init();
	static void cleanUp();
	static void startText();
	static void endText();
	static char* nextLine();

private:

	static HWND								m_hwnd;
	static int								m_index;

	friend LRESULT CALLBACK WndProc_MPBConsoleWindow( HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam );

};


#endif



