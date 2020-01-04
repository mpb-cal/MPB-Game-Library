
#include "MPBSpringTriangle.h"
#include "MPBSpring.h"
#include "MPBGameEntities.h"
#include "MPBPhysicsObject.h"
#include "mmgr.h"



MPBSpringTriangle::MPBSpringTriangle()
{
	MPBPhysicsObject* o;

	int objects = 6;
	MPBFLOAT x = 4;
	MPBFLOAT y = 1;
	for (int i=0; i<objects; i++)
	{
		o = new MPBPhysicsObject( POT_AABB );
		o->setRadius( .1f );
		o->setPosition( MPBVector( x, y - 2, -3 ) );
		o->setGravity( 0 );
		o->setCollidable( false );
		o->setCollisionTest( false );
		MPBPhysics::addObject( o );
		m_objects.push_back( o );

		if (i % 2 == 1) x++;
		y = !y;
	}

	((MPBPhysicsObject*)m_objects[0])->setImmovable( true );
	((MPBPhysicsObject*)m_objects[1])->setImmovable( true );
	((MPBPhysicsObject*)m_objects[i-2])->setImmovable( true );
	((MPBPhysicsObject*)m_objects[i-1])->setImmovable( true );

	MPBSpring* s;
	MPBFLOAT springConstant = 10000;
	MPBFLOAT dampingConstant = springConstant / 100;

	s = new MPBSpring( (MPBPhysicsObject*)m_objects[0], (MPBPhysicsObject*)m_objects[1], true, 1, springConstant, dampingConstant );
	MPBGameEntities::addSpring( s );

	for (i=1; i<objects-1; i+=2)
	{
		s = new MPBSpring( (MPBPhysicsObject*)m_objects[i], (MPBPhysicsObject*)m_objects[i+2], true, 1, springConstant, dampingConstant );
		MPBGameEntities::addSpring( s );

		s = new MPBSpring( (MPBPhysicsObject*)m_objects[i+2], (MPBPhysicsObject*)m_objects[i+1], true, 1, springConstant, dampingConstant );
		MPBGameEntities::addSpring( s );

		s = new MPBSpring( (MPBPhysicsObject*)m_objects[i+1], (MPBPhysicsObject*)m_objects[i-1], true, 1, springConstant, dampingConstant );
		MPBGameEntities::addSpring( s );
	}
}

MPBSpringTriangle::~MPBSpringTriangle()
{
}


void MPBSpringTriangle::draw()
{
	MPB_LIST_LOOP( m_objects, o, MPBPhysicsObject* )
	{
		o->getPosition().drawAsVertex();
	}
}





