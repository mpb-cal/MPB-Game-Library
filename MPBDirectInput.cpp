
#include "MPBDirectInput.h"
#include "MPBGlobal.h"
#include "mmgr.h"

#pragma comment(lib, "dinput8.lib")
#pragma comment(lib, "dxguid.lib")

IDirectInput8*					MPBDirectInput::m_directInput = NULL;
IDirectInputDevice8*			MPBDirectInput::m_didKeyboard = NULL;
IDirectInputDevice8*			MPBDirectInput::m_didMouse = NULL;
bool								MPBDirectInput::m_diReady = false;

void MPBDirectInput::init()
{
	HRESULT hr;

	MPB_ASSERT( g_hInstance );
	MPB_ASSERT( g_hwnd );

	hr = DirectInput8Create(	g_hInstance,
										DIRECTINPUT_VERSION,
										IID_IDirectInput8A,
										(LPVOID*)&m_directInput,
										NULL	);
	MPB_ASSERT( hr == DS_OK);

	hr = m_directInput->CreateDevice( GUID_SysKeyboard, &m_didKeyboard, NULL );
	MPB_ASSERT( hr == DS_OK);

	hr = m_didKeyboard->SetDataFormat( &c_dfDIKeyboard );
	MPB_ASSERT( hr == DS_OK);

	hr = m_didKeyboard->SetCooperativeLevel( g_hwnd, DISCL_FOREGROUND | DISCL_NONEXCLUSIVE );
	MPB_ASSERT( hr == DS_OK);

	hr = m_directInput->CreateDevice( GUID_SysMouse, &m_didMouse, NULL );
	MPB_ASSERT( hr == DS_OK);

	hr = m_didMouse->SetDataFormat( &c_dfDIMouse );
	MPB_ASSERT( hr == DS_OK);

	hr = m_didMouse->SetCooperativeLevel( g_hwnd, DISCL_FOREGROUND | DISCL_NONEXCLUSIVE );
	MPB_ASSERT( hr == DS_OK);

	HANDLE hMouse = CreateEvent( 0, 0, 0, 0 );

	MPB_ASSERT( hMouse );

	hr = m_didMouse->SetEventNotification( hMouse );
	MPB_ASSERT( hr == DS_OK);

	m_diReady = true;
	acquireDevices();
}

// call when processing WM_ACTIVATE and HIWORD(wParam) == 0
void MPBDirectInput::acquireDevices()
{
	if (!m_diReady) return;

	HRESULT hr;

	hr = m_didKeyboard->Acquire();
//	MPB_ASSERT( hr == DS_OK);

	hr = m_didMouse->Acquire();
//	MPB_ASSERT( hr == DS_OK);
}

// call when processing WM_ACTIVATE and HIWORD(wParam) != 0
void MPBDirectInput::unacquireDevices()
{
	if (!m_diReady) return;

	HRESULT hr;

	hr = m_didKeyboard->Unacquire();
//	MPB_ASSERT( hr == DS_OK);

	hr = m_didMouse->Unacquire();
//	MPB_ASSERT( hr == DS_OK);
}

// returns false on failure
bool MPBDirectInput::getMouseState( DIMOUSESTATE& mouseState )
{
	if (FAILED( m_didMouse->GetDeviceState( sizeof mouseState, (LPVOID)&mouseState ) ))
		return false;

	return true;
}











