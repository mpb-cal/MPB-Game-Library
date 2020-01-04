
#include "MPBRigidBodyStickman.h"
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
					15

					14

			10-------13

	9							12
			6--------7
	8							11
			4--------5

			2			3

			0			1

*/

MPBRigidBodyStickman::MPBRigidBodyStickman() :
	MPBRigidBody()
{
}

MPBRigidBodyStickman::~MPBRigidBodyStickman()
{
}

void MPBRigidBodyStickman::setCorners( const MPBVector& NLL, const MPBVector& FUR )
{
	MPBAABB::setCorners( NLL, FUR );

	{MPB_LIST_LOOP( m_particles, p, MPBPhysicsParticle* )
	{
		MPBPhysics::removeParticle( p );
	}}

	MPB_LIST_DELETE( m_particles, MPBPhysicsParticle* );

	MPBFLOAT unit = getWidth()/3;

	m_particles.push_back( new MPBPhysicsParticle( MPBVector( 
		getCenter().x - unit, 
		getCenter().y,
		getCenter().z ) ) );
	m_particles.push_back( new MPBPhysicsParticle( MPBVector( 
		getCenter().x + unit, 
		getCenter().y,
		getCenter().z ) ) );
	m_particles.push_back( new MPBPhysicsParticle( MPBVector( 
		getCenter().x - unit, 
		getCenter().y + unit*2,
		getCenter().z ) ) );
	m_particles.push_back( new MPBPhysicsParticle( MPBVector( 
		getCenter().x + unit, 
		getCenter().y + unit*2,
		getCenter().z ) ) );
	m_particles.push_back( new MPBPhysicsParticle( MPBVector( 
		getCenter().x - unit, 
		getCenter().y + unit*4,
		getCenter().z ) ) );
	m_particles.push_back( new MPBPhysicsParticle( MPBVector( 
		getCenter().x + unit, 
		getCenter().y + unit*4,
		getCenter().z ) ) );
	m_particles.push_back( new MPBPhysicsParticle( MPBVector( 
		getCenter().x - unit, 
		getCenter().y + unit*5,
		getCenter().z ) ) );
	m_particles.push_back( new MPBPhysicsParticle( MPBVector( 
		getCenter().x + unit, 
		getCenter().y + unit*5,
		getCenter().z ) ) );

	m_particles.push_back( new MPBPhysicsParticle( MPBVector( 
		getCenter().x - unit*1.5, 
		getCenter().y + unit*4,
		getCenter().z ) ) );
	m_particles.push_back( new MPBPhysicsParticle( MPBVector( 
		getCenter().x - unit*1.5, 
		getCenter().y + unit*5.5,
		getCenter().z ) ) );
	m_particles.push_back( new MPBPhysicsParticle( MPBVector( 
		getCenter().x - unit*1.5, 
		getCenter().y + unit*7,
		getCenter().z ) ) );

	m_particles.push_back( new MPBPhysicsParticle( MPBVector( 
		getCenter().x + unit*1.5, 
		getCenter().y + unit*4,
		getCenter().z ) ) );
	m_particles.push_back( new MPBPhysicsParticle( MPBVector( 
		getCenter().x + unit*1.5, 
		getCenter().y + unit*5.5,
		getCenter().z ) ) );
	m_particles.push_back( new MPBPhysicsParticle( MPBVector( 
		getCenter().x + unit*1.5, 
		getCenter().y + unit*7,
		getCenter().z ) ) );

	m_particles.push_back( new MPBPhysicsParticle( MPBVector( 
		getCenter().x, 
		getCenter().y + unit*8,
		getCenter().z ) ) );
	m_particles.push_back( new MPBPhysicsParticle( MPBVector( 
		getCenter().x, 
		getCenter().y + unit*9,
		getCenter().z ) ) );

	// feet	
	m_particles.push_back( new MPBPhysicsParticle( MPBVector( 
		getCenter().x - unit, 
		getCenter().y - unit * .6,
		getCenter().z ) ) );
	m_particles.push_back( new MPBPhysicsParticle( MPBVector( 
		getCenter().x + unit, 
		getCenter().y - unit * .6,
		getCenter().z ) ) );

	// hands
	m_particles.push_back( new MPBPhysicsParticle( MPBVector( 
		getCenter().x - unit*1.5, 
		getCenter().y + unit*3.4,
		getCenter().z ) ) );
	m_particles.push_back( new MPBPhysicsParticle( MPBVector( 
		getCenter().x + unit*1.5, 
		getCenter().y + unit*3.4,
		getCenter().z ) ) );

	{MPB_LIST_LOOP( m_particles, p, MPBPhysicsParticle* )
	{
		MPBVector pos = (p)->getPosition();
		(p)->setPosition( MPBVector( pos.x, getCenter().y, -pos.y ) );
		(p)->setPlaneCollisionSound( "leftfoot.wav" );
		(p)->setMass( 200 );
//		(p)->setGravity( 0 );
	}}

//	getParticle( 4 )->setInfiniteMass( true );

	getParticle( 16 )->addSpring( getParticle( 0 ) );
	getParticle( 1 )->addSpring( getParticle( 17 ) );
	getParticle( 18 )->addSpring( getParticle( 8 ) );
	getParticle( 19 )->addSpring( getParticle( 11 ) );
	getParticle( 0 )->addSpring( getParticle( 2 ) );
	getParticle( 2 )->addSpring( getParticle( 4 ) );
	getParticle( 1 )->addSpring( getParticle( 3 ) );
	getParticle( 3 )->addSpring( getParticle( 5 ) );
	getParticle( 4 )->addSpring( getParticle( 5 ) );
	getParticle( 4 )->addSpring( getParticle( 6 ) );
	getParticle( 5 )->addSpring( getParticle( 7 ) );
	getParticle( 6 )->addSpring( getParticle( 7 ) );
	getParticle( 8 )->addSpring( getParticle( 9 ) );
	getParticle( 9 )->addSpring( getParticle( 10 ) );
	getParticle( 10 )->addSpring( getParticle( 14 ) );
	getParticle( 14 )->addSpring( getParticle( 13 ) );
	getParticle( 13 )->addSpring( getParticle( 12 ) );
	getParticle( 12 )->addSpring( getParticle( 11 ) );
	getParticle( 6 )->addSpring( getParticle( 10 ) );
	getParticle( 7 )->addSpring( getParticle( 13 ) );
	getParticle( 14 )->addSpring( getParticle( 15 ) );
	getParticle( 4 )->addSpring( getParticle( 7 ) );
	getParticle( 5 )->addSpring( getParticle( 6 ) );
	getParticle( 6 )->addSpring( getParticle( 13 ) );
	getParticle( 13 )->addSpring( getParticle( 10 ) );
	getParticle( 10 )->addSpring( getParticle( 7 ) );
	getParticle( 7 )->addSpring( getParticle( 14 ) );
	getParticle( 14 )->addSpring( getParticle( 6 ) );

	// keep ankles away from pelvis
	getParticle( 0 )->addSpring( getParticle( 4 ), unit * 1, unit * 100 );
	getParticle( 1 )->addSpring( getParticle( 5 ), unit * 1, unit * 100 );

	// keep pelvis away from neck
	getParticle( 4 )->addSpring( getParticle( 14 ), unit * 4, unit * 100 );
	getParticle( 5 )->addSpring( getParticle( 14 ), unit * 4, unit * 100 );

	// keep knees from bending backwards
/*	getParticle( 0 )->addPlaneSpring( new MPBPlaneSpring(
		getParticle( 0 ),
		getParticle( 5 ),
		getParticle( 4 ),
		getParticle( 2 ),
		false	) );

	getParticle( 1 )->addPlaneSpring( new MPBPlaneSpring(
		getParticle( 1 ),
		getParticle( 3 ),
		getParticle( 5 ),
		getParticle( 4 ),
		false	) );

	// keep elbows from bending backwards
	getParticle( 8 )->addPlaneSpring( new MPBPlaneSpring(
		getParticle( 8 ),
		getParticle( 13 ),
		getParticle( 10 ),
		getParticle( 9 ),
		false	) );

	getParticle( 11 )->addPlaneSpring( new MPBPlaneSpring(
		getParticle( 11 ),
		getParticle( 12 ),
		getParticle( 13 ),
		getParticle( 10 ),
		false	) );
*/
	// restrict thighs to forward and back

/*	getParticle( 2 )->addPlaneSpring( new MPBPlaneSpring(
		getParticle( 2 ),
		getParticle( 4 ),
		getParticle( 5 ),
		getParticle( 4 )	) );

	getParticle( 3 )->addPlaneSpring( new MPBPlaneSpring(
		getParticle( 3 ),
		getParticle( 5 ),
		getParticle( 4 ),
		getParticle( 5 )	) );

	// restrict calves to forward and back

	getParticle( 0 )->addPlaneSpring( new MPBPlaneSpring(
		getParticle( 0 ),
		getParticle( 4 ),
		getParticle( 5 ),
		getParticle( 4 )	) );

	getParticle( 1 )->addPlaneSpring( new MPBPlaneSpring(
		getParticle( 1 ),
		getParticle( 5 ),
		getParticle( 4 ),
		getParticle( 5 )	) );


	// restrict upper arms to forward and back

	getParticle( 9 )->addPlaneSpring( new MPBPlaneSpring(
		getParticle( 9 ),
		getParticle( 10 ),
		getParticle( 13 ),
		getParticle( 10 )	) );

	getParticle( 12 )->addPlaneSpring( new MPBPlaneSpring(
		getParticle( 12 ),
		getParticle( 13 ),
		getParticle( 10 ),
		getParticle( 13 )	) );

	// restrict lower arms to forward and back

	getParticle( 8 )->addPlaneSpring( new MPBPlaneSpring(
		getParticle( 8 ),
		getParticle( 10 ),
		getParticle( 13 ),
		getParticle( 10 )	) );

	getParticle( 11 )->addPlaneSpring( new MPBPlaneSpring(
		getParticle( 11 ),
		getParticle( 13 ),
		getParticle( 10 ),
		getParticle( 13 )	) );


	// restrict head to forward and back

	getParticle( 15 )->addPlaneSpring( new MPBPlaneSpring(
		getParticle( 15 ),
		getParticle( 10 ),
		getParticle( 13 ),
		getParticle( 14 )	) );
*/

/*	MPBHardSpringAngular* s = NULL;

	// max thigh angle is 80 degrees

	s = new MPBHardSpringAngular(	getParticle( 2 ),
											getParticle( 6 ),
											getParticle( 4 ),
											PI / 2 * 1.1,
											PI	);
	getParticle( 2 )->addSpringAngular( s );

	s = new MPBHardSpringAngular(	getParticle( 3 ),
											getParticle( 7 ),
											getParticle( 5 ),
											PI / 2 * 1.1,
											PI	);
	getParticle( 3 )->addSpringAngular( s );
*/


	{MPB_LIST_LOOP( m_particles, p, MPBPhysicsParticle* )
	{
		MPBPhysics::addParticle( p );
	}}
}

