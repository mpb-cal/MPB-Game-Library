
#include "MPBGlobal.h"
#include "MPBLine.h"
#include "MPBCollision.h"
#include "MPBOpenGL.h"
#include "mmgr.h"


MPBLine::MPBLine(	const MPBVector& v1,
						const MPBVector& v2	):
	m_v1( v1 ),
	m_v2( v2 )
{
	m_red = RANDOM_FRACTION;
	m_green = RANDOM_FRACTION;
	m_blue = 1;
}						

MPBLine::~MPBLine()
{
}

void MPBLine::draw()
{
	glPushAttrib( GL_TEXTURE_BIT );

	glDisable( GL_TEXTURE_1D );
	glDisable( GL_TEXTURE_2D );
	glDisable( GL_TEXTURE_3D );
	glLineWidth( 3 );
	glBegin( GL_LINES );

	glColor4f( m_red, m_green, m_blue, 1 );
	glVertex3d( m_v1.x, m_v1.y, m_v1.z );
	glVertex3d( m_v2.x, m_v2.y, m_v2.z );
	glEnd();
	glColor4f( 1, 1, 1, 1 );

	glPopAttrib();
}
