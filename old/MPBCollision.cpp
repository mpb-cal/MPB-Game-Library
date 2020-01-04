
#include "MPBLibrary.h"
#include "MPBCollision.h"
#include "MPBGlobal.h"
#include "MPBMessageWindow.h"
#include "MPBAABB.h"
#include "MPBPlane.h"
#include "MPBQuaternion.h"
#include "MPBPerformanceMonitor.h"
#include "MPBShape.h"
#include "MPBPhysics.h"
#include "MPBGameEntities.h"
#include "MPBPhysicsObject.h"
#include "mmgr.h"


const bool TRACE_SPHERE_PLANE = false;


//////////////////////////////////////////////////////////////

Collision::Collision(	MPBFLOAT distance, 
								const MPBVector& newDestination,
								const MPBVector& slideVector	):
	m_distance( distance ),
	m_newDestination( newDestination ),
	m_slideVector( slideVector ),
	m_targetObject( NULL ),
	m_targetPlane( NULL )
{
}

bool collisionLessThan( const Collision* lhs, const Collision* rhs )
{
	if (floatEQ( lhs->m_distance, rhs->m_distance ))
	{
		return floatLT( 
			rhs->m_slideVector.getLength(), lhs->m_slideVector.getLength() );
	}

	return floatLT( lhs->m_distance, rhs->m_distance );
}

bool collisionEquals( const Collision* lhs, const Collision* rhs )
{
	if (floatEQ( lhs->m_distance, rhs->m_distance ))
	{
		return true;
	}

	return false;
}

/*
bool collisionPoint2DLine2D( 
	const MPBVector2D& point, 
	const MPBVector2D& destination, 
	const MPBLine2D& line, 
	MPBVector2D& intersection,
	bool checkEndpoints
)
{
	MPBFLOAT Ax = point.x;
	MPBFLOAT Ay = point.y;
	MPBFLOAT Bx = destination.x;
	MPBFLOAT By = destination.y;
	MPBFLOAT Cx = line.m_v1.x;
	MPBFLOAT Cy = line.m_v1.y;
	MPBFLOAT Dx = line.m_v2.x;
	MPBFLOAT Dy = line.m_v2.y;

	MPBFLOAT denom = (Bx-Ax) * (Dy-Cy) - (By-Ay) * (Dx-Cx);
	MPBFLOAT numer_r = (Ay-Cy) * (Dx-Cx) - (Ax-Cx) * (Dy-Cy);
	MPBFLOAT numer_s = (Bx-Ax) * (Dy-Cy) - (By-Ay) * (Dx-Cx);

	if (denom == 0) return false;		// lines are parallel: no collision

	MPBFLOAT r = numer_r / denom;
	MPBFLOAT s = numer_s / denom;

	intersection.x = Ax + r * (Bx - Ax);
	intersection.y = Ay + r * (By - Ay);

	// moving towards outside of line? no collision
	MPBVector2D moveVector = destination - point;
	moveVector.normalize();
	if (line.m_normal.dotProduct( moveVector ) > 0)
	{
		return false;
	}

	// inside of line and moving further toward inside of line? collision
//	if (line.pointInside( point )) return true;

	if (checkEndpoints)
	{
		// line segments do not meet? no collision
		if (r < 0.0 || r > 1.0 || s < 0.0 || s > 1.0)
			return false;

		MPBVector2D intV1 = line.m_v1 - intersection;
		intV1.normalize();

		MPBVector2D intV2 = line.m_v2 - intersection;
		intV2.normalize();

		if (floatEQ( intV1.dotProduct( intV2 ), 1 ))
			return false;
	}

	return true;
}
*/

//////////////////////////////////////////////////////////////
// tests a moving sphere against a static sphere

// before calling:
//		precalculate velocityVector (= destPosition - startPosition)
//		normalize velocityVector (= destPosition - startPosition)
//		precalculate travelDistance (= velocityVector.getLength())
bool collisionSphereSphere(	const MPBVector& startPosition,
										const MPBVector& destPosition,
										MPBFLOAT radius1,
										const MPBVector& position2,		// position of 2nd sphere
										MPBFLOAT radius2,						// radius of 2nd sphere
										const MPBVector& velocityVector,	// must be normalized
										MPBFLOAT travelDistance,
										Collision*& collision	)
{
	MPBPerformanceMonitor::m_sphereSphereCollisionTests++;

	MPBFLOAT allowedDistance = radius1 + radius2;

	MPBVector centerToCenter = position2 - startPosition;
	if (travelDistance < centerToCenter.getLength() - allowedDistance) return false;

	MPBVector intersectionPointNear;
	MPBVector intersectionPointFar;
	MPBFLOAT distanceToIntersectionNear;
	MPBFLOAT distanceToIntersectionFar;

	if (collisionRaySphere(	startPosition, 
									velocityVector, 
									position2, 
									allowedDistance,
									intersectionPointNear,
									intersectionPointFar,
									distanceToIntersectionNear,
									distanceToIntersectionFar	))
	{
		if (floatGTE( distanceToIntersectionNear, 0 ) && distanceToIntersectionNear < travelDistance)
		{
			collision = new Collision( distanceToIntersectionNear, intersectionPointNear, MPBVector() );

			return true;
		}
	}

	return false;
}

// ray should be normalized
bool collisionRaySphere(	const MPBVector& source, 
									const MPBVector& ray, 
									const MPBVector& sphereCenter, 
									MPBFLOAT sphereRadius,
									MPBVector& intersectionPointNear,
									MPBVector& intersectionPointFar,
									MPBFLOAT& distanceToIntersectionNear,
									MPBFLOAT& distanceToIntersectionFar	)
{
//	MPB_ASSERT( floatEQ( ray.getLength(), 1 ) );

	MPBVector sphereToStart = source - sphereCenter;
	MPBVector rayN = ray;
	rayN.normalize();

	MPBFLOAT a = 1;
//	MPBFLOAT a = ray.x * ray.x + ray.y * ray.y + ray.z* ray.z;
	MPBFLOAT b = 2 * rayN.dotProduct( sphereToStart );
	MPBFLOAT c = 
		pow( source.x - sphereCenter.x, 2 ) + 
		pow( source.y - sphereCenter.y, 2 ) + 
		pow( source.z - sphereCenter.z, 2 ) -
		pow( sphereRadius, 2 );

	MPBFLOAT discriminant = b * b - 4 * a * c;

	if (discriminant < 0.0) return false;

	MPBFLOAT t0 = (-b + sqrt( discriminant )) / (2 * a);
	MPBFLOAT t1 = (-b - sqrt( discriminant )) / (2 * a);

	distanceToIntersectionNear = min( t0, t1 );
	distanceToIntersectionFar = max( t0, t1 );

	intersectionPointNear.set(	source.x + rayN.x * distanceToIntersectionNear,
										source.y + rayN.y * distanceToIntersectionNear,
										source.z + rayN.z * distanceToIntersectionNear	);

	intersectionPointFar.set(	source.x + rayN.x * distanceToIntersectionFar,
										source.y + rayN.y * distanceToIntersectionFar,
										source.z + rayN.z * distanceToIntersectionFar	);

	return true;
}

