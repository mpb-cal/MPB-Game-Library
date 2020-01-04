
#include "MPBGlobal.h"
#include "MPBShapeQuad.h"
#include "MPBTexturedPlane.h"
#include "MPBMatrix.h"
#include "MPBOpenGL.h"
#include "mmgr.h"



// starts as an XY plane centered at the origin
MPBShapeQuad::MPBShapeQuad( int width, int height, const char* texture ):
	MPBShape(),
	m_isBillboard( false ),
	m_isXAxisBillboard( false ),
	m_isYAxisBillboard( false ),
	m_isZAxisBillboard( false )
{
	MPBVector LL;
	MPBVector LR;
	MPBVector UR;
	MPBVector UL;

	LL.set( -width/2, -height/2, 0 );
	LR.set(  width/2, -height/2, 0 );
	UR.set(  width/2,  height/2, 0 );
	UL.set( -width/2,  height/2, 0 );

	makePlane( LL, LR, UR, UL, texture );
}

// starts as an XZ plane centered at the origin
// height is IGNORED
MPBShapeQuad::MPBShapeQuad( int width, int height, int depth, const char* texture ):
	MPBShape(),
	m_isBillboard( false ),
	m_isXAxisBillboard( false ),
	m_isYAxisBillboard( false ),
	m_isZAxisBillboard( false )
{
	MPBVector LL;
	MPBVector LR;
	MPBVector UR;
	MPBVector UL;

	LL.set( -width/2, 0,  depth/2 );
	LR.set(  width/2, 0,  depth/2 );
	UR.set(  width/2, 0, -depth/2 );
	UL.set( -width/2, 0, -depth/2 );

	makePlane( LL, LR, UR, UL, texture );
}

MPBShapeQuad::MPBShapeQuad( 	MPBVector& LL,
										MPBVector& LR,
										MPBVector& UR,
										MPBVector& UL, 
										const char* texture ):
	MPBShape(),
	m_isBillboard( false ),
	m_isXAxisBillboard( false ),
	m_isYAxisBillboard( false ),
	m_isZAxisBillboard( false )
{
	makePlane( LL, LR, UR, UL, texture );
}

MPBShapeQuad::MPBShapeQuad( 	MPBVector& LL,
										MPBVector& LR,
										MPBVector& UR,
										const char* texture ):
	MPBShape(),
	m_isBillboard( false ),
	m_isXAxisBillboard( false ),
	m_isYAxisBillboard( false ),
	m_isZAxisBillboard( false )
{
	MPBTexturedPlane* tp = new MPBTexturedPlane( LL, LR, UR );
	tp->addTexture( texture );
	addPlane( tp );
}

MPBShapeQuad::~MPBShapeQuad()
{
}

void MPBShapeQuad::makePlane( MPBVector LL, MPBVector LR, MPBVector UR, MPBVector UL, const char* texture )
{
	MPBTexturedPlane* tp = new MPBTexturedPlane( LL, LR, UR, UL );
	tp->addTexture( texture );
	addPlane( tp );
	setTexSize( 0, 1, 1 );
}


#define RAD(x) (((x)*M_PI)/180.)

void calcMatrix( void )
{
	float m[16];

	glGetFloatv( GL_MODELVIEW_MATRIX, m );

	float theta = -180 * atan2f( m[8], m[10]) / M_PI;
	float x = -1;
	float y = 0;
	float z = 0;

	float d = x*x + y*y + z*z;
	float ct = cosf(RAD(theta)), st = sinf(RAD(theta));

	/* normalize */
	if (d > 0)
	{
		d = 1/d;
		x *= d;
		y *= d;
		z *= d;
	}

	m[ 0] = 1; m[ 1] = 0; m[ 2] = 0; m[ 3] = 0;
	m[ 4] = 0; m[ 5] = 1; m[ 6] = 0; m[ 7] = 0;
	m[ 8] = 0; m[ 9] = 0; m[10] = 1; m[11] = 0;
	m[12] = 0; m[13] = 0; m[14] = 0; m[15] = 1;

	/* R = uu' + cos(theta)*(I-uu') + sin(theta)*S
	*
	* S =  0  -z   y    u' = (x, y, z)
	*	    z   0  -x
	*	   -y   x   0
	*/

	m[0] = x*x + ct*(1-x*x) + st*0;
	m[4] = x*y + ct*(0-x*y) + st*-z;
	m[8] = x*z + ct*(0-x*z) + st*y;

	m[1] = y*x + ct*(0-y*x) + st*z;
	m[5] = y*y + ct*(1-y*y) + st*0;
	m[9] = y*z + ct*(0-y*z) + st*-x;

	m[2] = z*x + ct*(0-z*x) + st*-y;
	m[6] = z*y + ct*(0-z*y) + st*x;
	m[10]= z*z + ct*(1-z*z) + st*0;

	glMultMatrixf( m );
}

void MPBShapeQuad::updateFrame()
{
	MPBShape::updateFrame();

/*	if (m_isBillboard)
	{
		MPBVector look = getAABB().getCenter() - MPBOpenGL::getCameraPos();
		MPBQuaternion q;
		q.fromVector( look.normalized() );
		MPBMatrix m;
		q.toMatrix( m );

		multDrawMatrix( m );
	}
	else if (m_isXAxisBillboard)
	{
		MPB_ASSERT( 0 );
	}
	else if (m_isYAxisBillboard)
	{
		MPB_ASSERT( 0 );
		calcMatrix();
	}
	else if (m_isZAxisBillboard)
	{
		MPB_ASSERT( 0 );
	}
*/
}









