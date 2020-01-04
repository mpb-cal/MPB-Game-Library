
#include "MPBDirectdraw.h"
#include "MPBGlobal.h"
#include "MPBDirectDrawSprite.h"
#include "MPBPlane.h"
#include "MPBGameEntities.h"
#include "MPBPhysics.h"

#pragma comment(lib, "ddraw.lib")


IDirectDraw*								MPBDirectDraw::directDraw = NULL;
IDirectDrawSurface*						MPBDirectDraw::backSurface = NULL;
bool											MPBDirectDraw::m_fullscreen;
int											MPBDirectDraw::screenWidth;
int											MPBDirectDraw::screenHeight;
IDirectDrawSurface*						MPBDirectDraw::m_primarySurface = NULL;
IDirectDrawPalette*						MPBDirectDraw::m_palette = NULL;
IDirectDrawClipper*						MPBDirectDraw::m_clipper = NULL;
map<int,MPBDirectDrawBitmap*>			MPBDirectDraw::m_bitmapMap;
MPBDirectDrawSprite*						MPBDirectDraw::m_backgroundSprite = NULL;

MPBDirectDraw::MPBDirectDraw(	HWND hwnd, 
										bool fullscreen, 
										int p_screenWidth, 
										int p_screenHeight	)
{
	directDraw = NULL;
	backSurface = NULL;
	m_primarySurface = NULL;
	m_palette = NULL;
	m_clipper = NULL;
	m_fullscreen = fullscreen;
	screenWidth = p_screenWidth;
	screenHeight = p_screenHeight;

	HRESULT hr;

	hr = DirectDrawCreate( NULL, &directDraw, NULL );
	_ASSERTE( hr == DD_OK );

//	hr = directDraw->QueryInterface( IID_IDirectDraw2, (LPVOID *)&directDraw2 ); 
//	_ASSERTE( hr == DD_OK );

	if (fullscreen)
	{
		hr = directDraw->SetCooperativeLevel( hwnd, DDSCL_EXCLUSIVE | DDSCL_FULLSCREEN );
		_ASSERTE( hr == DD_OK );

		hr = directDraw->SetDisplayMode( screenWidth, screenHeight, 8 );
		_ASSERTE( hr == DD_OK );

		DDSURFACEDESC surfaceDesc;

		memset( &surfaceDesc, 0, sizeof(surfaceDesc) );
		surfaceDesc.dwSize = sizeof( surfaceDesc );

		surfaceDesc.dwFlags = DDSD_CAPS | DDSD_BACKBUFFERCOUNT;
		surfaceDesc.ddsCaps.dwCaps = DDSCAPS_PRIMARYSURFACE | DDSCAPS_FLIP | DDSCAPS_COMPLEX;
		surfaceDesc.dwBackBufferCount = 1;

		directDraw->CreateSurface( &surfaceDesc, &m_primarySurface, NULL );

		DDSCAPS ddscaps;
		ddscaps.dwCaps = DDSCAPS_BACKBUFFER;
		m_primarySurface->GetAttachedSurface(&ddscaps, &backSurface);
	}
	else
	{
		hr = directDraw->SetCooperativeLevel( hwnd, DDSCL_NORMAL );
		_ASSERTE( hr == DD_OK );

		DDSURFACEDESC surfaceDesc;

		// primary surface
		memset( &surfaceDesc, 0, sizeof(surfaceDesc) );
		surfaceDesc.dwSize = sizeof( surfaceDesc );

		surfaceDesc.dwFlags = DDSD_CAPS;
		surfaceDesc.ddsCaps.dwCaps = DDSCAPS_PRIMARYSURFACE;

		directDraw->CreateSurface( &surfaceDesc, &m_primarySurface, NULL );

		// back surface
		memset( &surfaceDesc, 0, sizeof(surfaceDesc) );
		surfaceDesc.dwSize = sizeof( surfaceDesc );
		surfaceDesc.dwFlags = DDSD_CAPS | DDSD_HEIGHT | DDSD_WIDTH;
		surfaceDesc.ddsCaps.dwCaps = DDSCAPS_OFFSCREENPLAIN;
		surfaceDesc.dwWidth = screenWidth;
		surfaceDesc.dwHeight = screenHeight;

		directDraw->CreateSurface( &surfaceDesc, &backSurface, NULL );

		// clipper
		directDraw->CreateClipper( 0, &m_clipper, NULL );
		m_clipper->SetHWnd( 0, hwnd );
		m_primarySurface->SetClipper( m_clipper );

//		loadPalette( IDB_RUNNER );
	}
}