void collisionRaySphereTest()
{
	bool rc;

	MPBVector source;
	MPBVector ray;
	MPBVector sphereCenter;
	MPBFLOAT sphereRadius;
	MPBVector intersectionPointNear;
	MPBVector intersectionPointFar;
	MPBFLOAT distanceToIntersectionNear;
	MPBFLOAT distanceToIntersectionFar;

	source.set( 0, 400, 0 );
	ray.set( -1, -.000244, 0 );
	ray.set( 0, -.000244, 0 );
	sphereCenter.set( 0, 389.999991, 0 );
	sphereRadius = 10;

	rc = collisionRaySphere(	source, 
										ray, 
										sphereCenter, 
										sphereRadius, 
										intersectionPointNear,
										intersectionPointFar,
										distanceToIntersectionNear,
										distanceToIntersectionFar	);

	MPB_ASSERT( rc == true );

	source.set( 0, 1, 0 );
	ray.set( 0, -1, 0 );
	sphereCenter.set( 0, .97051, 0 );
	sphereRadius = .025;

	rc = collisionRaySphere(	source, 
										ray, 
										sphereCenter, 
										sphereRadius, 
										intersectionPointNear,
										intersectionPointFar,
										distanceToIntersectionNear,
										distanceToIntersectionFar	);

	MPB_ASSERT( rc == true );
	MPB_ASSERT( distanceToIntersectionNear < .0045 );
}

//////////////////////////////////////////////////////////////
// tests a moving AABB against a static AABB

// before calling:
//		precalculate velocityVector (= destPosition - startPosition)
//		normalize velocityVector (= destPosition - startPosition)
//		precalculate travelDistance (= velocityVector.getLength())
bool collisionAABBAABB(	const MPBVector& destPosition,
								const MPBAABB& moveBox,
								const MPBAABB& aabb2,				// static AABB
								const MPBVector& velocityVector,	// must be normalized
								MPBFLOAT travelDistance,
								Collision*& collision	)
{
	MPBPerformanceMonitor::m_aabbAABBCollisionTests++;

	if (moveBox.intersects( aabb2 )) 
	{
		collision = new Collision( 0, MPBVector(), MPBVector() );
		return true;
	}

	return false;
}

//////////////////////////////////////////////////////////////
// tests a moving AABB against a static Shape

// mpb! fix this
bool collisionAABBShape(	const MPBVector& destPosition,
									const MPBAABB& aabb1,				// moving AABB in start position
									MPBShape* shape,
									Collision*& collision	)
{
	MPBAABB movedAABB = aabb1;
	movedAABB.setCenter( destPosition );
	if (movedAABB.intersects( shape->getAABB() )) 
	{
		collision = new Collision( 0, MPBVector(), MPBVector() );
		return true;
	}

	return false;
}

bool collisionPointPlane(
	MPBVector point, 
	Plane* plane, 
	MPBVector velocityVector, 
	MPBFLOAT travelDistance, 
	MPBFLOAT& distanceToPlane,
	MPBVector& planeIntersectionPoint
)
{
	if (plane->rayIntersect( point, point + velocityVector, planeIntersectionPoint ))
	{
		distanceToPlane = (planeIntersectionPoint - point).getLength();
		if (floatLTE( distanceToPlane, travelDistance )) return true;
	}

	return false;
}

//////////////////////////////////////////////////////////////
// tests a moving AABB against a static Plane

// mpb! only tests corners
bool collisionAABBPlane(	MPBFLOAT travelDistance,
									const MPBVector& velocityVector,
									const MPBVector& velocityVectorNormalized,
									const MPBVector& destPosition,
									const MPBAABB& aabb1,				// moving AABB in start position
									Plane* plane,
									Collision*& collision	)
{
	MPBPerformanceMonitor::m_spherePlaneCollisionTests++;

	// moving away from plane?
	if (plane->getNormal().dotProduct( velocityVectorNormalized ) > 0)
	{
		return false;
	}

	MPBVector collisionVertex;
	MPBVector planeIntersectionPoint;
	MPBVector planeIntersectionPointTmp;
	MPBFLOAT distanceToPlaneTmp;
	MPBFLOAT distanceToPlane = 1000000;
	bool collided = false;

#define TEST_POINT( x )	\
	if (plane->lineInsideCorners( x, x + velocityVector, planeIntersectionPointTmp ))	\
	{	\
		distanceToPlaneTmp = (planeIntersectionPointTmp - x).getLength();	\
		if (floatLTE( distanceToPlaneTmp, travelDistance ))	\
		{	\
			if (distanceToPlaneTmp < distanceToPlane)	\
			{	\
				collided = true;	\
				distanceToPlane = distanceToPlaneTmp;	\
				planeIntersectionPoint = planeIntersectionPointTmp;	\
				collisionVertex = x;	\
			}	\
		}	\
	}

	TEST_POINT( aabb1.getNLL() );
	TEST_POINT( aabb1.getNLR() );
	TEST_POINT( aabb1.getNUR() );
	TEST_POINT( aabb1.getNUL() );
	TEST_POINT( aabb1.getFLL() );
	TEST_POINT( aabb1.getFLR() );
	TEST_POINT( aabb1.getFUR() );
	TEST_POINT( aabb1.getFUL() );

	if (collided)
	{
		/////////////////////////////////////////////////////
		// find slide vector

		MPBVector slideDestination;
		MPBVector vertexDestination = collisionVertex + velocityVector;
		plane->lineIntersect(	vertexDestination, 
										vertexDestination + plane->getNormal(), 
										slideDestination	);

		MPBVector slideVector;
		slideVector = slideDestination - planeIntersectionPoint;

		collision = new Collision( 
			distanceToPlane, 
			aabb1.getCenter() + velocityVectorNormalized * distanceToPlane, 
			slideVector
		);
		collision->m_targetPlane = plane;
		return true;
	}

	return false;
}


