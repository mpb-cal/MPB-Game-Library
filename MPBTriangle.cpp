

#include "MPBLibrary.h"
#include "MPBTriangle.h"
#include "MPBGlobal.h"
#include "MPBCollision.h"
#include "MPBOpenGL.h"
#include "mmgr.h"



MPBTriangle::MPBTriangle():
	m_collidable( true ),
	m_polygonOffsetFactor( 0 ),
	m_polygonOffsetUnits( 0 )
{
}

MPBTriangle::MPBTriangle(	const MPBVector& _v1,
									const MPBVector& _v2,
									const MPBVector& _v3	):
	m_polygonOffsetFactor( 0 ),
	m_polygonOffsetUnits( 0 )
{
	setCorners( _v1, _v2, _v3 );
}

MPBTriangle::~MPBTriangle()
{
}

void MPBTriangle::toString( char* buf ) const
{
	sprintf( buf, "%+12f, %+12f, %+12f, %+12f", m_normal.x, m_normal.y, m_normal.z, m_d );
}

void MPBTriangle::setNormal( const MPBVector& normal )
{
	MPB_ASSERT( floatEQ( normal.getLength(), 1 ) );
	m_normal = normal;
}

void MPBTriangle::setD( MPBFLOAT d )
{
	m_d = d;
}

void MPBTriangle::setCorners(	const MPBVector& _v1,
										const MPBVector& _v2,
										const MPBVector& _v3	)
{
	vertex[0] = _v1;
	vertex[1] = _v2;
	vertex[2] = _v3;
	findEquation();
}

const MPBTriangle& MPBTriangle::operator=( const MPBTriangle& rhs )
{
	setCorners( rhs.vertex[0], rhs.vertex[1], rhs.vertex[2] );

	return *this;
}

void MPBTriangle::findEquation()
{
/*

								vertex[2]
						 ^
						/
					V2
				/

		vertex[0]			->			vertex[1]
					vertex[0]

*/

	MPB_ASSERT( vertex[0] != vertex[1] );
	MPB_ASSERT( vertex[0] != vertex[2] );
	MPB_ASSERT( vertex[1] != vertex[2] );

	MPBVector V1;
	MPBVector V2;

	V1 = vertex[1] - vertex[0];
	V2 = vertex[2] - vertex[0];

	V1.normalize();
	V2.normalize();

	// find cross product of V1 and V2 to get normal

	MPBVector normal = V1.crossProduct( V2 );
	normal.normalize();
	setNormal( normal );

	MPBVector pointOnPlane = vertex[0];
	if (pointOnPlane.getLength() == 0) pointOnPlane = vertex[1];
	if (pointOnPlane.getLength() == 0) pointOnPlane = vertex[2];

	setD( -m_normal.dotProduct( pointOnPlane ) );
}

MPBFLOAT MPBTriangle::distanceToPoint( MPBVector point ) const
{
	return m_normal.dotProduct( point ) + m_d;
}

// aka in front of
bool MPBTriangle::isSphereOutside( MPBVector center, MPBFLOAT radius ) const
{
	return (distanceToPoint( center ) > -radius);
}

// aka in back of
bool MPBTriangle::isSphereInside( MPBVector center, MPBFLOAT radius ) const
{
	return !isSphereOutside( center, radius );
}

// aka in front of
bool MPBTriangle::isPointOutside( MPBVector point ) const
{
	return (distanceToPoint( point ) > 0);
}

// aka in back of
bool MPBTriangle::isPointInside( MPBVector point ) const
{
	return !isPointOutside( point );
}

bool MPBTriangle::arePointsInside( MPBVector* pointList, int numPoints ) const
{
	for (int point=0; point<numPoints; point++)
	{
		if (isPointOutside( pointList[point] )) return false;
	}

	return true;
}

// returns the y-value for a given x,z
MPBFLOAT MPBTriangle::getYAtPoint( MPBFLOAT x, MPBFLOAT z ) const
{
	MPBFLOAT y;

	if (m_normal.y == 0) 
	{
		y = vertex[0].y;
	}
	else
	{
		y =	(	-(	m_normal.x * (x - vertex[0].x) + 
						m_normal.z * (z - vertex[0].z)	)
				) /
				m_normal.y +
				vertex[0].y;
	}

	return y;
}