MPBDirectDraw::~MPBDirectDraw()
{
	DELETE_MAP( m_bitmapMap, int, MPBDirectDrawBitmap* );

	delete m_backgroundSprite;

	if (m_primarySurface != NULL)
	{
		m_primarySurface->Release();
		m_primarySurface = NULL;
	}

	if (backSurface != NULL)
	{
		backSurface->Release();
		backSurface = NULL;
	}

	if (directDraw != NULL)
	{
		directDraw->Release();
		directDraw = NULL;
	}

	if (m_palette != NULL)
	{
		m_palette->Release();
		m_palette = NULL;
	}

	if (m_clipper != NULL)
	{
//		m_clipper->Release();
//		m_clipper = NULL;
	}
}

void MPBDirectDraw::flip()
{
	HRESULT hr;

	if (m_fullscreen)
	{
		hr = m_primarySurface->Flip( NULL, DDFLIP_WAIT );
	}
	else
	{
		RECT rcRectSrc;
		RECT rcRectDest;
		POINT p;

		// first we need to figure out where on the primary surface our window lives
		p.x = 0; 
		p.y = 0;
		ClientToScreen( g_hwnd, &p );
		GetClientRect( g_hwnd, &rcRectDest );
		OffsetRect( &rcRectDest, p.x, p.y );
		SetRect( &rcRectSrc, 0, 0, screenWidth, screenHeight );
		hr = m_primarySurface->Blt( &rcRectDest, backSurface, &rcRectSrc, DDBLT_WAIT, NULL);
	}

	if (hr == DDERR_SURFACELOST)
	{
//			m_primarySurface->Restore();
//			backSurface->Restore();
//			m_backgroundSurface->Restore();

//			loadBitmap( "background.bmp", m_backgroundSurface );

//		LPDIRECTDRAWSURFACE spriteSurface = m_sprite->getSurface();
//		spriteSurface->Restore();
//		loadBitmap( "runner.bmp", spriteSurface );

//			LPDIRECTDRAWSURFACE cannonSurface = m_cannon->getSurface();
//			cannonSurface->Restore();
//			loadBitmap( "cannon.bmp", cannonSurface );
	}
}

void MPBDirectDraw::updateFrame()
{
	HDC hdc;
	backSurface->GetDC( &hdc );

	MPBPhysics::draw2D( hdc );

	VECTOR_LOOP( MPBGameEntities::spriteVector, MPBDirectDrawSprite* )
	{
		(*itr)->drawPrimitive( hdc );
	}}

	VECTOR_LOOP( MPBGameEntities::planeVector, Plane* )
	{
		(*itr)->draw2D( hdc );
	}}

	backSurface->ReleaseDC( hdc );

	flip();
	m_backgroundSprite->draw();
}

void MPBDirectDraw::addBitmap( int resourceID )
{
	m_bitmapMap[resourceID] = new MPBDirectDrawBitmap( resourceID, 0 );
}

void MPBDirectDraw::setBackgroundSprite( MPBDirectDrawSprite* sprite )
{
	m_backgroundSprite = sprite;
}

void MPBDirectDraw::drawBitmap( int resourceID, double x, double y )
{
	getBitmap( resourceID )->draw( x, y );
}

MPBDirectDrawBitmap* MPBDirectDraw::getBitmap( int resourceID )
{
	_ASSERTE( m_bitmapMap[resourceID] );
	return m_bitmapMap[resourceID];
}

