
#include "MPBGlobal.h"
#include "MPBPlane.h"
#include "MPBCollision.h"
#include "MPBOpenGL.h"
#include "MPBTriangle.h"
#include "mmgr.h"


const MPBFLOAT DRAW_SCALE = 1.0;


//
// if created with 3 points, they are always LL, LR and UR,
// and UL is extrapolated.
//

Plane::Plane():
	m_red( 1 ),
	m_green( 1 ),
	m_blue( 1 ),
	m_alpha( 1 ),
	m_collidable( true ),
	m_polygonOffsetFactor( 0 ),
	m_polygonOffsetUnits( 0 )
{
}

Plane::Plane(	const MPBVector& LL,
					const MPBVector& LR,
					const MPBVector& UR,
					const MPBVector& UL	):
	m_red( 1 ),
	m_green( 1 ),
	m_blue( 1 ),
	m_alpha( 1 ),
	m_polygonOffsetFactor( 0 ),
	m_polygonOffsetUnits( 0 )
{
	setCorners( LL, LR, UR, UL );
}

Plane::Plane(	const MPBVector& LL,
					const MPBVector& LR,
					const MPBVector& UR	):
	m_red( 1 ),
	m_green( 1 ),
	m_blue( 1 ),
	m_alpha( 1 ),
	m_polygonOffsetFactor( 0 ),
	m_polygonOffsetUnits( 0 )
{
	setCorners( LL, LR, UR );
}

Plane::~Plane()
{
}

void Plane::toString( char* buf ) const
{
	sprintf( buf, "%+12f, %+12f, %+12f, %+12f", m_normal.x, m_normal.y, m_normal.z, m_d );
}

void Plane::setNormal( const MPBVector& normal )
{
	MPB_ASSERT( floatEQ( normal.getLength(), 1 ) );
	m_normal = normal;
}

void Plane::setD( MPBFLOAT d )
{
	m_d = d;
}

void Plane::setCorners(	const MPBVector& LL,
								const MPBVector& LR,
								const MPBVector& UR,
								const MPBVector& UL	)
{
	m_LL = LL;
	m_LR = LR;
	m_UR = UR;
	m_UL = UL;
	findEquation();
}

void Plane::setCorners(	const MPBVector& LL,
								const MPBVector& LR,
								const MPBVector& UR	)
{
	MPBVector UR2LL = LL - UR;
	UR2LL.setLength( UR2LL.getLength() / 2 );
	setCorners( LL, LR, UR, UR + UR2LL );
}

const Plane& Plane::operator=( const Plane& rhs )
{
	setCorners( rhs.m_LL, rhs.m_LR, rhs.m_UR, rhs.m_UL );

	return *this;
}

void Plane::findEquation()
{
/*

		UL						UR

		^	V2
		|

		LL			->			LR
					V1

*/

	MPBVector V1;
	MPBVector V2;

/*	MPB_ASSERT( m_LL != m_LR );
	MPB_ASSERT( m_LL != m_UR );
	MPB_ASSERT( m_LL != m_UL );

	MPB_ASSERT( m_LR != m_UR );
	MPB_ASSERT( m_LR != m_UL );

	MPB_ASSERT( m_UR != m_UL );
*/
	if (m_LL == m_LR)	V1 = m_UL - m_LL;
	else					V1 = m_LR - m_LL;

//	if (m_UR == m_LR)	V2 = m_LL - m_LR;
//	else					V2 = m_UR - m_LR;

	if (m_UL == m_LL)	V2 = m_UR - m_LL;
	else					V2 = m_UL - m_LL;

	MPB_ASSERT( V1.getLength() );
	MPB_ASSERT( V2.getLength() );

	m_width = V1.getLength();
	m_height = V2.getLength();
	m_area = m_width * m_height;

	V1.normalize();
	V2.normalize();

	// find cross product of V1 and V2 to get normal

//	MPBVector normal = V2.crossProduct( V1 );
	MPBVector normal = V1.crossProduct( V2 );
	normal.normalize();
	setNormal( normal );

	MPBVector pointOnPlane = m_LL;
	pointOnPlane = m_LL;
	if (pointOnPlane.getLength() == 0) pointOnPlane = m_LR;
	if (pointOnPlane.getLength() == 0) pointOnPlane = m_UR;
	if (pointOnPlane.getLength() == 0) pointOnPlane = m_UL;

	setD( -m_normal.dotProduct( pointOnPlane ) );

	m_center.getMidpoint( m_UR, m_LL );
}

