
#include "MPBDirectDraw.h"
#include "MPBDirectDrawSprite.h"
#include "MPBCollision.h"
#include "MPBPlane.h"
#include "MPBGlobal.h"
#include "MPBMessageWindow.h"
#include "MPBGameEntities.h"
#include "MPBFrameCounter.h"


const double DRAW_SCALE = 1.0;

MPBDirectDrawSprite::MPBDirectDrawSprite()
{
	m_speed = 0;
	m_xPos = 0;
	m_yPos = 0;
	m_bitmapIndex = -1;
	m_radius = 1;		// in meters
	m_colorRef = 0x00FFFFFF;
	m_rotation = 0;
}

MPBDirectDrawSprite::~MPBDirectDrawSprite()
{
}

void MPBDirectDrawSprite::setXPos( double x ) 
{ 
	m_xPos = x; 
}

void MPBDirectDrawSprite::setYPos( double y ) 
{ 
	m_yPos = y; 
}

void MPBDirectDrawSprite::setRadius( double v ) 
{ 
	m_radius = v; 
}

void MPBDirectDrawSprite::setSpeed( double speed ) { m_speed = speed; }
void MPBDirectDrawSprite::setBitmap( int index ) { m_bitmapIndex = index; }

int MPBDirectDrawSprite::getWidth()
{
	return MPBDirectDraw::getBitmap( m_bitmapVector[ m_bitmapIndex ] )->getWidth();
}

int MPBDirectDrawSprite::getHeight()
{
	return MPBDirectDraw::getBitmap( m_bitmapVector[ m_bitmapIndex ] )->getHeight();
}

void MPBDirectDrawSprite::addBitmap( int resourceID, double degreesOfRotation )
{
	m_bitmapVector.push_back( resourceID );
	m_bitmapIndex++;
}

void MPBDirectDrawSprite::setColor( COLORREF cr )
{
	m_colorRef = cr;
}

void MPBDirectDrawSprite::setTempColor( COLORREF cr )
{
	m_colorRefTemp = cr;
}

void MPBDirectDrawSprite::draw()
{
	MPBDirectDraw::drawBitmap( m_bitmapVector[ m_bitmapIndex ], m_xPos * DRAW_SCALE, m_yPos * DRAW_SCALE );
}

void MPBDirectDrawSprite::drawPrimitive( HDC hdc )
{
	SelectObject( hdc, GetStockObject( BLACK_PEN ) );
	SelectObject( hdc, CreateSolidBrush( m_colorRefTemp ) );

	Ellipse(	hdc,
				m_xPos * DRAW_SCALE - m_radius * DRAW_SCALE,
				m_yPos * DRAW_SCALE - m_radius * DRAW_SCALE,
				m_xPos * DRAW_SCALE + m_radius * DRAW_SCALE,
				m_yPos * DRAW_SCALE + m_radius * DRAW_SCALE	);

	MPBVector moveDirection( 0, -m_radius * DRAW_SCALE, 0 );
	moveDirection.rotateZRadians( m_rotation );
	moveDirection.draw2D( &hdc, m_xPos * DRAW_SCALE, m_yPos * DRAW_SCALE );

	m_colorRefTemp = m_colorRef;
}

void MPBDirectDrawSprite::moveForwards()
{
	MPBVector moveDirection( 0, -m_speed, 0 );	// point straight up
	moveDirection.rotateZRadians( m_rotation );
	move( moveDirection );
}

void MPBDirectDrawSprite::moveBackwards()
{
	MPBVector moveDirection( 0, -m_speed, 0 );	// point straight up
	moveDirection.rotateZRadians( m_rotation );
	moveDirection.rotateZ( 180 );
	move( moveDirection );
}

void MPBDirectDrawSprite::move( const MPBVector& direction )
{
	MPBVector startPosition( m_xPos, m_yPos, 0 );
	MPBVector destPosition(	m_xPos + direction.x,
									m_yPos + direction.y,
									0	);
	MPBVector newDestPosition;
	MPBVector travelVector = destPosition - startPosition;
	MPBVector velocityVector = travelVector;
	double travelDistance = velocityVector.getLength();
	velocityVector.normalize();
	vector<Collision*> collisionList;
	Collision* collision;
	bool collided = false;

	VECTOR_LOOP( MPBGameEntities::spriteVector, MPBDirectDrawSprite* )
	{
		if (*itr == this) continue;
		MPBVector position2( (*itr)->m_xPos, (*itr)->m_yPos, 0 );
		(*itr)->setColor( 0x00FFFFFF );

		if (collisionSphereSphere(	startPosition,
											destPosition,
											m_radius,
											position2,
											(*itr)->m_radius,
											velocityVector,
											travelDistance,
											collision	))
		{
			destPosition = collision->m_newDestination;
			(*itr)->setTempColor( 0x000000FF );
			break;
		}
	}}

	VECTOR_LOOP( MPBGameEntities::planeVector, Plane* )
	{
		if (collisionSpherePlane(	*itr, 
											startPosition, 
											destPosition, 
											velocityVector,
											travelVector,
											travelDistance,
											m_radius,
											collision	))
		{
			collisionList.push_back( collision );
			collided = true;
		}
	}}

	if (collided)
	{
		// find closest collision
		collision = *(min_element( 
			collisionList.begin(), 
			collisionList.end(),
			collisionLessThan ));

		// move as far as we can
		setXPos( collision->m_newDestination.x );
		setYPos( collision->m_newDestination.y );
	}
	else
	{
		// no collision; move to desired position
		setXPos( destPosition.x );
		setYPos( destPosition.y );
	}
}

void MPBDirectDrawSprite::spinLeft( int times )
{
	m_rotation -= 1 * times;
}

void MPBDirectDrawSprite::spinRight( int times )
{
	m_rotation += 1 * times;
}

void MPBDirectDrawSprite::updateFrame()
{
}







