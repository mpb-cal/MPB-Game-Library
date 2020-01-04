#ifndef __MPBDIRECTDRAW_H__
#define __MPBDIRECTDRAW_H__

#include "MPBLibrary.h"

class MPBDirectDrawBitmap;
class MPBDirectDrawSprite;

class MPBDirectDraw
{
	public:

		MPBDirectDraw::MPBDirectDraw( HWND hwnd, bool fullscreen, int screenWidth, int screenHeight );
		MPBDirectDraw::~MPBDirectDraw();
		static void updateFrame();
		static void addBitmap( int resourceID );
		static void setBackgroundSprite( MPBDirectDrawSprite* sprite );
		static void drawBitmap( int resourceID, double x, double y );
		static MPBDirectDrawBitmap* getBitmap( int resourceID );

		static IDirectDraw*								directDraw;
		static IDirectDrawSurface*						backSurface;
		static int											screenWidth;
		static int											screenHeight;

	private:

		static void flip();

		static bool											m_fullscreen;
		static IDirectDrawSurface*						m_primarySurface;
		static IDirectDrawPalette*						m_palette;
		static IDirectDrawClipper*						m_clipper;
		static map<int,MPBDirectDrawBitmap*>		m_bitmapMap;
		static MPBDirectDrawSprite*					m_backgroundSprite;
};




#endif

