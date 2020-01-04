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

/*
bool collisionPoint2DLine2D( 
	const MPBVector2D& point, 
	const MPBVector2D& destination, 
	const MPBLine2D& line, 
	MPBVector2D& intersection,
	bool checkEndpoints
);
*/
bool collisionSphereSphere(	const MPBVector& startPosition,
										const MPBVector& destPosition,
										MPBFLOAT radius1,
										const MPBVector& position2,
										MPBFLOAT radius2,
										const MPBVector& velocityVector,
										MPBFLOAT travelDistance,
										Collision*& collision	);

bool collisionRaySphere(	const MPBVector& source, 
									const MPBVector& ray, 
									const MPBVector& sphereCenter, 
									MPBFLOAT sphereRadius,
									MPBVector& intersectionPointNear,
									MPBVector& intersectionPointFar,
									MPBFLOAT& distanceToIntersectionNear	,
									MPBFLOAT& distanceToIntersectionFar	);

bool collisionAABBAABB(	const MPBVector& destPosition,
								const MPBAABB& aabb1,
								const MPBAABB& aabb2,
								const MPBVector& velocityVector,
								MPBFLOAT travelDistance,
								Collision*& collision	);

bool collisionAABBShape(	const MPBVector& destPosition,
									const MPBAABB& aabb1,
									MPBShape* shape,
									Collision*& collision	);

bool collisionPointPlane(	MPBVector point, 
									Plane* plane, 
									MPBVector velocityVector, 
									MPBFLOAT travelDistance, 
									MPBFLOAT& distanceToPlane,
									MPBVector& planeIntersectionPoint	);

bool collisionAABBPlane(	MPBFLOAT travelDistance,
									const MPBVector& velocityVector,
									const MPBVector& velocityVectorNormalized,
									const MPBVector& destPosition,
									const MPBAABB& aabb1,
									Plane* plane,
									Collision*& collision	);

bool collisionPointAABB(	const MPBVector& point,
									const MPBAABB& aabb	);

bool collisionSpherePlane(	Plane* plane, 
									const MPBVector& sourcePosition, 
									const MPBVector& destPosition, 
									const MPBVector& velocityVector,	// must be normalized!
									const MPBVector& travelVector,	// = non-normalized velocityVector
									MPBFLOAT travelDistance,
									MPBFLOAT objectRadius,
									Collision*& collision	);

MPBVector closestPointOnLine(	MPBVector a,
										MPBVector b,
										MPBVector p	);

void collisionBounce(	MPBFLOAT elasticity,							// between 0.0 and 1.0
								const MPBVector& collisionPoint,			// in world coords
								const MPBVector& velocity1,
								const MPBVector& velocity2,
								const MPBVector& localVelocity1,
								const MPBVector& localVelocity2,
								const MPBVector& angularVelocity1,
								const MPBVector& angularVelocity2,
								const MPBVector& position1,
								const MPBVector& position2,
								const MPBQuaternion& orientation1,
								const MPBQuaternion& orientation2,
//								const MPBVector& rotation1,
//								const MPBVector& rotation2,
								MPBFLOAT mass1,
								MPBFLOAT mass2,
								MPBFLOAT inertia1,
								MPBFLOAT inertia2,
								MPBVector& newVelocity1,					// resulting velocity of object 1
								MPBVector& newVelocity2,					// resulting velocity of object 2
								MPBVector& newAngularVelocity1,			// resulting angular velocity of object 1
								MPBVector& newAngularVelocity2,			// resulting angular velocity of object 2
								bool ignoreAngular	);

bool pathIsClearSphere(
	MPBVector from,
	MPBVector to,
	MPBFLOAT radius,
	bool testObjects,
	MPBPhysicsObject* source,	// can be null
	MPBPhysicsObject* target,	// can be null
	Collision*& collision
);

bool pathIsClearAABB(
	MPBVector from,
	MPBVector to,
	MPBFLOAT radius,
	bool testObjects,
	MPBPhysicsObject* source,	// can be null
	MPBPhysicsObject* target	// can be null
);

bool aabbInFrustum( const MPBAABB& aabb, Frustum& frustum );
bool sphereInFrustum( const MPBVector& center, MPBFLOAT radius, Frustum& frustum );
bool pointsInFrustum( MPBVector* pointList, int points, Frustum& frustum );

bool aabbInPolyhedron( const MPBAABB& aabb, Polyhedron& polyhedron );
bool sphereInPolyhedron( const MPBVector& center, MPBFLOAT radius, Polyhedron& polyhedron );
bool pointsInPolyhedron( MPBVector* pointList, int points, Polyhedron& polyhedron );

void findHalfspaceIntersection( MPBList& in, MPBList& out );

#endif