// tests for collision between a sphere and a plane
// returns true if collided
// collision will contain more info:
//		m_slideVector: 
bool collisionSpherePlane(	Plane* plane, 
									const MPBVector& sourcePosition, 
									const MPBVector& destPosition, 
									const MPBVector& velocityVector,	// must be normalized!
									const MPBVector& travelVector,	// = non-normalized velocityVector
									MPBFLOAT travelDistance,
									MPBFLOAT objectRadius,
									Collision*& collision	)
{
	MPB_ASSERT( objectRadius );

	MPBPerformanceMonitor::m_spherePlaneCollisionTests++;

	char buf[1000];

	if (TRACE_SPHERE_PLANE) 
	{
		writeLog( "\n" );

		sprintf( g_logBuf, "plane         : %X\n", plane );
		writeLog( g_logBuf );

		plane->getNormal().toString( buf );
		sprintf( g_logBuf, "plane normal  : %s\n", buf );
		writeLog( g_logBuf );

		sourcePosition.toString( buf );
		sprintf( g_logBuf, "source pos    : %s\n", buf );
		writeLog( g_logBuf );

		destPosition.toString( buf );
		sprintf( g_logBuf, "dest pos      : %s\n", buf );
		writeLog( g_logBuf );

		velocityVector.toString( buf );
		sprintf( g_logBuf, "velocityVector: %s\n", buf );
		writeLog( g_logBuf );

		travelVector.toString( buf );
		sprintf( g_logBuf, "travelVector  : %s\n", buf );
		writeLog( g_logBuf );
	}

	MPB_ASSERT( floatEQ( velocityVector.getLength(), 1 ) );
	MPB_ASSERT( plane->getNormal().getLength() );

	/////////////////////////////////////////////////////
	// return if we are not moving

	if (travelDistance == 0) return false;

	/////////////////////////////////////////////////////
	// return if we are already inside plane

//	if (plane->isPointInside( sourcePosition ))
//	{
//		return false;
//	}

	/////////////////////////////////////////////////////
	// return if we are moving parallel to plane or
	// in same direction as plane's normal (ie, away from plane)

	if (velocityVector.dotProduct( plane->getNormal() ) >= 0)
	{
		if (TRACE_SPHERE_PLANE) writeLog( "NOT COLLIDED (moving parallel to plane or away from plane)\n" );
		return false;
	}

	/////////////////////////////////////////////////////
	// find orthogonalPlaneIntersectionPoint

	MPBVector orthogonalPlaneIntersectionPoint;
	bool orthogonalIntersects = 
		plane->lineIntersect(	sourcePosition, 
										sourcePosition + plane->getNormal() * -1, 
										orthogonalPlaneIntersectionPoint	);

	MPBFLOAT distanceToPlane = (orthogonalPlaneIntersectionPoint - sourcePosition).getLength();

	if (TRACE_SPHERE_PLANE) 
	{
		sprintf( g_logBuf, "distanceToPlane: %f\n", distanceToPlane );
		writeLog( g_logBuf );
	}

	/////////////////////////////////////////////////////

	MPBVector planeIntersectionPoint; 
	MPBVector polygonIntersectionPoint;

	bool embedded = false;

	/////////////////////////////////////////////////////
	// find plane intersection point

	if (distanceToPlane < objectRadius)
	{
		// we are embedded in this plane
		embedded = true;

		if (!orthogonalIntersects)
		{
			// we are more than halfway inside plane
			if (TRACE_SPHERE_PLANE) writeLog( "NOT COLLIDED (more than halfway inside plane)\n" );
			return false;
		}

//		planeIntersectionPoint = orthogonalPlaneIntersectionPoint;

		// if embedded, use orthogonal plane intersection point from 
		// the dest position??
		plane->lineIntersect(	destPosition, 
										destPosition + plane->getNormal() * -1, 
										planeIntersectionPoint	);
	}
	else
	{
		// we are not embedded in this plane
		embedded = false;

		MPBVector sphereIntersectionPoint = sourcePosition - (plane->getNormal() * objectRadius);

		if (TRACE_SPHERE_PLANE) 
		{
			sphereIntersectionPoint.toString( buf );
			sprintf( g_logBuf, "sphereIntersectionPoint: %s\n", buf );
			writeLog( g_logBuf );
		}

		if (plane->rayIntersect(
				sphereIntersectionPoint, 
				sphereIntersectionPoint + travelVector, 
				planeIntersectionPoint	) == false)
		{
			// we are not on a path to hit plane
			if (TRACE_SPHERE_PLANE) writeLog( "NOT COLLIDED (not on a path to hit plane)\n" );
			return false;
		}
	}

	if (TRACE_SPHERE_PLANE) 
	{
		sprintf( g_logBuf, "embedded: %d\n", embedded );
		writeLog( g_logBuf );

		planeIntersectionPoint.toString( buf );
		sprintf( g_logBuf, "planeIntersectionPoint: %s\n", buf );
		writeLog( g_logBuf );
	}

	/////////////////////////////////////////////////////
	// check if intersection is inside quad by adding up angles
	// from corners to intersection point: 360 = inside quad

	bool planeIntersectionPointIsInPoly = false;

	if (planeIntersectionPoint == plane->getLL()) planeIntersectionPointIsInPoly = true;
	else if (planeIntersectionPoint == plane->getLR()) planeIntersectionPointIsInPoly = true;
	else if (planeIntersectionPoint == plane->getUR()) planeIntersectionPointIsInPoly = true;
	else if (planeIntersectionPoint == plane->getUL()) planeIntersectionPointIsInPoly = true;
	else
	{
		MPBVector PIPToLL = plane->getLL() - planeIntersectionPoint;
		MPBVector PIPToLR = plane->getLR() - planeIntersectionPoint;
		MPBVector PIPToUR = plane->getUR() - planeIntersectionPoint;
		MPBVector PIPToUL = plane->getUL() - planeIntersectionPoint;

		PIPToLL.normalize();
		PIPToLR.normalize();
		PIPToUR.normalize();
		PIPToUL.normalize();

		MPBFLOAT dp_LL_LR = PIPToLL.dotProduct( PIPToLR );
		MPBFLOAT dp_LR_UR = PIPToLR.dotProduct( PIPToUR );
		MPBFLOAT dp_UR_UL = PIPToUR.dotProduct( PIPToUL );
		MPBFLOAT dp_UL_LL = PIPToUL.dotProduct( PIPToLL );

		// some dot products are slightly out of range due to fp precision errors
		if (dp_LL_LR > 1) dp_LL_LR = 1;
		if (dp_LR_UR > 1) dp_LR_UR = 1;
		if (dp_UR_UL > 1) dp_UR_UL = 1;
		if (dp_UL_LL > 1) dp_UL_LL = 1;

		if (dp_LL_LR < -1) dp_LL_LR = -1;
		if (dp_LR_UR < -1) dp_LR_UR = -1;
		if (dp_UR_UL < -1) dp_UR_UL = -1;
		if (dp_UL_LL < -1) dp_UL_LL = -1;

		MPBFLOAT angle_LL_LR = acos( dp_LL_LR );
		MPBFLOAT angle_LR_UR = acos( dp_LR_UR );
		MPBFLOAT angle_UR_UL = acos( dp_UR_UL );
		MPBFLOAT angle_UL_LL = acos( dp_UL_LL );

		MPBFLOAT dAngle = angle_LL_LR + angle_LR_UR + angle_UR_UL + angle_UL_LL;

		if (floatEQ( dAngle, _2PI ))
		{
			// angles add up to 360 (2*PI); intersection point is in the quad.
			planeIntersectionPointIsInPoly = true;

			if (TRACE_SPHERE_PLANE) 
			{
				sprintf( g_logBuf, "dp_LL_LR: %.20f\n", dp_LL_LR );
				writeLog( g_logBuf );

				sprintf( g_logBuf, "dp_LR_UR: %.20f\n", dp_LR_UR );
				writeLog( g_logBuf );

				sprintf( g_logBuf, "dp_UR_UL: %.20f\n", dp_UR_UL );
				writeLog( g_logBuf );

				sprintf( g_logBuf, "dp_UL_LL: %.20f\n", dp_UL_LL );
				writeLog( g_logBuf );

				sprintf( g_logBuf, "angle_LL_LR: %f\n", angle_LL_LR );
				writeLog( g_logBuf );

				sprintf( g_logBuf, "angle_LR_UR: %f\n", angle_LR_UR );
				writeLog( g_logBuf );

				sprintf( g_logBuf, "angle_UR_UL: %f\n", angle_UR_UL );
				writeLog( g_logBuf );

				sprintf( g_logBuf, "angle_UL_LL: %f\n", angle_UL_LL );
				writeLog( g_logBuf );

				PIPToLL.toString( buf );
				sprintf( g_logBuf, "PIPToLL: %s\n", buf );
				writeLog( g_logBuf );

				PIPToLR.toString( buf );
				sprintf( g_logBuf, "PIPToLR: %s\n", buf );
				writeLog( g_logBuf );

				PIPToUR.toString( buf );
				sprintf( g_logBuf, "PIPToUR: %s\n", buf );
				writeLog( g_logBuf );

				PIPToUL.toString( buf );
				sprintf( g_logBuf, "PIPToUL: %s\n", buf );
				writeLog( g_logBuf );

				sprintf( g_logBuf, "dAngle: %f\n", dAngle );
				writeLog( g_logBuf );
			}
		}
	}

	if (TRACE_SPHERE_PLANE) 
	{
		sprintf( g_logBuf, "planeIntersectionPointIsInPoly: %d\n", planeIntersectionPointIsInPoly );
		writeLog( g_logBuf );
	}

	if (planeIntersectionPointIsInPoly)
	{
		polygonIntersectionPoint = planeIntersectionPoint;
	}
	else
	{
		/////////////////////////////////////////////////////////////////
		// find point on quad's perimeter nearest to intersection point.

		// find closest point on each edge
		MPBVector pointOnP1P2 = closestPointOnLine( plane->getLL(), plane->getLR(), planeIntersectionPoint );
		MPBVector pointOnP2P3 = closestPointOnLine( plane->getLR(), plane->getUR(), planeIntersectionPoint );
		MPBVector pointOnP3P4 = closestPointOnLine( plane->getUR(), plane->getUL(), planeIntersectionPoint );
		MPBVector pointOnP4P1 = closestPointOnLine( plane->getUL(), plane->getLL(), planeIntersectionPoint );

		// find distances to those points
		MPBFLOAT pointOnP1P2_distance = (pointOnP1P2 - planeIntersectionPoint).getLength();
		MPBFLOAT pointOnP2P3_distance = (pointOnP2P3 - planeIntersectionPoint).getLength();
		MPBFLOAT pointOnP3P4_distance = (pointOnP3P4 - planeIntersectionPoint).getLength();
		MPBFLOAT pointOnP4P1_distance = (pointOnP4P1 - planeIntersectionPoint).getLength();

		// pick the one that's closest
		MPBFLOAT closest = min(
			min( pointOnP1P2_distance, pointOnP2P3_distance ),
			min( pointOnP3P4_distance, pointOnP4P1_distance )
		);

		if (floatEQ( closest, pointOnP1P2_distance )) 
			polygonIntersectionPoint = pointOnP1P2;
		else if (floatEQ( closest, pointOnP2P3_distance )) 
			polygonIntersectionPoint = pointOnP2P3;
		else if (floatEQ( closest, pointOnP3P4_distance )) 
			polygonIntersectionPoint = pointOnP3P4;
		else if (floatEQ( closest, pointOnP4P1_distance )) 
			polygonIntersectionPoint = pointOnP4P1;
		else MPB_ASSERT( 0 );
	}

	if (TRACE_SPHERE_PLANE) 
	{
		polygonIntersectionPoint.toString( buf );
		sprintf( g_logBuf, "polygonIntersectionPoint: %s\n", buf );
		writeLog( g_logBuf );
	}

	/////////////////////////////////////////////////////
	// find point on sphere where collision would occur

	MPBVector intersectionPointNear;
	MPBVector intersectionPointFar;
	MPBFLOAT distanceToIntersectionNear;
	MPBFLOAT distanceToIntersectionFar;

	bool rayHitsSphere = collisionRaySphere(	polygonIntersectionPoint, 
															/*travelVector * -1,*/velocityVector * -1, 
															sourcePosition, 
															objectRadius, 
															intersectionPointNear,
															intersectionPointFar,
															distanceToIntersectionNear,
															distanceToIntersectionFar	);

	MPBVector polygonToSphereIntersection = intersectionPointNear;
	MPBFLOAT polygonToSphereDistance = distanceToIntersectionNear;

//	sprintf( g_logBuf, "travelDistance = %f", travelDistance );
//	MPBMessageWindow::writeMessage( g_logBuf );
	if (TRACE_SPHERE_PLANE)
	{
		sprintf( g_logBuf, "travelDistance = %+12.12f\n", travelDistance );
		writeLog( g_logBuf );
	}

	if (rayHitsSphere)
	{
		if (TRACE_SPHERE_PLANE)
		{
			sprintf( g_logBuf, "polygonToSphereDistance = %+12.12f\n", polygonToSphereDistance );
			writeLog( g_logBuf );
		}

		if (floatGTE( polygonToSphereDistance, 0 ) && polygonToSphereDistance < travelDistance)
		{
			// see if we're just scraping the edge of the plane
			if (distanceToIntersectionNear == distanceToIntersectionFar)
			{
				if (TRACE_SPHERE_PLANE) writeLog( "NOT COLLIDED (just scraping the edge of the plane)\n" );
				return false;
			}

			/////////////////////////////////////////////////////
			// find closest point we can get to along the velocity vector

			MPBVector newDestPosition = sourcePosition + velocityVector * polygonToSphereDistance;
//			if (newDestPosition.y > 390) PostQuitMessage( 1 );

			/////////////////////////////////////////////////////
			// find slide vector

			MPBVector slideDestination;
			plane->lineIntersect(	destPosition, 
											destPosition + plane->getNormal(), 
											slideDestination	);

			MPBVector slideVector;
			if (embedded)
				slideVector = slideDestination - orthogonalPlaneIntersectionPoint;
			else
				slideVector = slideDestination - planeIntersectionPoint;

			collision = new Collision( polygonToSphereDistance, newDestPosition, slideVector );

			if (TRACE_SPHERE_PLANE) 
			{
				writeLog( "COLLIDED\n" );

				slideDestination.toString( buf );
				sprintf( g_logBuf, "slideDestination: %s\n", buf );
				writeLog( g_logBuf );

				newDestPosition.toString( buf );
				sprintf( g_logBuf, "newDestPosition: %s\n", buf );
				writeLog( g_logBuf );

				slideVector.toString( buf );
				sprintf( g_logBuf, "slideVector: %s\n", buf );
				writeLog( g_logBuf );
			}

			return true;
		}

		if (TRACE_SPHERE_PLANE) writeLog( "NOT COLLIDED (polygonToSphereDistance > travelDistance)\n" );
		return false;
	}

	if (TRACE_SPHERE_PLANE) writeLog( "NOT COLLIDED (ray does not hit sphere)\n" );
//	if (destPosition.y > 390) PostQuitMessage( 1 );
	return false;
}

