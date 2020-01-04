
#include "MPBDirectDrawBitmap.h"
#include "MPBGlobal.h"
#include "MPBDirectDraw.h"

MPBDirectDrawBitmap::MPBDirectDrawBitmap( int resourceID, int colorKey )
{
	m_resourceID = resourceID;

	HBITMAP hbm = (HBITMAP)LoadBitmap(
		g_hInstance, 
		MAKEINTRESOURCE( resourceID )
	);

	_ASSERTE( hbm );

	DDSURFACEDESC surfaceDesc;
	BITMAP bm;

	GetObject( hbm, sizeof(bm), &bm );

	m_width = bm.bmWidth;
	m_height = bm.bmHeight;

	ZeroMemory( &surfaceDesc, sizeof(surfaceDesc) );
	surfaceDesc.dwSize = sizeof(surfaceDesc);
	surfaceDesc.dwFlags = DDSD_CAPS | DDSD_HEIGHT |DDSD_WIDTH;
	surfaceDesc.ddsCaps.dwCaps = DDSCAPS_OFFSCREENPLAIN;
	surfaceDesc.dwWidth = bm.bmWidth;
	surfaceDesc.dwHeight = bm.bmHeight;

	MPBDirectDraw::directDraw->CreateSurface( &surfaceDesc, &m_surface, NULL );

/*
	int size = bm.bmWidth * bm.bmHeight;
	BYTE* pixels = new BYTE[ size ];
	GetBitmapBits( hbm, size, pixels );
	BYTE* newPixels = new BYTE[ size ];
	rotatePixels( bm.bmWidth, bm.bmHeight, pixels, newPixels, degreesOfRotation );
	SetBitmapBits( hbm, size, newPixels );
*/
	HDC hdcImage = CreateCompatibleDC( NULL );
	HBITMAP hbmOld = (HBITMAP)SelectObject( hdcImage, hbm );
	HDC hdc = NULL;

	// copy the image from the DC to the DD surface
	HRESULT hResult;
	hResult = m_surface->GetDC( &hdc );
/*	if (hResult == DDERR_DCALREADYCREATED) Game::writeLog( "Bitmap: DDERR_DCALREADYCREATED\n" );
	if (hResult == DDERR_GENERIC) Game::writeLog( "Bitmap: DDERR_GENERIC\n" );
	if (hResult == DDERR_INVALIDOBJECT) Game::writeLog( "Bitmap: DDERR_INVALIDOBJECT\n" );
	if (hResult == DDERR_INVALIDPARAMS) Game::writeLog( "Bitmap: DDERR_INVALIDPARAMS\n" );
	if (hResult == DDERR_INVALIDSURFACETYPE) Game::writeLog( "Bitmap: DDERR_INVALIDSURFACETYPE\n" );
	if (hResult == DDERR_SURFACELOST) Game::writeLog( "Bitmap: DDERR_SURFACELOST\n" );
	if (hResult == DDERR_UNSUPPORTED) Game::writeLog( "Bitmap: DDERR_UNSUPPORTED\n" );
	if (hResult == DDERR_WASSTILLDRAWING) Game::writeLog( "Bitmap: DDERR_WASSTILLDRAWING\n" );
*/
	_ASSERTE( hResult == DD_OK );

	BitBlt( hdc, 0, 0, bm.bmWidth, bm.bmHeight, hdcImage, 0, 0, SRCCOPY );
	m_surface->ReleaseDC( hdc );

	SelectObject( hdcImage, hbmOld );
	DeleteDC( hdcImage );
	DeleteObject( hbm );

	if (colorKey >= 0)
	{	
		DDCOLORKEY ddck;
		ddck.dwColorSpaceLowValue = colorKey;
		ddck.dwColorSpaceHighValue = colorKey;
		m_surface->SetColorKey( DDCKEY_SRCBLT, &ddck );
	}
}

MPBDirectDrawBitmap::~MPBDirectDrawBitmap()
{
	m_surface->Release();
	m_surface = NULL;
}

void MPBDirectDrawBitmap::draw( RECT& srcRect, int destX, int destY )
{
	MPBDirectDraw::backSurface->BltFast( 
		destX,
		destY,
		m_surface, 
		&srcRect,
		DDBLTFAST_SRCCOLORKEY | DDBLTFAST_WAIT	);
}

void MPBDirectDrawBitmap::draw( int x, int y )
{
	RECT srcRect;
	SetRect( &srcRect, 0, 0, m_width, m_height );
	draw( srcRect, x, y );
}

int MPBDirectDrawBitmap::getWidth()
{
	return m_width;
}

int MPBDirectDrawBitmap::getHeight()
{
	return m_height;
}


