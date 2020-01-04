#ifndef __MPBDIRECTINPUT_H__
#define __MPBDIRECTINPUT_H__

#include "MPBLibrary.h"

class MPBDirectInput
{

public:

	static void init();
	static void acquireDevices();
	static void unacquireDevices();
	static bool getMouseState( DIMOUSESTATE& mouseState );

private:

	static IDirectInput8*				m_directInput;
	static IDirectInputDevice8*		m_didKeyboard;
	static IDirectInputDevice8*		m_didMouse;
	static bool								m_diReady;
};


#endif