bool collisionPointAABB(	const MPBVector& point,
									const MPBAABB& aabb	)
{
	MPBVector NLL = aabb.getNLL();
	MPBVector FUR = aabb.getFUR();

	if (	point.x >= NLL.x &&
			point.x <= FUR.x &&
			point.y >= NLL.y &&
			point.y <= FUR.y &&
			point.z <= NLL.z &&
			point.z >= FUR.z	)
		return true;

	return false;
}

// returns point on line ab closest to p
MPBVector closestPointOnLine(	MPBVector a,
										MPBVector b,
										MPBVector p	)
{
	if (a == b) return a;

	MPBVector AP = p - a;
	MPBVector AB = b - a;
	MPBFLOAT lineLength = AB.getLength();
	AB.normalize();

	MPBFLOAT dotProduct = AB.dotProduct( AP );

	if (floatLT( dotProduct, 0 )) return a;
	if (floatGT( dotProduct, lineLength )) return b;

	return a + AB * dotProduct;
}

/*
// if objects 1 and 2 have collided, call to determine resulting velocities
void collisionBounce(	MPBVector& center1ToCenter2,
								const MPBVector& velocity1,
								const MPBVector& velocity2,
								MPBFLOAT mass1,
								MPBFLOAT mass2,
								MPBVector& newVelocity1,		// resulting velocity of object 1
								MPBVector& newVelocity2	)		// resulting velocity of object 2
{
	MPB_ASSERT( mass1 > 0 );
	MPB_ASSERT( mass2 > 0 );

	center1ToCenter2.normalize();
	MPBFLOAT a1 = 0;
	MPBFLOAT a2 = 0;
	if (velocity1.getLength())
		a1 = velocity1.dotProduct( center1ToCenter2 );
	if (velocity2.getLength())
		a2 = velocity2.dotProduct( center1ToCenter2 );
	MPBFLOAT optimizedP = (2.0 * (a1 - a2)) / (mass1 + mass2);

	newVelocity1 = velocity1 - (center1ToCenter2 * (optimizedP * mass2));
	newVelocity2 = velocity2 + (center1ToCenter2 * (optimizedP * mass1));

	MPBFLOAT elasticity = 1;
}
*/