// tests if line intersects plane and is within corners
bool MPBTriangle::lineInsideCorners(	const MPBVector& start, 
													const MPBVector& end, 
													MPBVector& intersection ) const
{
	if (!lineIntersect( start, end, intersection )) return false;

	/////////////////////////////////////////////////////
	// check if intersection is inside quad by adding up angles
	// from corners to intersection point: 360 = inside quad

	bool intersectionIsInPoly = false;

	if (intersection == vertex[0]) intersectionIsInPoly = true;
	else if (intersection == vertex[1]) intersectionIsInPoly = true;
	else if (intersection == vertex[2]) intersectionIsInPoly = true;
	else
	{
		MPBVector PIPToV1 = vertex[0] - intersection;
		MPBVector PIPToV2 = vertex[1] - intersection;
		MPBVector PIPToV3 = vertex[2] - intersection;

		PIPToV1.normalize();
		PIPToV2.normalize();
		PIPToV3.normalize();

		MPBFLOAT dp_V1_V2 = PIPToV1.dotProduct( PIPToV2 );
		MPBFLOAT dp_V2_V3 = PIPToV2.dotProduct( PIPToV3 );
		MPBFLOAT dp_V3_V1 = PIPToV3.dotProduct( PIPToV1 );

		// some dot products are slightly out of range due to fp precision errors
		if (dp_V1_V2 > 1) dp_V1_V2 = 1;
		if (dp_V2_V3 > 1) dp_V2_V3 = 1;
		if (dp_V3_V1 > 1) dp_V3_V1 = 1;

		if (dp_V1_V2 < -1) dp_V1_V2 = -1;
		if (dp_V2_V3 < -1) dp_V2_V3 = -1;
		if (dp_V3_V1 < -1) dp_V3_V1 = -1;

		MPBFLOAT angle_V1_V2 = (float)acos( dp_V1_V2 );
		MPBFLOAT angle_V2_V3 = (float)acos( dp_V2_V3 );
		MPBFLOAT angle_V3_V1 = (float)acos( dp_V3_V1 );

		MPBFLOAT dAngle = angle_V1_V2 + angle_V2_V3 + angle_V3_V1;

		if (floatEQ( dAngle, PI ))
		{
			// angles add up to 180 (PI radians); intersection point is in the triangle.
			intersectionIsInPoly = true;
		}
	}


	return intersectionIsInPoly;
}

// returns false only if line is parallel to plane
// does not test if line is within corners
bool MPBTriangle::lineIntersect(	const MPBVector& start, 
									const MPBVector& end, 
									MPBVector& intersection ) const
{
	intersection.set( 0, 0, 0 );

	MPBVector line = end - start;
	line.normalize();

	MPBFLOAT divisor = m_normal.dotProduct( line );

	// if plane's normal is perpendicular to line,
	// line is parallel to plane
	if (floatEQ( divisor, 0 )) return false;

	MPBFLOAT t = (-m_normal.x * start.x - 
					 m_normal.y * start.y - 
					 m_normal.z * start.z - 
					 m_d) 
					/ divisor;
	intersection.x = start.x + line.x * t;
	intersection.y = start.y + line.y * t;
	intersection.z = start.z + line.z * t;

	return true;
}

// returns false if line is parallel to plane
// or if endpoints don't contain the plane
// does not test if line is within corners
bool MPBTriangle::rayIntersect(	const MPBVector& start, 
									const MPBVector& end, 
									MPBVector& intersection ) const
{
	intersection.set( 0, 0, 0 );

	MPBVector line = end - start;
	if (line == MPBVector()) return false;
	line.normalize();

	MPBFLOAT divisor = m_normal.dotProduct( line );

	// if plane's normal is perpendicular to line,
	// line is parallel to plane
//	if (floatEQ( divisor, 0 )) return false;
	if (divisor == 0) return false;

	MPBFLOAT t = (-m_normal.x * start.x - 
					 m_normal.y * start.y - 
					 m_normal.z * start.z - 
					 m_d) 
					/ divisor;

	// endpoints don't contain the plane
	if (t < 0) return false;

	intersection.x = start.x + line.x * t;
	intersection.y = start.y + line.y * t;
	intersection.z = start.z + line.z * t;

	return true;
}

