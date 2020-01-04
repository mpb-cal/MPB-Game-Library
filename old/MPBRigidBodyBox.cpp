
#include "MPBRigidBodyBox.h"
#include "MPBGameEntities.h"
#include "MPBHardSpring.h"
#include "MPBHardSpringAngular.h"
#include "MPBPlaneSpring.h"
#include "MPBPhysics.h"
#include "MPBPhysicsParticle.h"
#include "MPBShape.h"
#include "MPBMatrix.h"
#include "MPBPlane.h"
#include "MPBOpenGL.h"
#include "mmgr.h"


/*

		7			6
	4			5

		3			2
	0			1
*/

MPBRigidBodyBox::MPBRigidBodyBox() :
	MPBRigidBody()
{
}

MPBRigidBodyBox::~MPBRigidBodyBox()
{
}

void MPBRigidBodyBox::setCorners( const MPBVector& NLL, const MPBVector& FUR )
{
	MPBAABB::setCorners( NLL, FUR );

	MPB_LIST_LOOP( m_particles, p, MPBPhysicsParticle* )
	{
		MPBPhysics::removeParticle( p );
	}

	MPB_LIST_DELETE( m_particles, MPBPhysicsParticle* );

	m_particles.push_back( new MPBPhysicsParticle( getNLL() ) );
	m_particles.push_back( new MPBPhysicsParticle( getNLR() ) );
	m_particles.push_back( new MPBPhysicsParticle( getFLR() ) );
	m_particles.push_back( new MPBPhysicsParticle( getFLL() ) );
	m_particles.push_back( new MPBPhysicsParticle( getNUL() ) );
//	m_particles.push_back( new MPBPhysicsParticle( getNUR() ) );
//	m_particles.push_back( new MPBPhysicsParticle( getFUR() ) );
//	m_particles.push_back( new MPBPhysicsParticle( getFUL() ) );
	
	{MPB_LIST_LOOP( m_particles, p, MPBPhysicsParticle* )
	{
		p->setPlaneCollisionSound( "leftfoot.wav" );
		p->setMass( 200 );
		p->setGravity( 0 );
	}}

	getParticle( 2 )->setInfiniteMass( true );

	getParticle( 0 )->addSpring( getParticle( 1 ) );
	getParticle( 1 )->addSpring( getParticle( 2 ) );
	getParticle( 2 )->addSpring( getParticle( 3 ) );
	getParticle( 3 )->addSpring( getParticle( 0 ) );

	getParticle( 0 )->addSpring( getParticle( 2 ) );
	getParticle( 1 )->addSpring( getParticle( 3 ) );

	getParticle( 0 )->addSpring( getParticle( 4 ) );

/*	getParticle( 4 )->addSpring( getParticle( 5 ) );
	getParticle( 5 )->addSpring( getParticle( 6 ) );
	getParticle( 6 )->addSpring( getParticle( 7 ) );
	getParticle( 7 )->addSpring( getParticle( 4 ) );

	getParticle( 0 )->addSpring( getParticle( 4 ) );
	getParticle( 1 )->addSpring( getParticle( 5 ) );
	getParticle( 2 )->addSpring( getParticle( 6 ) );
	getParticle( 3 )->addSpring( getParticle( 7 ) );
*/
	MPBHardSpringAngular* s;

	s = new MPBHardSpringAngular(	getParticle( 0 ),
											getParticle( 2 ),
											getParticle( 1 ),
											PI / 2	);
	getParticle( 0 )->addSpringAngular( s );

	s = new MPBHardSpringAngular(	getParticle( 0 ),
											getParticle( 2 ),
											getParticle( 3 ),
											PI / 2	);
	getParticle( 0 )->addSpringAngular( s );


/*	s = new MPBHardSpringAngular(	getParticle( 3 ),
											getParticle( 4 ),
											getParticle( 0 ),
											PI / 2	);
	getParticle( 3 )->addSpringAngular( s );
*/

	{MPB_LIST_LOOP( m_particles, p, MPBPhysicsParticle* )
	{
		MPBPhysics::addParticle( p );
	}}
}

void MPBRigidBodyBox::draw() const
{
/*	int end[] =   { 1, 2, 3, 0, 5, 6, 7, 4, 4, 5, 6, 7 };
	int start[] = { 0, 1, 2, 3, 4, 5, 6, 7, 0, 1, 2, 3 };
	int limbs = sizeof( start ) / sizeof( int );

	for (int i=0; i<limbs; i++)
	{
		glPushMatrix();

		MPBVector trans = getParticle( start[i]]->getPosition();
		glTranslatef( trans.x, trans.y, trans.z );

		MPBVector v = getParticle( start[i]]->getPosition() - getParticle( end[i]]->getPosition();
		MPBVector vr = v;
		vr.doOpenGLRotation();

		gluCylinder( MPBOpenGL::quadricObj, getWidth() / 16, getWidth() / 16, v.getLength(), 8, 8 );

		glPopMatrix();
	}
*/
}

void MPBRigidBodyBox::updateFrame()
{
}