// if objects 1 and 2 have collided, call to determine resulting velocities,
// including angular velocities
void collisionBounce(	MPBFLOAT elasticity,							// between 0.0 and 1.0
								const MPBVector& collisionPoint,			// in world coords
								const MPBVector& velocity1,				// in world coords
								const MPBVector& velocity2,				// in world coords
								const MPBVector& localVelocity1,			// in local coords
								const MPBVector& localVelocity2,			// in local coords
								const MPBVector& angularVelocity1,		// in radians per second, local coords
								const MPBVector& angularVelocity2,		// in radians per second, local coords
								const MPBVector& position1,
								const MPBVector& position2,
								const MPBQuaternion& orientation1,
								const MPBQuaternion& orientation2,
//								const MPBVector& rotation1,				// in radians
//								const MPBVector& rotation2,				// in radians
								MPBFLOAT mass1,
								MPBFLOAT mass2,
								MPBFLOAT inertia1,
								MPBFLOAT inertia2,
								MPBVector& newVelocity1,					// resulting velocity of object 1
								MPBVector& newVelocity2,					// resulting velocity of object 2
								MPBVector& newAngularVelocity1,			// resulting angular velocity of object 1, local coords
								MPBVector& newAngularVelocity2,			// resulting angular velocity of object 2, local coords
								bool ignoreAngular	)
{
	MPB_ASSERT( elasticity >= 0.0 );
	MPB_ASSERT( elasticity <= 1.0 );

	MPBFLOAT term1 = 0;
	MPBFLOAT term2 = 0;
	if (mass1) term1 = 1.0 / mass1;
	if (mass2) term2 = 1.0 / mass2;

	MPBVector collisionNormal = position1 - position2;
	collisionNormal.normalize();

	MPBFLOAT impulseSize;
	MPBVector relativeVelocity;
	MPBVector collisionPoint1;
	MPBVector collisionPoint2;

	if (ignoreAngular)
	{
		relativeVelocity = velocity1 - velocity2;

		impulseSize =	(-(1.0 + elasticity) * (relativeVelocity.dotProduct( collisionNormal ))) /
							((collisionNormal.dotProduct( collisionNormal )) * (term1 + term2));
	}
	else
	{
		collisionPoint1 = collisionPoint - position1;	// convert to object coords
		collisionPoint2 = collisionPoint - position2;	// convert to object coords

/*		MPBVector v1 = localVelocity1 + angularVelocity1.crossProduct( collisionPoint1 );
		MPBVector v2 = localVelocity2 + angularVelocity2.crossProduct( collisionPoint2 );

		// convert velocities to world coords
		orientation1.rotateVector( v1 );
		orientation2.rotateVector( v2 );

		relativeVelocity = v1 - v2;
*/
		relativeVelocity = velocity1 - velocity2;

		MPBFLOAT invInertia1 = 0;
		if (inertia1) invInertia1 = 1.0 / inertia1;

		MPBFLOAT invInertia2 = 0;
		if (inertia2) invInertia2 = 1.0 / inertia2;

		MPBFLOAT term3 = collisionNormal.dotProduct( 
			(collisionPoint1.crossProduct( collisionNormal ) * invInertia1).crossProduct( collisionPoint1 )
		);

		MPBFLOAT term4 = collisionNormal.dotProduct( 
			(collisionPoint2.crossProduct( collisionNormal ) * invInertia2).crossProduct( collisionPoint2 )
		);

		MPB_ASSERT( term1 + term2 + term3 + term4 );

		impulseSize =	(-(1.0 + elasticity) * (relativeVelocity.dotProduct( collisionNormal ))) /
							( term1 + term2 + term3 + term4 );
	}

	MPBVector impulse = collisionNormal * impulseSize;

/*	MPBFLOAT frictionCoefficient = -.01;
	MPBVector collisionTangent = collisionNormal.crossProduct( relativeVelocity ).crossProduct( collisionNormal );
	if (collisionTangent != MPBVector( 0, 0, 0 )) collisionTangent.normalize();
	MPBVector friction = collisionTangent * frictionCoefficient * impulseSize;

	impulse = impulse + friction;
*/
	if (mass1) 
		newVelocity1 = velocity1 + impulse / mass1;
	else 
		newVelocity1 = velocity1;

	if (mass2) 
		newVelocity2 = velocity2 - impulse / mass2;
	else 
		newVelocity2 = velocity2;

	if (ignoreAngular) 
	{
		newAngularVelocity1 = angularVelocity1;
		newAngularVelocity2 = angularVelocity2;
	}
	else
	{
		MPBVector addAngularVelocity1;
		MPBVector addAngularVelocity2;

		if (inertia1) 
		{
			addAngularVelocity1 = collisionPoint1.crossProduct( impulse ) / inertia1;

			// convert angular velocity to local coords
			MPBQuaternion tmp = ~orientation1;
			tmp.rotateVector( addAngularVelocity1 );
		}

		newAngularVelocity1 = angularVelocity1 + addAngularVelocity1;

		if (inertia2) 
		{
			newAngularVelocity2 = collisionPoint2.crossProduct( impulse ) / inertia2;

			// convert angular velocity to local coords
			MPBQuaternion tmp = ~orientation2;
			tmp.rotateVector( addAngularVelocity2 );
		}

		newAngularVelocity2 = angularVelocity2 - addAngularVelocity2;
	}

	////////////////////////////////////////////

/*	if (mass1 && mass2)
	{
		MPBFLOAT momentumBefore = (velocity1 * mass1 + velocity2 * mass2).getLength();
		MPBFLOAT momentumAfter = (newVelocity1 * mass1 + newVelocity2 * mass2).getLength();
		MPB_ASSERT( floatEQ( momentumBefore, momentumAfter ) );
	}
*/
/*	char buf[1000];

	sprintf( g_logBuf, "impulse: %f, mass1: %f, mass2: %f\n", impulse, mass1, mass2 );
	writeLog();

	v1.toString( buf );
	sprintf( g_logBuf, "v1: %s\n", buf );
	writeLog();

	v2.toString( buf );
	sprintf( g_logBuf, "v2: %s\n", buf );
	writeLog();

	relativeVelocity.toString( buf );
	sprintf( g_logBuf, "relativeVelocity: %s\n", buf );
	writeLog();

	sprintf( g_logBuf, "momentumBeforeTotal: %f, momentumAfterTotal: %f\n", momentumBeforeTotal, momentumAfterTotal );
	writeLog();

	writeLog( "\n" );
*/
/*
	sprintf( g_logBuf, "%f", j );
	writeLog( g_logBuf );
	writeLog( "\n" );
	newVelocity1.toString( g_logBuf );
	writeLog( g_logBuf );
	writeLog( "\n" );
	newVelocity2.toString( g_logBuf );
	writeLog( g_logBuf );
	writeLog( "\n" );

	sprintf( g_logBuf, "%f", newVelocity1.getLength() );
	writeLog( g_logBuf );
	MPBMessageWindow::writeMessage( g_logBuf );
	writeLog( "\n" );
	sprintf( g_logBuf, "%f", newVelocity2.getLength() );
	writeLog( g_logBuf );
	writeLog( "\n" );

	writeLog( "\n" );
*/
}

