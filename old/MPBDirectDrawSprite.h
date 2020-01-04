#ifndef __MPBDIRECTDRAWSPRITE_H__
#define __MPBDIRECTDRAWSPRITE_H__

#include "MPBLibrary.h"
#include "MPBDirectDrawBitmap.h"

class MPBDirectDrawSprite
{
protected:

	vector<int>				m_bitmapVector;
	int						m_bitmapIndex;
	double					m_xPos;
	double					m_yPos;
	double					m_speed;
	double					m_rotation;		// in radians
	double					m_radius;
	COLORREF					m_colorRef;
	COLORREF					m_colorRefTemp;

public:
	
	MPBDirectDrawSprite();
	virtual ~MPBDirectDrawSprite();

	void					updateFrame();
	virtual void		addBitmap( int resourceID, double degreesOfRotation );
	virtual void		draw();
	void					setRadius( double v );
	double				getRadius() { return m_radius; }
	virtual void		drawPrimitive( HDC hdc );
	virtual void		move( const MPBVector& direction );
	virtual void		moveForwards();
	virtual void		moveBackwards();
	virtual void		spinLeft( int times );
	virtual void		spinRight( int times );
	virtual void		setXPos( double x );
	virtual void		setYPos( double y );
	virtual double		getXPos() { return m_xPos; }
	virtual double		getYPos() { return m_yPos; }
	virtual void		setSpeed( double speed );
	virtual void		setBitmap( int index );
	virtual int			getBitmapIndex() { return m_bitmapIndex; }
	virtual int			getWidth();
	virtual int			getHeight();
	void					setColor( COLORREF cr );
	void					setTempColor( COLORREF cr );
};



#endif __MPBDIRECTDRAWSPRITE_H__