// flips left and right vertices
void Plane::flipHorizontal()
{
	setCorners( getLR(), getLL(), getUL(), getUR() );
}

MPBFLOAT Plane::distanceToPoint( MPBVector point ) const
{
	return m_normal.dotProduct( point ) + m_d;
}

// aka in front of
bool Plane::isSphereOutside( MPBVector center, MPBFLOAT radius ) const
{
	return (distanceToPoint( center ) > -radius);
}

// aka in back of
bool Plane::isSphereInside( MPBVector center, MPBFLOAT radius ) const
{
	return !isSphereOutside( center, radius );
}

// aka in front of
bool Plane::isPointOutside( MPBVector point ) const
{
	return (distanceToPoint( point ) > 0);
}

// aka in back of
bool Plane::isPointInside( MPBVector point ) const
{
	return !isPointOutside( point );
}

// returns true if ALL points are inside
bool Plane::arePointsInside( MPBVector* pointList, int numPoints ) const
{
	for (int point=0; point<numPoints; point++)
	{
		if (isPointOutside( pointList[point] )) return false;
	}

	return true;
}

// returns the y-value for a given x,z
MPBFLOAT Plane::getYAtPoint( MPBFLOAT x, MPBFLOAT z ) const
{
	MPBFLOAT y;

	if (m_normal.y == 0) 
	{
		y = m_LL.y;
	}
	else
	{
		y =	(	-(	m_normal.x * (x - m_LL.x) + 
						m_normal.z * (z - m_LL.z)	)
				) /
				m_normal.y +
				m_LL.y;
	}

	return y;
}

// tests if line intersects plane and is within corners
bool Plane::lineInsideCorners(	const MPBVector& start, 
											const MPBVector& end, 
											MPBVector& intersection ) const
{
	if (!lineIntersect( start, end, intersection )) return false;

	/////////////////////////////////////////////////////
	// check if intersection is inside quad by adding up angles
	// from corners to intersection point: 360 = inside quad

	bool intersectionIsInPoly = false;

	if (intersection == getLL()) intersectionIsInPoly = true;
	else if (intersection == getLR()) intersectionIsInPoly = true;
	else if (intersection == getUR()) intersectionIsInPoly = true;
	else if (intersection == getUL()) intersectionIsInPoly = true;
	else
	{
		MPBVector PIPToLL = getLL() - intersection;
		MPBVector PIPToLR = getLR() - intersection;
		MPBVector PIPToUR = getUR() - intersection;
		MPBVector PIPToUL = getUL() - intersection;

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

		MPBFLOAT angle_LL_LR = (float)acos( dp_LL_LR );
		MPBFLOAT angle_LR_UR = (float)acos( dp_LR_UR );
		MPBFLOAT angle_UR_UL = (float)acos( dp_UR_UL );
		MPBFLOAT angle_UL_LL = (float)acos( dp_UL_LL );

		MPBFLOAT dAngle = angle_LL_LR + angle_LR_UR + angle_UR_UL + angle_UL_LL;

		if (floatEQ( dAngle, _2PI ))
		{
			// angles add up to 360 (2*PI); intersection point is in the quad.
			intersectionIsInPoly = true;
		}
	}


	return intersectionIsInPoly;
}