// true if any part of this plane is in the box
bool MPBTriangle::AABBIntersect( const MPBAABB& aabb ) const
{
	// first, check if any corner is in the box

//	if (collisionPointAABB( vertex[0], aabb )) return true;
//	if (collisionPointAABB( vertex[1], aabb )) return true;
//	if (collisionPointAABB( vertex[2], aabb )) return true;

//	return false;

	return true;

/*
	// second, check if any edge goes through any of the box's 6 planes

	vector<Plane*> aabbPlanes;
	aabb.getPlanes( aabbPlanes );

	VECTOR_LOOP( aabbPlanes, Plane*, itr )
	{
		Plane* plane = *itr;
		MPBVector intersection;

		if (plane->rayIntersect( vertex[0], (vertex[0] - vertex[1]), intersection ))
		{
			// check if intersection is inside quad by adding up angles
			// from corners to intersection point: 360 = inside quad

			MPBVector intersectionToP1 = plane->vertex[0] - intersection;
			MPBVector intersectionToP2 = plane->vertex[1] - intersection;
			MPBVector intersectionToP3 = plane->vertex[2] - intersection;
			MPBVector intersectionToP4 = plane->getUL() - intersection;

			intersectionToP1.normalize();
			intersectionToP2.normalize();
			intersectionToP3.normalize();
			intersectionToP4.normalize();

			MPBFLOAT dAngle =
				acos( intersectionToP1.dotProduct( intersectionToP2 ) ) + 
				acos( intersectionToP2.dotProduct( intersectionToP3 ) ) + 
				acos( intersectionToP3.dotProduct( intersectionToP4 ) ) + 
				acos( intersectionToP4.dotProduct( intersectionToP1 ) );

			if (fabs( dAngle - 2 * PI ) < EPSILON)
			{
				return true;
			}
		}
	}

	return false;
*/
}

void MPBTriangle::translate( MPBFLOAT x, MPBFLOAT y, MPBFLOAT z )
{
	vertex[0].translate( x, y, z );
	vertex[1].translate( x, y, z );
	vertex[2].translate( x, y, z );

	findEquation();
}

void MPBTriangle::rotateX( MPBFLOAT degrees )
{
	vertex[0].rotateX( degrees );
	vertex[1].rotateX( degrees );
	vertex[2].rotateX( degrees );

	findEquation();
}

void MPBTriangle::rotateY( MPBFLOAT degrees )
{
	vertex[0].rotateY( degrees );
	vertex[1].rotateY( degrees );
	vertex[2].rotateY( degrees );

	findEquation();
}

void MPBTriangle::rotateZ( MPBFLOAT degrees )
{
	vertex[0].rotateZ( degrees );
	vertex[1].rotateZ( degrees );
	vertex[2].rotateZ( degrees );

	findEquation();
}

void MPBTriangle::scale( MPBFLOAT x, MPBFLOAT y, MPBFLOAT z )
{
	vertex[0].scale( x, y, z );
	vertex[1].scale( x, y, z );
	vertex[2].scale( x, y, z );

	findEquation();
}

void MPBTriangle::draw()
{
	glNormal3f( m_normal.x, m_normal.y, m_normal.z );

	if (MPBOpenGL::is2D()) 
	{
		glPushAttrib( GL_TEXTURE_BIT );
		glDisable( GL_TEXTURE_2D );
		glBegin( GL_LINE_LOOP );
	}
	else 
	{
		MPBOpenGL::bindTexture( "planeTexture" );
		glBegin( GL_QUADS );
	}

	if (m_polygonOffsetFactor || m_polygonOffsetUnits)
	{
		glEnable( GL_POLYGON_OFFSET_FILL );
		glPolygonOffset( m_polygonOffsetFactor, m_polygonOffsetUnits );
	}

	glColor4f( 0, 0, 0, 1 );
	glVertex3d( vertex[0].x, vertex[0].y, vertex[0].z );
	glVertex3d( vertex[1].x, vertex[1].y, vertex[1].z );
	glVertex3d( vertex[2].x, vertex[2].y, vertex[2].z );
	glEnd();

	if (m_polygonOffsetFactor || m_polygonOffsetUnits)
		glDisable( GL_POLYGON_OFFSET_FILL );

	if (MPBOpenGL::is2D()) 
	{
		glPopAttrib();
	}
}

