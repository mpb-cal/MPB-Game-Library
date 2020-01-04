
#include "MPBPhysicsParticle.h"
#include "MPBPhysics.h"
#include "MPBGlobal.h"
#include "MPBFrameCounter.h"
#include "MPBCollision.h"
#include "MPBGameEntities.h"
#include "MPBMessageWindow.h"
#include "MPBPlane.h"
#include "MPBOpenGL.h"
#include "MPBSoundManager.h"
#include "MPBShadow.h"
#include "MPBParticleSystem.h"
#include "MPBShape.h"
#include "MPBHardSpring.h"
#include "MPBHardSpringAngular.h"
#include "MPBPlaneSpring.h"
#include "MPBHeightMap.h"
#include "MPBRigidBody.h"
#include "mmgr.h"


MPBPhysicsParticle::MPBPhysicsParticle( MPBVector position ):
	MPB3DObject(),
	m_collisionTest( true )
{
	m_position = position;
	m_prevPosition = position;
	m_rotationMatrix.loadIdentity();
	m_rotationMatrix2.loadIdentity();
}

MPBPhysicsParticle::~MPBPhysicsParticle()
{
	MPB_LIST_DELETE( m_springs, MPBHardSpring* );
	MPB_LIST_DELETE( m_springAngulars, MPBHardSpringAngular* );
	MPB_LIST_DELETE( m_planeSprings, MPBPlaneSpring* );
}

MPBVector MPBPhysicsParticle::getAxisVector( DirectionType direction ) const
{
	// take a unit vector pointing down the z-axis
	// and rotate it appropriately.
	MPBVector axisVector( 0, 0, -1 );

	switch (direction)
	{
		case DIR_FORWARDS:	break;
		case DIR_BACKWARDS:	axisVector.rotateY( 180 ); break;
		case DIR_LEFT:			axisVector.rotateY( 90 ); break;
		case DIR_RIGHT:		axisVector.rotateY( -90 ); break;
		case DIR_UP:			axisVector.rotateX( 90 ); break;
		case DIR_DOWN:			axisVector.rotateX( -90 ); break;
		MPB_ASSERT( 0 );
	}

	axisVector = axisVector * m_rotationMatrix;
	axisVector = axisVector * m_rotationMatrix2;

	return axisVector;
}

void MPBPhysicsParticle::draw()
{
	MPB_LIST_LOOP( m_springs, s, MPBHardSpring* )
	{
		s->draw();
	}

	{MPB_LIST_LOOP( m_planeSprings, s, MPBPlaneSpring* )
	{
		s->draw();
	}}

	glPushMatrix();

	glTranslated( m_position.x, m_position.y, m_position.z );

	MPBFLOAT array[16];
	m_rotationMatrix.toOpenGLArray( array );
	glMultMatrixd( array );

	m_rotationMatrix2.toOpenGLArray( array );
	glMultMatrixd( array );

	if (m_shapes.size())
	{
		MPB_LIST_LOOP( m_shapes, shape, MPBShape* )
		{
			shape->draw( true, false, true );
		}
	}
	else
	{
		glColor3ub( m_red, m_green, m_blue );
//		gluSphere( MPBOpenGL::quadricObj, m_mass * .001, 10, 10 );
		gluSphere( MPBOpenGL::quadricObj, .1, 10, 10 );
		glColor3ub( 255, 255, 255 );
	}

	glPopMatrix();
}

void MPBPhysicsParticle::addSpring( MPBHardSpring* s ) 
{
	m_springs.push_back( s ); 
}

void MPBPhysicsParticle::addSpring( MPBPhysicsParticle* o ) 
{
	m_springs.push_back( new MPBHardSpring( this, o ) ); 
}

void MPBPhysicsParticle::addSpring( MPBPhysicsParticle* o, MPBFLOAT restLength ) 
{
	m_springs.push_back( new MPBHardSpring( this, o, restLength ) ); 
}

void MPBPhysicsParticle::addSpring( MPBPhysicsParticle* o, MPBFLOAT restLengthMin, MPBFLOAT restLengthMax ) 
{
	m_springs.push_back( new MPBHardSpring( this, o, restLengthMin, restLengthMax ) ); 
}

