
#include "MPBRigidBody.h"
#include "MPBGameEntities.h"
#include "MPBHardSpring.h"
#include "MPBPhysics.h"
#include "MPBPhysicsParticle.h"
#include "MPBShape.h"
#include "MPBMatrix.h"
#include "MPBPlane.h"
#include "mmgr.h"


/*
	0 - top
	1 - front
	2 - left
	3 - right
*/


MPBRigidBody::MPBRigidBody() :
	MPBAABB()
{
	m_rotationMatrix.loadIdentity();
	m_translationMatrix.loadIdentity();
}

MPBRigidBody::~MPBRigidBody()
{
	MPB_LIST_DELETE( m_shapes, MPBShape* );
}

void MPBRigidBody::setCorners( const MPBVector& NLL, const MPBVector& FUR )
{
	MPBAABB::setCorners( NLL, FUR );

	MPB_LIST_LOOP( m_particles, p, MPBPhysicsParticle* )
	{
		MPBPhysics::removeParticle( p );
	}

	MPB_LIST_DELETE( m_particles, MPBPhysicsParticle* );

	m_particles.push_back( new MPBPhysicsParticle(
		MPBVector( getCenter().x, getCenter().y + getHeight() / 2, getCenter().z ) ) );
	m_particles.push_back( new MPBPhysicsParticle(
		MPBVector( getCenter().x, getCenter().y, getCenter().z - getDepth() / 2 ) ) );
	m_particles.push_back( new MPBPhysicsParticle(
		MPBVector( getCenter().x - getWidth() / 2, getCenter().y, getCenter().z + getDepth() / 2 ) ) );
	m_particles.push_back( new MPBPhysicsParticle(
		MPBVector( getCenter().x + getWidth() / 2, getCenter().y, getCenter().z + getDepth() / 2 ) ) );

	{MPB_LIST_LOOP( m_particles, p, MPBPhysicsParticle* )
	{
		p->setPlaneCollisionSound( "leftfoot.wav" );
		p->setMass( 500 );
	}}

	getParticle( 0 )->addSpring( getParticle( 1 ), getWidth() );
	getParticle( 0 )->addSpring( getParticle( 2 ), getWidth() );
	getParticle( 0 )->addSpring( getParticle( 3 ), getWidth() );

	getParticle( 1 )->addSpring( getParticle( 2 ), getWidth() );
	getParticle( 1 )->addSpring( getParticle( 3 ), getWidth() );
	getParticle( 2 )->addSpring( getParticle( 3 ), getWidth() );

	{MPB_LIST_LOOP( m_particles, p, MPBPhysicsParticle* )
	{
		MPBPhysics::addParticle( p );
	}}
}

void MPBRigidBody::addShape( MPBShape* s ) 
{ 
	s->setFrustumCull( false );
	s->getPlane( 2 )->setColor( .8f, .4f, .4f, 1 );
	s->getPlane( 0 )->setColor( .4f, .4f, .8f, 1 );
	m_shapes.push_back( s ); 
}

void MPBRigidBody::draw()
{
	MPBFLOAT array[16];

	m_translationMatrix.toOpenGLArray( array );
	glMultMatrixd( array );

	m_rotationMatrix.toOpenGLArray( array );
	glMultMatrixd( array );

	MPB_LIST_LOOP( m_shapes, s, MPBShape* )
	{
		s->draw( true, false, true );
	}
}

void MPBRigidBody::updateFrame()
{
	MPBVector avgLR(
		(getParticle( 3 )->getPosition() +
		getParticle( 2 )->getPosition())
			/ 2.0 );

	MPBVector right;
	MPBVector up;
	MPBVector forward;

	right = getParticle( 3 )->getPosition() - getParticle( 2 )->getPosition();
	forward = getParticle( 1 )->getPosition() - avgLR;

//	right.set( 1, 0, 0 );
//	up.set( 0, 1, 0 );
//	forward.set( 0, 0, -1 );

	// orthonormalize
	up = right.crossProduct( forward );
	right = forward.crossProduct( up );

	m_rotationMatrix.createRotation( right, up, forward );

	getParticle( 0 )->setRotationMatrix( m_rotationMatrix );
	getParticle( 1 )->setRotationMatrix( m_rotationMatrix );
	getParticle( 2 )->setRotationMatrix( m_rotationMatrix );
	getParticle( 3 )->setRotationMatrix( m_rotationMatrix );

	// find CG
	MPBVector avgAll(
		(getParticle( 0 )->getPosition() +
		getParticle( 1 )->getPosition() +
		getParticle( 2 )->getPosition() +
		getParticle( 3 )->getPosition())
			/ 4.0 );

	m_translationMatrix.loadIdentity();
	m_translationMatrix.translate( avgAll );
}