// returns false only if line is parallel to plane or line is 0 length
// does not test if line is within corners
bool Plane::lineIntersect(	const MPBVector& start, 
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

// returns false if line is parallel to plane or line is 0 length
// or if endpoints don't contain the plane
// does not test if line is within corners
bool Plane::rayIntersect(	const MPBVector& start, 
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
bool Plane::AABBIntersect( const MPBAABB& aabb ) const
{
	// first, check if any corner is in the box

//	if (collisionPointAABB( m_LL, aabb )) return true;
//	if (collisionPointAABB( m_LR, aabb )) return true;
//	if (collisionPointAABB( m_UR, aabb )) return true;
//	if (collisionPointAABB( m_UL, aabb )) return true;

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

		if (plane->rayIntersect( m_LL, (m_LL - m_LR), intersection ))
		{
			// check if intersection is inside quad by adding up angles
			// from corners to intersection point: 360 = inside quad

			MPBVector intersectionToP1 = plane->getLL() - intersection;
			MPBVector intersectionToP2 = plane->getLR() - intersection;
			MPBVector intersectionToP3 = plane->getUR() - intersection;
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

void Plane::translate( MPBFLOAT x, MPBFLOAT y, MPBFLOAT z )
{
	m_LL.translate( x, y, z );
	m_LR.translate( x, y, z );
	m_UR.translate( x, y, z );
	m_UL.translate( x, y, z );

	findEquation();
}

void Plane::rotateX( MPBFLOAT degrees )
{
	m_LL.rotateX( degrees );
	m_LR.rotateX( degrees );
	m_UR.rotateX( degrees );
	m_UL.rotateX( degrees );

	findEquation();
}

void Plane::rotateY( MPBFLOAT degrees )
{
	m_LL.rotateY( degrees );
	m_LR.rotateY( degrees );
	m_UR.rotateY( degrees );
	m_UL.rotateY( degrees );

	findEquation();
}

void Plane::rotateZ( MPBFLOAT degrees )
{
	m_LL.rotateZ( degrees );
	m_LR.rotateZ( degrees );
	m_UR.rotateZ( degrees );
	m_UL.rotateZ( degrees );

	findEquation();
}

void Plane::rotateMatrix( MPBMatrix& m )
{
	m_LL.rotateMatrix( m );
	m_LR.rotateMatrix( m );
	m_UR.rotateMatrix( m );
	m_UL.rotateMatrix( m );

	findEquation();
}

void Plane::scale( MPBFLOAT x, MPBFLOAT y, MPBFLOAT z )
{
	m_LL.scale( x, y, z );
	m_LR.scale( x, y, z );
	m_UR.scale( x, y, z );
	m_UL.scale( x, y, z );

	findEquation();
}

void Plane::draw2D( HDC hdc )
{
	POINT p;

	MoveToEx( hdc, (int)(m_LL.x * DRAW_SCALE), (int)(m_LL.y * DRAW_SCALE), &p );
	LineTo( hdc, (int)(m_LR.x * DRAW_SCALE), (int)(m_LR.y * DRAW_SCALE) );
}

void Plane::draw()
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
	glTexCoord2d( 0, 0 ); glVertex3d( m_LL.x, m_LL.y, m_LL.z );
	glTexCoord2d( 1, 0 ); glVertex3d( m_LR.x, m_LR.y, m_LR.z );
	glTexCoord2d( 1, 1 ); glVertex3d( m_UR.x, m_UR.y, m_UR.z );
	glTexCoord2d( 0, 1 ); glVertex3d( m_UL.x, m_UL.y, m_UL.z );
	glEnd();

	if (m_polygonOffsetFactor || m_polygonOffsetUnits)
		glDisable( GL_POLYGON_OFFSET_FILL );

	if (MPBOpenGL::is2D()) 
	{
		glPopAttrib();
	}
}

void Plane::drawVertices() const
{
	glNormal3f( m_normal.x, m_normal.y, m_normal.z );

	if (m_polygonOffsetFactor || m_polygonOffsetUnits)
	{
		glEnable( GL_POLYGON_OFFSET_FILL );
		glPolygonOffset( m_polygonOffsetFactor, m_polygonOffsetUnits );
	}

	glBegin( GL_QUADS );
	glVertex3d( m_LL.x, m_LL.y, m_LL.z );
	glVertex3d( m_LR.x, m_LR.y, m_LR.z );
	glVertex3d( m_UR.x, m_UR.y, m_UR.z );
	glVertex3d( m_UL.x, m_UL.y, m_UL.z );
	glEnd();

	if (m_polygonOffsetFactor || m_polygonOffsetUnits)
		glDisable( GL_POLYGON_OFFSET_FILL );
}

void Plane::drawVertices2() const
{
	if (m_polygonOffsetFactor || m_polygonOffsetUnits)
	{
		glEnable( GL_POLYGON_OFFSET_FILL );
		glPolygonOffset( m_polygonOffsetFactor, m_polygonOffsetUnits );
	}

	glVertex3d( m_LL.x, m_LL.y, m_LL.z );
	glVertex3d( m_LR.x, m_LR.y, m_LR.z );
	glVertex3d( m_UR.x, m_UR.y, m_UR.z );
	glVertex3d( m_UL.x, m_UL.y, m_UL.z );

	if (m_polygonOffsetFactor || m_polygonOffsetUnits)
		glDisable( GL_POLYGON_OFFSET_FILL );
}

void Plane::setPolygonOffset( GLfloat factor, GLfloat units )
{
	m_polygonOffsetFactor = factor;
	m_polygonOffsetUnits = units;
}

void Plane::getTriangles( MPBTriangle& t1, MPBTriangle& t2 )
{
	t1.setCorners( m_LL, m_LR, m_UL );
	t2.setCorners( m_UR, m_UL, m_LR );
}


/////////////////////////////////////////////////////////////////

void Plane::test()
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




