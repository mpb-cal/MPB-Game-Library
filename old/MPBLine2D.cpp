
#include "MPBGlobal.h"
#include "MPBLine2D.h"
#include "MPBCollision.h"
#include "MPBOpenGL.h"
#include "mmgr.h"


MPBLine2D::MPBLine2D()
{
}						

MPBLine2D::MPBLine2D(	const MPBVector2D& v1,
								const MPBVector2D& v2	)
{
	setPoints( v1, v2 );
}						

MPBLine2D::~MPBLine2D()
{
}

void MPBLine2D::setPoints(	const MPBVector2D& v1,
									const MPBVector2D& v2	)
{
	m_v1 = v1;
	m_v2 = v2;
	m_line = v2 - v1;
	m_normal = m_line;
	m_normal.rotate( 90 );
	m_normal.normalize();
}						

bool MPBLine2D::pointInside( const MPBVector2D& point ) const
{
//	MPBVector2D v = point - m_v1;
//	v.normalize();
//	return (m_normal.dotProduct( v ) < 0);

	return (distanceToPoint( point ) < 0);
}

MPBFLOAT MPBLine2D::distanceToPoint( const MPBVector2D& point ) const
{
	MPBVector2D v = point - m_v1;
	return v.dotProduct( m_normal );
}

bool MPBLine2D::intersectLine( const MPBLine2D& line, MPBVector2D& intersection ) const
{
	MPBFLOAT Ax = line.m_v1.x;
	MPBFLOAT Ay = line.m_v1.y;
	MPBFLOAT Bx = line.m_v2.x;
	MPBFLOAT By = line.m_v2.y;
	MPBFLOAT Cx = m_v1.x;
	MPBFLOAT Cy = m_v1.y;
	MPBFLOAT Dx = m_v2.x;
	MPBFLOAT Dy = m_v2.y;

	MPBFLOAT denom = (Bx-Ax) * (Dy-Cy) - (By-Ay) * (Dx-Cx);
	MPBFLOAT numer_r = (Ay-Cy) * (Dx-Cx) - (Ax-Cx) * (Dy-Cy);
	MPBFLOAT numer_s = (Bx-Ax) * (Dy-Cy) - (By-Ay) * (Dx-Cx);

	if (denom == 0) return false;		// lines are parallel

	MPBFLOAT r = numer_r / denom;
	MPBFLOAT s = numer_s / denom;

	intersection.x = Ax + r * (Bx - Ax);
	intersection.y = Ay + r * (By - Ay);

	return true;
}





