
#include "MPBLibrary.h"
#include "MPBHardSpringAngular.h"
#include "MPBPhysicsParticle.h"
#include "MPBOpenGL.h"
#include "mmgr.h"


// restAngle in radians
MPBHardSpringAngular::MPBHardSpringAngular(	MPBPhysicsParticle* object1, 
															MPBPhysicsParticle* object2, 
															MPBPhysicsParticle* objectHinge, 
															MPBFLOAT restAngle	) :
	m_object1( object1 ),
	m_object2( object2 ),
	m_objectHinge( objectHinge ),
	m_restAngleMin( restAngle ),
	m_restAngleMax( restAngle )
{
}

// restAngle in radians
MPBHardSpringAngular::MPBHardSpringAngular(	MPBPhysicsParticle* object1, 
															MPBPhysicsParticle* object2, 
															MPBPhysicsParticle* objectHinge, 
															MPBFLOAT restAngleMin,
															MPBFLOAT restAngleMax	) :
	m_object1( object1 ),
	m_object2( object2 ),
	m_objectHinge( objectHinge ),
	m_restAngleMin( restAngleMin ),
	m_restAngleMax( restAngleMax )
{
}

/*
MPBHardSpringAngular::MPBHardSpringAngular(	MPBPhysicsParticle* object1, 
															MPBPhysicsParticle* object2,
															MPBPhysicsParticle* objectHinge	) :
	m_object1( object1 ),
	m_object2( object2 ),
	m_objectHinge( objectHinge )
{
	MPB_ASSERT( 0 );
//	m_restAngleMin = m_restAngleMax = (m_object1->getPosition() - m_object2->getPosition()).getLength();
}
*/

void MPBHardSpringAngular::updateFrame()
{
	MPB_ASSERT( m_object1 && m_object2 && m_objectHinge );

	MPBVector n1 = m_object1->getPosition() - m_objectHinge->getPosition();
	MPBVector n2 = m_object2->getPosition() - m_objectHinge->getPosition();
	n1.normalize();
	n2.normalize();
	MPBFLOAT angle = acos( n1.dotProduct( n2 ) );

	MPBFLOAT restAngle;
	if (angle > m_restAngleMax) restAngle = m_restAngleMax;
	else if (angle < m_restAngleMin) restAngle = m_restAngleMin;
	else return;

	MPBFLOAT magnitude = .5 * (angle - restAngle) / angle;

	MPBVector axis = n1.crossProduct( n2 );
	MPBQuaternion q;

	q.fromAxisAngle( axis, magnitude );
	MPBVector n1_new = n1;
	q.rotateVector( n1_new );
	m_object1->changePosition( n1_new - n1 );

	q.fromAxisAngle( axis, -magnitude );
	MPBVector n2_new = n2;
	q.rotateVector( n2_new );
	m_object2->changePosition( n2_new - n2 );
}

void MPBHardSpringAngular::draw() const
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