// returns true if path is not blocked by other objects or shapes
bool pathIsClearSphere(
	MPBVector from,
	MPBVector to,
	MPBFLOAT radius,
	bool testObjects,
	MPBPhysicsObject* source,	// object to ignore collisions with, can be null
	MPBPhysicsObject* target,	// object to ignore collisions with, can be null
	Collision*& collision		// if not null, closest collision will be determined
)
{
	MPBList collisionList;
	bool findClosest = false;
	if (collision) findClosest = true;

//	Collision* collision = NULL;
	MPBVector fromTo = to - from;
	MPBFLOAT distance = fromTo.getLength();
	if (distance == 0) return true;
	MPBVector fromToNormalized = fromTo;
	fromToNormalized.normalize();

	if (testObjects)
	{
		MPB_LIST_LOOP( MPBPhysics::objects, obj, MPBPhysicsObject* )
		{
			if ((MPBPhysicsObject*)obj == source) continue;
			if ((MPBPhysicsObject*)obj == target) continue;

			if (collisionSphereSphere(	from,
												to,
												radius,
												obj->getPosition(),
												obj->getRadius(),
												fromToNormalized,
												distance,
												collision	))
			{
				if (findClosest)
				{
					collisionList.push_back( collision );
					collision->m_targetObject = obj;
				}
				else
				{
//					NULLIFY( collision );
					return false;
				}
			}
		}
	}

	// make a box that bounds the start and destination positions
	MPBVector extents( radius, radius, radius );

	MPBAABB startBox;
	startBox.setCenterAndExtents( from, extents );

	MPBAABB destBox;
	destBox.setCenterAndExtents( to, extents );

	MPBAABB moveBox;
	moveBox.bound( startBox, destBox );

	MPBList planes;
	MPBGameEntities::getCollisionPlanes( planes, moveBox );

	MPB_LIST_LOOP( planes, plane, Plane* )
	{
		if (!plane->isCollidable()) continue;

		if (collisionSpherePlane(	plane, 
											from, 
											to, 
											fromToNormalized,
											fromTo,
											distance,
											radius,
											collision	))
		{
			if (findClosest)
			{
				collisionList.push_back( collision );
			}
			else
			{
//				NULLIFY( collision );
				return false;
			}
		}
	}

	if (findClosest)
	{
		if (collisionList.size())
		{
			Collision* c = (Collision*)collisionList[0];
			MPB_LIST_LOOP( collisionList, c2, Collision* )
			{
				if (c != c2 && collisionLessThan( c2, c )) c = c2;
			}

			collision = new Collision( c->m_distance, c->m_newDestination, c->m_slideVector );
			collision->m_targetObject = c->m_targetObject;
			collision->m_targetPlane = c->m_targetPlane;

			collisionList.deleteAll();

			return false;
		}
	}
	else
	{
//		NULLIFY( collision );
	}

	return true;
}