void MPBPhysicsParticle::addSpringAngular( MPBHardSpringAngular* s ) 
{
	m_springAngulars.push_back( s ); 
}

void MPBPhysicsParticle::addPlaneSpring( MPBPlaneSpring* s )
{
	m_planeSprings.push_back( s ); 
}

void MPBPhysicsParticle::removeSprings()
{
	MPB_LIST_DELETE( m_springs, MPBHardSpring* );
	MPB_LIST_DELETE( m_springAngulars, MPBHardSpringAngular* );
	MPB_LIST_DELETE( m_planeSprings, MPBPlaneSpring* );
}

// call once on each particle,
// then call updateFrame2() 1 or more times
void MPBPhysicsParticle::updateFrame1()
{
	// update shapes

	MPB_LIST_LOOP( m_shapes, shape, MPBShape* )
	{
		shape->updateFrame();
	}

	// make collision sound

	if (m_collided && m_planeCollisionSound)
	{
/*		LONG volume = 16000 * m_penetration.getLength() - 1000;

		if (volume > -500)
			MPBSoundManager::playSound( 
				m_planeCollisionSound, 
				m_position, 
				MPBVector(),//m_velocity.normalized(), 
				volume
			);
*/	}

	// apply gravity

	MPBVector gravity( 0, -m_gravity, 0 );
	m_acceleration = m_acceleration + gravity;

	// apply air resistance

	const MPBFLOAT DRAG_COEFFICIENT = .43;
	MPBFLOAT airDensity = 1.3;
	MPBFLOAT frontalArea =	3;
	MPBFLOAT drag = -.5 * DRAG_COEFFICIENT * frontalArea * airDensity;

	MPBVector dragForce(	drag * m_velocity.x * fabs( m_velocity.x ),
								drag * m_velocity.y * fabs( m_velocity.y ),
								drag * m_velocity.z * fabs( m_velocity.z )	);

	applyForce( dragForce * m_mass );

	// apply collision friction

	if (m_collided)
	{
//		applyForce( m_velocity * m_mass * -100000 * m_penetration.getLength() );
	}

	// apply lateral friction

	if (/*m_lateralFriction && */m_collided)
	{
//		applyForce( getLateralVelocity() * -m_mass * 500 );
	}

	////////////////////////////
	// Verlet integration

	MPBVector newPosition = 
		m_position * 2 - 
		m_prevPosition + 
		m_acceleration * MPBFrameCounter::secondsElapsed * MPBFrameCounter::secondsElapsed;

	m_prevPosition = m_position;
	m_position = newPosition;

	if (MPBOpenGL::is2D())
	{
		m_position.z = -1;
		m_prevPosition.z = -1;
	}

	m_acceleration.set( 0, 0, 0 );
	m_rotationMatrix2.loadIdentity();
	m_collided = false;
}

// call 1 or more times after calling updateFrame1()
void MPBPhysicsParticle::updateFrame2()
{
	MPBVector oldPosition = m_position;

	// test for collisions

	if (m_collisionTest)
	{
		MPB_LIST_LOOP( MPBGameEntities::shapes, shape, MPBShape* )
		{
			if (!shape->isCollidable()) continue;
			if (shape->getPenetrationPoint( oldPosition, m_position ))
			{
				m_collided = true;
				break;
			}
		}

		if (MPBGameEntities::getHeightMap())
		{
			if (MPBGameEntities::getHeightMap()->getPenetrationPoint( m_position ))
			{
				m_collided = true;
			}
		}

		m_penetration = m_position - oldPosition;
	}

	// update springs

	MPB_LIST_LOOP( m_springs, s, MPBHardSpring* )
	{
		s->updateFrame();
	}

	{MPB_LIST_LOOP( m_springAngulars, s, MPBHardSpringAngular* )
	{
		s->updateFrame();
	}}

	{MPB_LIST_LOOP( m_planeSprings, s, MPBPlaneSpring* )
	{
		s->updateFrame();
	}}

	m_velocity = (m_position - m_prevPosition) / MPBFrameCounter::secondsElapsed;
}