void MPBRigidBodyStickman::draw()
{
	MPBOpenGL::bindTexture( "font2" );

	int end[] =   { 2, 3, 4, 5, 9, 10,12, 13, 15, 13, 7, 5, 10,13,6, 7, 0,  1,  8,  11 };
	int start[] = { 0, 1, 2, 3, 8, 9, 11, 12, 14, 10, 6, 4, 6, 7, 4, 5, 16, 17, 18, 19 };
	int limbs = sizeof( start ) / sizeof( int );

	for (int i=0; i<limbs; i++)
	{
		glPushMatrix();

		MPBVector trans = getParticle( start[i] )->getPosition();
		glTranslatef( trans.x, trans.y, trans.z );

		MPBVector v = getParticle( start[i] )->getPosition() - getParticle( end[i] )->getPosition();
		MPBVector vr = v;
		vr.doOpenGLRotation();

		gluCylinder( MPBOpenGL::quadricObj, getWidth() / 16, getWidth() / 16, v.getLength(), 8, 8 );

		glPopMatrix();
	}

/*	glPushMatrix();

	MPBVector avg = (
		getParticle( 6 )->getPosition() +
		getParticle( 7 )->getPosition() +
		getParticle( 10 )->getPosition() +
		getParticle( 13 )->getPosition() +
		getParticle( 14 )->getPosition()
		) / 5;

	glTranslatef( avg.x, avg.y, avg.z );

	MPBVector v = getParticle( 14 )->getPosition() - avg;
	MPBVector v2 = getParticle( 13 )->getPosition() - avg;
	v.doOpenGLRotation( v2 );

//	glScaled( 1, .3, 1 );

	gluSphere( MPBOpenGL::quadricObj, getWidth() / 3, 8, 8 );

	glPopMatrix();*/
}

void MPBRigidBodyStickman::updateFrame()
{
//	MPBRigidBody::updateFrame();
}