// returns true if path is not blocked by other objects or shapes
bool pathIsClearAABB(
	MPBVector from,
	MPBVector to,
	MPBFLOAT radius,
	bool testObjects,
	MPBPhysicsObject* source,	// object to ignore collisions with, can be null
	MPBPhysicsObject* target	// object to ignore collisions with, can be null
)
{
	Collision* collision = NULL;
	MPBVector fromTo = to - from;
	MPBFLOAT distance = fromTo.getLength();
	MPBVector fromToNormalized = fromTo;
	fromToNormalized.normalize();

	if (testObjects)
	{
		MPB_LIST_LOOP( MPBPhysics::objects, obj, MPBPhysicsObject* )
		{
			if (obj == source) continue;
			if (obj == target) continue;

			if (collisionSphereSphere(	from,
												to,
												radius,
												obj->getPosition(),
												obj->getRadius(),
												fromToNormalized,
												distance,
												collision	))
			{
				NULLIFY( collision );
				return false;
			}
		}
	}

	// make a box that bounds the start and destination positions
	MPBVector extents( radius, radius, radius );

	MPBAABB startBox;
	startBox.setCenterAndExtents( from, extents );

	MPBAABB destBox;
	destBox.setCenterAndExtents( to, extents );

	MPBAABB moveBox;
	moveBox.bound( startBox, destBox );

	MPBList planes;
	MPBGameEntities::getCollisionPlanes( planes, moveBox );

	MPB_LIST_LOOP( planes, plane, Plane* )
	{
		if (!plane->isCollidable()) continue;

		if (collisionSpherePlane(	plane, 
											from, 
											to, 
											fromToNormalized,
											fromTo,
											distance,
											radius,
											collision	))
		{
			NULLIFY( collision );
			return false;
		}
	}

	NULLIFY( collision );
	return true;
}

// frustum can have NULL planes
bool aabbInFrustum( const MPBAABB& aabb, Frustum& frustum )
{
	MPBVector vertices[8];
	vertices[0] = aabb.getNLL();
	vertices[1] = aabb.getNLR();
	vertices[2] = aabb.getNUR();
	vertices[3] = aabb.getNUL();
	vertices[4] = aabb.getFLL();
	vertices[5] = aabb.getFLR();
	vertices[6] = aabb.getFUR();
	vertices[7] = aabb.getFUL();
	return pointsInFrustum( vertices, 8, frustum );
}

// frustum can have NULL planes
bool sphereInFrustum( const MPBVector& center, MPBFLOAT radius, Frustum& frustum )
{
	if (frustum.Left && frustum.Left->isSphereInside( center, radius )) return false;
	if (frustum.Right && frustum.Right->isSphereInside( center, radius )) return false;
	if (frustum.Top&& frustum.Top->isSphereInside( center, radius )) return false;
	if (frustum.Bottom && frustum.Bottom->isSphereInside( center, radius )) return false;
	if (frustum.Near && frustum.Near->isSphereInside( center, radius )) return false;
	if (frustum.Far && frustum.Far->isSphereInside( center, radius )) return false;

	return true;
}

