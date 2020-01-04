
#include "MPBGlobal.h"
#include "MPBAABB.h"
#include "MPBList.h"
#include "MPBPlane.h"
#include "mmgr.h"



MPBAABB::MPBAABB() :
	m_extentX( 0 ),
	m_extentY( 0 ),
	m_extentZ( 0 ),
	m_width( 0 ),
	m_height( 0 ),
	m_depth( 0 )
{
}

MPBAABB::MPBAABB( const MPBVector& NLL, const MPBVector& FUR ) :
	m_extentX( 0 ),
	m_extentY( 0 ),
	m_extentZ( 0 ),
	m_width( 0 ),
	m_height( 0 ),
	m_depth( 0 )
{
	setCorners( NLL, FUR );
}

MPBAABB::~MPBAABB()
{
}

const MPBAABB& MPBAABB::operator=( const MPBAABB& rhs )
{
	MPBVector extents( rhs.getExtentX(), rhs.getExtentY(), rhs.getExtentZ() );
	setCenterAndExtents( rhs.getCenter(), extents );

	return *this;
}

void MPBAABB::setCorners( const MPBVector& NLL, const MPBVector& FUR )
{
	MPB_ASSERT( NLL.x <= FUR.x );
	MPB_ASSERT( NLL.y <= FUR.y );
	MPB_ASSERT( NLL.z >= FUR.z );

	m_NLL = NLL;
	m_FUR = FUR;

	m_center = (m_NLL + m_FUR) / 2.0f;
	m_extentX = (float)fabs( m_NLL.x - m_FUR.x ) / 2.0f;
	m_extentY = (float)fabs( m_NLL.y - m_FUR.y ) / 2.0f;
	m_extentZ = (float)fabs( m_NLL.z - m_FUR.z ) / 2.0f;

	m_width = m_extentX * 2;
	m_height = m_extentY * 2;
	m_depth = m_extentZ * 2;

	m_NLR.set( m_NLL.x + m_width,	m_NLL.y,					m_NLL.z );
	m_NUR.set( m_NLL.x + m_width,	m_NLL.y + m_height,	m_NLL.z );
	m_NUL.set( m_NLL.x,				m_NLL.y + m_height,	m_NLL.z );
	
	m_FLL.set( m_NLL.x,				m_NLL.y,					m_FUR.z );
	m_FLR.set( m_NLL.x + m_width,	m_NLL.y,					m_FUR.z );
	m_FUL.set( m_NLL.x,				m_NLL.y + m_height,	m_FUR.z );
}

void MPBAABB::setCenter( const MPBVector& center )
{
	MPBVector extents( m_extentX, m_extentY, m_extentZ );
	setCenterAndExtents( center, extents );
}

void MPBAABB::setCenterAndExtents( const MPBVector& center, const MPBVector& extents )
{
	MPBVector NLL(	center.x - extents.x,
						center.y - extents.y,
						center.z + extents.z	);
	MPBVector FUR(	center.x + extents.x,
						center.y + extents.y,
						center.z - extents.z	);
	setCorners( NLL, FUR );
}

void MPBAABB::bound( const MPBAABB& box1, const MPBAABB& box2 )
{
	MPBVector NLL;
	MPBVector FUR;

	MPBVector box1NLL = box1.getNLL();
	MPBVector box2NLL = box2.getNLL();

	MPBVector box1FUR = box1.getFUR();
	MPBVector box2FUR = box2.getFUR();

	NLL.x = min( box1NLL.x, box2NLL.x );
	NLL.y = min( box1NLL.y, box2NLL.y );
	NLL.z = max( box1NLL.z, box2NLL.z );

	FUR.x = max( box1FUR.x, box2FUR.x );
	FUR.y = max( box1FUR.y, box2FUR.y );
	FUR.z = min( box1FUR.z, box2FUR.z );

	setCorners( NLL, FUR );
}

void MPBAABB::grow( MPBFLOAT d )
{
	MPB_ASSERT( d != 0 );

	MPBVector extents( m_extentX + d, m_extentY + d, m_extentZ + d );
	setCenterAndExtents( m_center, extents );
}

void MPBAABB::scale( MPBFLOAT x, MPBFLOAT y, MPBFLOAT z )
{
	m_NLL.scale( x, y, z );
	m_FUR.scale( x, y, z );
	setCorners( m_NLL, m_FUR );
}

