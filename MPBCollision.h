#ifndef __MPBCOLLISION_H__
#define __MPBCOLLISION_H__

#include "MPBVector.h"
#include "MPBPlane.h"
#include "MPBList.h"

class MPBAABB;
class MPBPhysicsObject;
class MPBShape;
class MPBPhysicsObject;
class MPBLine2D;


class Collision
{
	public:

	Collision(	MPBFLOAT distance, 
					const MPBVector& newDestination,
					const MPBVector& slideVector	);

	MPBFLOAT 			m_distance;
	MPBVector			m_newDestination;
	MPBVector			m_slideVector;
	MPBPhysicsObject*	m_targetObject;
	Plane*				m_targetPlane;
	MPBShape*			m_targetShape;
	MPBVector			m_normal;
};

struct Frustum
{
	Plane* Left;
	Plane* Right;
	Plane* Top;
	Plane* Bottom;
	Plane* Near;
	Plane* Far;
};

struct Polyhedron
{
	MPBList	planes;
};

void MPBCollisionTest();

bool collisionLessThan( const Collision* lhs, const Collision* rhs );
bool collisionEquals( const Collision* lhs, const Collision* rhs );



#endif

