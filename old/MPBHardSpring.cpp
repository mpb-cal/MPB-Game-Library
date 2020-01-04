
#include "MPBLibrary.h"
#include "MPBHardSpring.h"
#include "MPBPhysicsParticle.h"
#include "MPBOpenGL.h"
#include "mmgr.h"


// restLength in meters
MPBHardSpring::MPBHardSpring(	MPBPhysicsParticle* object1, 
										MPBPhysicsParticle* object2, 
										MPBFLOAT restLength	) :
	m_object1( object1 ),
	m_object2( object2 ),
	m_restLengthMin( restLength ),
	m_restLengthMax( restLength )
{
}

// restLength in meters
MPBHardSpring::MPBHardSpring(	MPBPhysicsParticle* object1, 
										MPBPhysicsParticle* object2, 
										MPBFLOAT restLengthMin,
										MPBFLOAT restLengthMax	) :
	m_object1( object1 ),
	m_object2( object2 ),
	m_restLengthMin( restLengthMin ),
	m_restLengthMax( restLengthMax )
{
}

MPBHardSpring::MPBHardSpring(	MPBPhysicsParticle* object1, 
										MPBPhysicsParticle* object2	) :
	m_object1( object1 ),
	m_object2( object2 )
{
	m_restLengthMin = m_restLengthMax = (m_object1->getPosition() - m_object2->getPosition()).getLength();
}

void MPBHardSpring::updateFrame()
{
	MPB_ASSERT( m_object1 && m_object2 );

	// spring

	MPBVector n = m_object2->getPosition() - m_object1->getPosition();
	MPBFLOAT distance = n.getLength();

	if (floatEQ( distance, 0 )) return;

	MPBFLOAT restLength;
	if (distance > m_restLengthMax) restLength = m_restLengthMax;
	else if (distance < m_restLengthMin) restLength = m_restLengthMin;
	else return;

	n.normalize();
	MPBFLOAT invMass1 = 1.0 / m_object1->getMass();
	MPBFLOAT invMass2 = 1.0 / m_object2->getMass();

	// mass ignored
//	MPBFLOAT magnitude = (distance - restLength) / distance;
//	MPBVector force = n * magnitude * .5;
//	m_object1->changePosition( force );
//	m_object2->changePosition( force );

	MPBFLOAT magnitude = (distance - restLength) / (distance * (invMass1 + invMass2));
	if (floatEQ( magnitude, 0 )) return;
	MPBVector force1 = n * magnitude * invMass1;
	MPBVector force2 = n * -magnitude * invMass2;

	m_object1->changePosition( force1 );
	m_object2->changePosition( force2 );
}

void MPBHardSpring::draw() const
{
	MPB_ASSERT( m_object1 && m_object2 );

	if (MPBOpenGL::is2D()) 
	{
		glPushAttrib( GL_TEXTURE_BIT );
		glDisable( GL_TEXTURE_2D );
	}
	else 
	{
		MPBOpenGL::bindTexture( "planeTexture" );
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

