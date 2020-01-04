
#include "MPBLibrary.h"
#include "MPBSpring.h"
#include "MPBPhysics.h"
#include "MPBOpenGL.h"
#include "MPBPhysicsObject.h"
#include "mmgr.h"


// restLength in meters
// pullBoth == false: only robot2 will be acted on
MPBSpring::MPBSpring(	MPBPhysicsObject* object1, 
								MPBPhysicsObject* object2, 
								bool pullBoth, 
								MPBFLOAT restLength, 
								MPBFLOAT springConstant,
								MPBFLOAT dampingConstant ) :
	m_object1( object1 ),
	m_object2( object2 ),
	m_pullBoth( pullBoth ),
	m_restLength( restLength ),
	m_springConstant( springConstant ),
	m_dampingConstant( dampingConstant )
{
}

void MPBSpring::updateFrame()
{
	MPB_ASSERT( m_object1 && m_object2 );

	// spring

	MPBVector n = m_object2->getPosition() - m_object1->getPosition();
	MPBFLOAT length = n.getLength();
	MPBFLOAT magnitude = m_springConstant * (length - m_restLength);
	if (length)
	{
		n.normalize();
	}
	else 
	{
		n.set( 1, 0, 0 );
		length = 1;
	}

	if (m_pullBoth)
	{
		MPBVector force1 = n * magnitude;
		m_object1->applyForce( force1 );
	}

	MPBVector force2 = n * -magnitude;
	m_object2->applyForce( force2 );

	// damper

	MPBVector v;

	if (m_pullBoth) v = (m_object2->getVelocity() - m_object1->getVelocity()) * m_dampingConstant;
	else v = m_object2->getVelocity() * m_dampingConstant;

	if (m_pullBoth) m_object1->applyForce( v );

	v = v * -1;
	m_object2->applyForce( v );
}

void MPBSpring::draw() const
{
	MPB_ASSERT( m_object1 && m_object2 );

	if (MPBOpenGL::is2D()) 
	{
		glPushAttrib( GL_TEXTURE_BIT );
		glDisable( GL_TEXTURE_2D );
	}
	else 
	{
		MPBOpenGL::bindTexture( "font2" );
	}

	glColor4f( 0, 1, 0, 1 );
	glBegin( GL_LINES );
	glVertex3d( m_object1->getPosition().x, m_object1->getPosition().y, m_object1->getPosition().z );
	glVertex3d( m_object2->getPosition().x, m_object2->getPosition().y, m_object2->getPosition().z );
	glEnd();
	glColor4f( 1, 1, 1, 1 );

	if (MPBOpenGL::is2D()) 
	{
		glPopAttrib();
	}
}

void MPBSpring::draw2D( const void* hdc ) const
{
	SelectObject( *(HDC*)hdc, CreatePen( PS_SOLID, 1, 0x0000FFFF ) );

	POINT p;

	MoveToEx( *(HDC*)hdc, m_object1->getPosition().x, m_object1->getPosition().y, &p );
	LineTo( *(HDC*)hdc, m_object2->getPosition().x, m_object2->getPosition().y );
}