// returns false only if all points are inside any frustum plane
// frustum can have NULL planes
bool pointsInFrustum( MPBVector* pointList, int points, Frustum& frustum )
{
	if (frustum.Left && frustum.Left->arePointsInside( pointList, points )) return false;
	if (frustum.Right && frustum.Right->arePointsInside( pointList, points )) return false;
	if (frustum.Top && frustum.Top->arePointsInside( pointList, points )) return false;
	if (frustum.Bottom && frustum.Bottom->arePointsInside( pointList, points )) return false;
	if (frustum.Near && frustum.Near->arePointsInside( pointList, points )) return false;
	if (frustum.Far && frustum.Far->arePointsInside( pointList, points )) return false;

	return true;
}

// polyhedron can have NULL planes
bool aabbInPolyhedron( const MPBAABB& aabb, Polyhedron& polyhedron )
{
	MPBVector vertices[8];
	vertices[0] = aabb.getNLL();
	vertices[1] = aabb.getNLR();
	vertices[2] = aabb.getNUR();
	vertices[3] = aabb.getNUL();
	vertices[4] = aabb.getFLL();
	vertices[5] = aabb.getFLR();
	vertices[6] = aabb.getFUR();
	vertices[7] = aabb.getFUL();
	return pointsInPolyhedron( vertices, 8, polyhedron );
}

// frustum can have NULL planes
bool sphereInPolyhedron( const MPBVector& center, MPBFLOAT radius, Polyhedron& polyhedron )
{
	MPB_LIST_LOOP( polyhedron.planes, plane, Plane* )
	{
		if (plane && plane->isSphereInside( center, radius )) return false;
	}

	return true;
}

// returns false only if all points are inside any polyhedron plane
// polyhedron can have NULL planes
bool pointsInPolyhedron( MPBVector* pointList, int points, Polyhedron& polyhedron )
{
	MPB_LIST_LOOP( polyhedron.planes, plane, Plane* )
	{
		if (plane && plane->arePointsInside( pointList, points )) return false;
	}

	return true;
}


void findHalfspaceIntersection( MPBList& in, MPBList& out )
{
	HANDLE h = CreateFile(
		"C:/MPB/3DCollisions/qhalfOutput.txt",
		GENERIC_READ | GENERIC_WRITE,
		0,
		NULL,
		CREATE_ALWAYS, 
		0,
		0
	);

	if (h == INVALID_HANDLE_VALUE)
	{
		MPB_ASSERT( 0 );
	}

	PROCESS_INFORMATION pi;
	ZeroMemory( &pi, sizeof( PROCESS_INFORMATION ) );

	STARTUPINFO si;
	ZeroMemory( &si, sizeof( STARTUPINFO ) );
	si.cb = sizeof( STARTUPINFO );
	si.dwFlags = STARTF_USESHOWWINDOW | STARTF_USESTDHANDLES;
	si.wShowWindow = SW_HIDE;
	si.hStdOutput = h;
	si.hStdError = h;

	BOOL b = CreateProcess(
		"C:/MPB/3DCollisions/qhalf.exe",
		"Fx "
			"2 1\n"
			"0	0\n"
			"3\n"
			"8\n"
			"0	-1	-1\n"
			"-1	0	-1\n"
			"1	0	-1\n"
			"0	1	-1\n"
			"0	-1	-2\n"
			"-1	0	-.5\n"
			"1	0	-.5\n"
			"0	1	-2\n",
		NULL,
		NULL,
		0,
		0,
		NULL,
		NULL,
		&si,
		&pi
	);

	if (!b) 
	{
		writeLastError( "findHalfspaceIntersection: " );
		MPB_ASSERT( 0 );
	}

	CloseHandle( h );

	MPB_LIST_LOOP( in, plane, Plane* )
	{
		out.push_back( plane );
	}
}



//////////////////////////////////////////////////////////////////


void MPBCollisionTest()
{
	collisionRaySphereTest();


	bool rc;

	Collision* collision;
	MPBVector sourcePosition;
	MPBVector destPosition;
	MPBVector velocityVector;
	MPBVector travelVector;

	sourcePosition.set( 1.400000000000, +0.979803279942, -0.078088639561 );		// was broken
//	sourcePosition.set( +1.400000000000, +0.979802433369, -0.076318300944 );	// working

	travelVector.set( 0.000000000000, -0.000008953427, -0.001790338617 );

	destPosition = sourcePosition + travelVector;
	velocityVector = travelVector;
	velocityVector.normalize();
	rc = collisionSpherePlane(
		new Plane(
			MPBVector( 1, 0, 10 ), 
			MPBVector( 1.2, 0, 10 ), 
			MPBVector( 1.2, 0, -10), 
			MPBVector( 1, 0, -10)	),
		sourcePosition, 
		destPosition, 
		velocityVector,
		travelVector,
		travelVector.getLength(),
		1.0,
		collision	);

	MPB_ASSERT( rc == true );


	sourcePosition.set( 0, 2, 0 );
	travelVector.set( 0, -4, 0 );

	destPosition = sourcePosition + travelVector;
	velocityVector = travelVector;
	velocityVector.normalize();
	rc = collisionSpherePlane(
		new Plane(
			MPBVector( -2, -2, 10 ), 
			MPBVector( 2, 2, 10 ), 
			MPBVector( 2, 2, -10), 
			MPBVector( -2, -2, -10)	),
		sourcePosition, 
		destPosition, 
		velocityVector,
		travelVector,
		travelVector.getLength(),
		1.0,
		collision	);

	MPB_ASSERT( floatEQ( collision->m_slideVector.x, -1.707107 ) );
	MPB_ASSERT( floatEQ( collision->m_slideVector.y, -1.707107 ) );
	MPB_ASSERT( floatEQ( collision->m_slideVector.z, 0 ) );
	MPB_ASSERT( rc == true );


	sourcePosition = collision->m_newDestination;
	travelVector = collision->m_slideVector;

	destPosition = sourcePosition + travelVector;
	velocityVector = travelVector;
	velocityVector.normalize();
	rc = collisionSpherePlane(
		new Plane(
			MPBVector( -2, -2, 10 ), 
			MPBVector( 2, 2, 10 ), 
			MPBVector( 2, 2, -10), 
			MPBVector( -2, -2, -10)	),
		sourcePosition, 
		destPosition, 
		velocityVector,
		travelVector,
		travelVector.getLength(),
		1.0,
		collision	);

	MPB_ASSERT( rc == false );
}