bool MPBAABB::intersects( const MPBAABB& rhs ) const
{
	MPBVector distance = m_center - rhs.m_center;

	return	fabs( distance.x ) <= m_extentX + rhs.m_extentX &&
				fabs( distance.y ) <= m_extentY + rhs.m_extentY &&
				fabs( distance.z ) <= m_extentZ + rhs.m_extentZ;
}

bool MPBAABB::intersectsSphere( const MPBVector& center, MPBFLOAT radius ) const
{
	MPBFLOAT s, d = 0; 

	if (center.x < m_NLL.x)
	{
		s = center.x - m_NLL.x;
		d += s * s; 
	}
	else if (center.x > m_NLR.x)
	{
		s = center.x - m_NLR.x;
		d += s * s; 
	}

	if (center.y < m_NLL.y)
	{
		s = center.y - m_NLL.y;
		d += s * s; 
	}
	else if (center.y > m_NUL.y)
	{
		s = center.y - m_NUL.y;
		d += s * s; 
	}

	if (center.z < m_FLL.z)
	{
		s = center.z - m_FLL.z;
		d += s * s; 
	}
	else if (center.z > m_NLL.z)
	{
		s = center.z - m_NLL.z;
		d += s * s; 
	}

	return d <= radius * radius;
}

void MPBAABB::draw() const
{
	MPBFLOAT vertices[8][3];
	getVertices( vertices );

	glBegin( GL_QUADS );

	// top
	glNormal3f( 0, 1, 0 );
	glTexCoord2d( 0, 0 );	glVertex3fv( vertices[3] );
	glTexCoord2d( 1, 0 );	glVertex3fv( vertices[2] );
	glTexCoord2d( 1, 1 );	glVertex3fv( vertices[6] );
	glTexCoord2d( 0, 1 );	glVertex3fv( vertices[7] );

	// bottom
	glNormal3f( 0, -1, 0 );
	glTexCoord2d( 0, 0 );	glVertex3fv( vertices[4] );
	glTexCoord2d( 1, 0 );	glVertex3fv( vertices[5] );
	glTexCoord2d( 1, 1 );	glVertex3fv( vertices[1] );
	glTexCoord2d( 0, 1 );	glVertex3fv( vertices[0] );

	// front side
	glNormal3f( 0, 0, 1 );
	glTexCoord2d( 0, 0 );	glVertex3fv( vertices[0] );
	glTexCoord2d( 1, 0 );	glVertex3fv( vertices[1] );
	glTexCoord2d( 1, 1 );	glVertex3fv( vertices[2] );
	glTexCoord2d( 0, 1 );	glVertex3fv( vertices[3] );

	// back side
	glNormal3f( 0, 0, -1 );
	glTexCoord2d( 0, 0 );	glVertex3fv( vertices[5] );
	glTexCoord2d( 1, 0 );	glVertex3fv( vertices[4] );
	glTexCoord2d( 1, 1 );	glVertex3fv( vertices[7] );
	glTexCoord2d( 0, 1 );	glVertex3fv( vertices[6] );

	// left side
	glNormal3f( -1, 0, 0 );
	glTexCoord2d( 0, 0 );	glVertex3fv( vertices[4] );
	glTexCoord2d( 1, 0 );	glVertex3fv( vertices[0] );
	glTexCoord2d( 1, 1 );	glVertex3fv( vertices[3] );
	glTexCoord2d( 0, 1 );	glVertex3fv( vertices[7] );

	// right side
	glNormal3f( 1, 0, 0 );
	glTexCoord2d( 0, 0 );	glVertex3fv( vertices[1] );
	glTexCoord2d( 1, 0 );	glVertex3fv( vertices[5] );
	glTexCoord2d( 1, 1 );	glVertex3fv( vertices[6] );
	glTexCoord2d( 0, 1 );	glVertex3fv( vertices[2] );

	glEnd();
}

