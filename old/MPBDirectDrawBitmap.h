#ifndef __MPBDIRECTDRAWBITMAP_H__
#define __MPBDIRECTDRAWBITMAP_H__

#include "MPBLibrary.h"


class MPBDirectDrawBitmap
{
public:
	
	MPBDirectDrawBitmap( int resourceID, int colorKey );
	~MPBDirectDrawBitmap();

	void				draw( RECT& srcRect, int destX, int destY );
	void				draw( int x, int y );
	int				getWidth();
	int				getHeight();
	int				getResourceID() { return m_resourceID; }

protected:

	LPDIRECTDRAWSURFACE	m_surface;
	int						m_width;
	int						m_height;
	int						m_resourceID;
};


#endif __MPBDIRECTDRAWBITMAP_H__
