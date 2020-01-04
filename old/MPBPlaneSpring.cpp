
#include "MPBPlaneSpring.h"
#include "MPBPhysicsParticle.h"
#include "MPBOpenGL.h"
#include "mmgr.h"


// constrain particle to stay in plane defined by normal and point in plane
MPBPlaneSpring::MPBPlaneSpring(	MPBPhysicsParticle* particle, 
											MPBPhysicsParticle* normalStart, 
											MPBPhysicsParticle* normalEnd,
											MPBPhysicsParticle* pointInPlane	) :
	m_particle( particle ),
	m_normalStart( normalStart ),
	m_normalEnd( normalEnd ),
	m_pointInPlane( pointInPlane ),
	m_planeLL( NULL ),
	m_planeLR( NULL ),
	m_planeUR( NULL )
{
}

// constrain particle to stay in front of or behind plane defined by 3 particles
MPBPlaneSpring::MPBPlaneSpring(	MPBPhysicsParticle* particle, 
											MPBPhysicsParticle* planeLL, 
											MPBPhysicsParticle* planeLR,
											MPBPhysicsParticle* planeUR,
											bool stayBehind	) :
	m_particle( particle ),
	m_normalStart( NULL ),
	m_normalEnd( NULL ),
	m_pointInPlane( NULL ),
	m_planeLL( planeLL ),
	m_planeLR( planeLR ),
	m_planeUR( planeUR ),
	m_stayBehind( stayBehind )
{
	MPB_ASSERT( m_stayBehind == false );
}

void MPBPlaneSpring::updateFrame()
{
	// stay in plane
	if (m_particle && m_normalStart && m_normalEnd && m_pointInPlane)
	{
		MPBVector normal( m_normalEnd->getPosition() - m_normalStart->getPosition() );
		normal.normalize();
		MPBFLOAT d = -normal.dotProduct( m_pointInPlane->getPosition() );
		MPBFLOAT distance = normal.dotProduct( m_particle->getPosition() ) + d;
		if (floatEQ( distance, 0 )) return;

		MPBVector force = normal * -distance;
		m_particle->changePosition( force );
	}
	// stay in front of plane
	else if (m_particle && m_planeLL && m_planeLR && m_planeUR)
	{
		Plane plane( m_planeLL->getPosition(), m_planeLR->getPosition(), m_planeUR->getPosition() );

		MPBFLOAT distance = plane.distanceToPoint( m_particle->getPosition() );
		if (floatGTE( distance, 0 )) return;

		MPBVector newPos;
		plane.lineIntersect(	m_particle->getPosition(), 
									m_particle->getPosition() + plane.getNormal(), 
									newPos	);

		m_particle->changePosition( newPos - m_particle->getPosition() );
	}
	else
	{
		MPB_ASSERT( 0 );
	}
}

void MPBPlaneSpring::draw() const
{
/*	if (MPBOpenGL::is2D()) 
	{
		glPushAttrib( GL_TEXTURE_BIT );
		glDisable( GL_TEXTURE_2D );
	}
	else 
	{
		MPBOpenGL::bindTexture( "font2" );
	}

	glColor4f( 0, 0, 1, 1 );
	glBegin( GL_LINES );
	glVertex3d( m_particle->getPosition().x, m_particle->getPosition().y, m_particle->getPosition().z );
	glVertex3d( m_normalStart->getPosition().x, m_normalStart->getPosition().y, m_normalStart->getPosition().z );
	glVertex3d( m_particle->getPosition().x, m_particle->getPosition().y, m_particle->getPosition().z );
	glVertex3d( m_normalEnd->getPosition().x, m_normalEnd->getPosition().y, m_normalEnd->getPosition().z );
	glVertex3d( m_normalStart->getPosition().x, m_normalStart->getPosition().y, m_normalStart->getPosition().z );
	glVertex3d( m_normalEnd->getPosition().x, m_normalEnd->getPosition().y, m_normalEnd->getPosition().z );
	glEnd();
	glColor4f( 1, 1, 1, 1 );

	if (MPBOpenGL::is2D()) 
	{
		glPopAttrib();
	}
*/
}