void MPBAABB::drawLines() const
{
	MPBFLOAT vertices[8][3];
	getVertices( vertices );

	// top
	glBegin( GL_LINE_LOOP );
	glTexCoord2d( 0, 0 );	glVertex3fv( vertices[3] );
	glTexCoord2d( 1, 0 );	glVertex3fv( vertices[2] );
	glTexCoord2d( 1, 1 );	glVertex3fv( vertices[6] );
	glTexCoord2d( 0, 1 );	glVertex3fv( vertices[7] );
	glEnd();

	// bottom
	glBegin( GL_LINE_LOOP );
	glNormal3f( 0, -1, 0 );
	glTexCoord2d( 0, 0 );	glVertex3fv( vertices[4] );
	glTexCoord2d( 1, 0 );	glVertex3fv( vertices[5] );
	glTexCoord2d( 1, 1 );	glVertex3fv( vertices[1] );
	glTexCoord2d( 0, 1 );	glVertex3fv( vertices[0] );
	glEnd();

	// front side
	glBegin( GL_LINE_LOOP );
	glNormal3f( 0, 0, 1 );
	glTexCoord2d( 0, 0 );	glVertex3fv( vertices[0] );
	glTexCoord2d( 1, 0 );	glVertex3fv( vertices[1] );
	glTexCoord2d( 1, 1 );	glVertex3fv( vertices[2] );
	glTexCoord2d( 0, 1 );	glVertex3fv( vertices[3] );
	glEnd();

	// back side
	glBegin( GL_LINE_LOOP );
	glNormal3f( 0, 0, -1 );
	glTexCoord2d( 0, 0 );	glVertex3fv( vertices[5] );
	glTexCoord2d( 1, 0 );	glVertex3fv( vertices[4] );
	glTexCoord2d( 1, 1 );	glVertex3fv( vertices[7] );
	glTexCoord2d( 0, 1 );	glVertex3fv( vertices[6] );
	glEnd();

	// left side
	glBegin( GL_LINE_LOOP );
	glNormal3f( -1, 0, 0 );
	glTexCoord2d( 0, 0 );	glVertex3fv( vertices[4] );
	glTexCoord2d( 1, 0 );	glVertex3fv( vertices[0] );
	glTexCoord2d( 1, 1 );	glVertex3fv( vertices[3] );
	glTexCoord2d( 0, 1 );	glVertex3fv( vertices[7] );
	glEnd();

	// right side
	glBegin( GL_LINE_LOOP );
	glNormal3f( 1, 0, 0 );
	glTexCoord2d( 0, 0 );	glVertex3fv( vertices[1] );
	glTexCoord2d( 1, 0 );	glVertex3fv( vertices[5] );
	glTexCoord2d( 1, 1 );	glVertex3fv( vertices[6] );
	glTexCoord2d( 0, 1 );	glVertex3fv( vertices[2] );
	glEnd();
}
/*
// true if any of 8 vertices are in view frustum
bool MPBAABB::isInFrustum() const
{
	MPBVector vertices[8];
	vertices[0] = m_NLL;
	vertices[1] = m_NLR;
	vertices[2] = m_NUR;
	vertices[3] = m_NUL;
	vertices[4] = m_FLL;
	vertices[5] = m_FLR;
	vertices[6] = m_FUR;
	vertices[7] = m_FUL;
	return GE::polygonInFrustum( vertices, 8 );
}
*/
void MPBAABB::getVertices( MPBFLOAT vertices[8][3] ) const
{
	m_NLL.toArray( vertices[0] );
	m_NLR.toArray( vertices[1] );
	m_NUR.toArray( vertices[2] );
	m_NUL.toArray( vertices[3] );
	
	m_FLL.toArray( vertices[4] );
	m_FLR.toArray( vertices[5] );
	m_FUR.toArray( vertices[6] );
	m_FUL.toArray( vertices[7] );
}

void MPBAABB::getPlanes( MPBList& planes ) const
{
	// front face
	planes.push_back( new Plane( m_NLL, m_NLR, m_NUR, m_NUL ) );

	// back face
	planes.push_back( new Plane( m_FLR, m_FLL, m_FUL, m_FUR ) );

	// left face
	planes.push_back( new Plane( m_FLL, m_NLL, m_NUL, m_FUL ) );

	// right face
	planes.push_back( new Plane( m_NLR, m_FLR, m_FUR, m_NUR ) );

	// top face
	planes.push_back( new Plane( m_NUL, m_NUR, m_FUR, m_FUL ) );

	// bottom face
	planes.push_back( new Plane( m_FLL, m_FLR, m_NLR, m_NLL ) );
}