void MPBTriangle::drawVertices() const
{
	glNormal3f( m_normal.x, m_normal.y, m_normal.z );

	if (m_polygonOffsetFactor || m_polygonOffsetUnits)
	{
		glEnable( GL_POLYGON_OFFSET_FILL );
		glPolygonOffset( m_polygonOffsetFactor, m_polygonOffsetUnits );
	}

	glBegin( GL_TRIANGLES );
	glVertex3d( vertex[0].x, vertex[0].y, vertex[0].z );
	glVertex3d( vertex[1].x, vertex[1].y, vertex[1].z );
	glVertex3d( vertex[2].x, vertex[2].y, vertex[2].z );
	glEnd();

	if (m_polygonOffsetFactor || m_polygonOffsetUnits)
		glDisable( GL_POLYGON_OFFSET_FILL );
}

void MPBTriangle::drawVertices2() const
{
	if (m_polygonOffsetFactor || m_polygonOffsetUnits)
	{
		glEnable( GL_POLYGON_OFFSET_FILL );
		glPolygonOffset( m_polygonOffsetFactor, m_polygonOffsetUnits );
	}

	glVertex3d( vertex[0].x, vertex[0].y, vertex[0].z );
	glVertex3d( vertex[1].x, vertex[1].y, vertex[1].z );
	glVertex3d( vertex[2].x, vertex[2].y, vertex[2].z );

	if (m_polygonOffsetFactor || m_polygonOffsetUnits)
		glDisable( GL_POLYGON_OFFSET_FILL );
}

void MPBTriangle::setPolygonOffset( GLfloat factor, GLfloat units )
{
	m_polygonOffsetFactor = factor;
	m_polygonOffsetUnits = units;
}



/////////////////////////////////////////////////////////////////

void MPBTriangle::test()
{
	Plane plane;
	MPBVector LL;
	MPBVector LR;
	MPBVector UR;
	MPBVector UL;
	MPBVector point;
	MPBFLOAT dist;

	// a 100x100 plane in the XY plane, lower left at the origin
	LL.set( 0, 0, 0 );
	LR.set( 100, 0, 0 );
	UR.set( 100, 100, 0 );
	UL.set( 0, 100, 0 );
	plane.setCorners( LL, LR, UR, UL );

	MPB_ASSERT( plane.getNormal().x == 0 );
	MPB_ASSERT( plane.getNormal().y == 0 );
	MPB_ASSERT( plane.getNormal().z == 1 );

	point.set( 0, 0, -100 );
	dist = plane.distanceToPoint( point );
	MPB_ASSERT( dist == -100 );

	point.set( 0, 0, 100 );
	dist = plane.distanceToPoint( point );
	MPB_ASSERT( dist == 100 );

	point.set( 0, 50, 100 );
	dist = plane.distanceToPoint( point );
	MPB_ASSERT( dist == 100 );

	point.set( 0, 0, 10 );
	dist = plane.distanceToPoint( point );
	MPB_ASSERT( dist == 10 );


	point.set( 0, 0, 10 );
	MPB_ASSERT( plane.isPointOutside( point ) == true );

	point.set( 0, 0, 10 );
	MPB_ASSERT( plane.isPointInside( point ) == false );

	point.set( 0, 0, -10 );
	MPB_ASSERT( plane.isPointOutside( point ) == false );

	LL.set( 0, 0, -100 );
	LR.set( 100, 0, -100 );
	UR.set( 100, 100, -100 );
	UL.set( 0, 100, -100 );
	plane.setCorners( LL, LR, UR, UL );

	point.set( 0, 0, 10 );
	MPB_ASSERT( plane.isPointOutside( point ) == true );

	point.set( 0, 0, -110 );
	MPB_ASSERT( plane.isPointOutside( point ) == false );

}





