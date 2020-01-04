
#include "MPBLibrary.h"
#include "MPBGlobal.h"
#include "MPBVector.h"
//#include "MPBVector2D.h"
#include "MPBQuaternion.h"
#include "MPBMatrix.h"
#include <math.h>
#include "mmgr.h"

#pragma comment(lib, "opengl32.lib")

MPBVector::MPBVector() :
	x( 0 ),
	y( 0 ),
	z( 0 ),
	m_normalized( false )
{
}

MPBVector::MPBVector( const MPBVector& v ) :
	x( v.x ),
	y( v.y ),
	z( v.z ),
	m_normalized( false )
{
}

/*
MPBVector::MPBVector( const MPBVector2D& v ) :
	x( v.x ),
	y( v.y ),
	z( 0 ),
	m_normalized( false )
{
}
*/

MPBVector::MPBVector( MPBFLOAT px, MPBFLOAT py, MPBFLOAT pz ) :
	x( px ),
	y( py ),
	z( pz ),
	m_normalized( false )
{
}

MPBVector::MPBVector( const MPBFLOAT* xyz ) :
	x( xyz[0] ),
	y( xyz[1] ),
	z( xyz[2] ),
	m_normalized( false )
{
}

const MPBVector& MPBVector::operator=( const MPBVector& v )
{
	x = v.x;
	y = v.y;
	z = v.z;

	xn = v.xn;
	yn = v.yn;
	zn = v.zn;

	m_normalized = v.m_normalized;

	return *this;
}
/*
const MPBVector& MPBVector::operator=( const MPBVector2D& v )
{
	x = v.x;
	y = v.y;
	z = 0;

	xn = v.xn;
	yn = v.yn;
	zn = 0;

	m_normalized = v.m_normalized;

	return *this;
}
*/

// doesn't change x,y,z but stores normalized version in xn,yn,zn
MPBVector MPBVector::normalized()
{
	if (m_normalized)
	{
		return MPBVector( xn, yn, zn );
	}

	MPBVector n = *this;
	n.normalize();
	this->xn = n.x;
	this->yn = n.y;
	this->zn = n.z;
	m_normalized = true;
	return n;
}

void MPBVector::set( MPBFLOAT px, MPBFLOAT py, MPBFLOAT pz )
{
	x = px;
	y = py;
	z = pz;
}

bool MPBVector::operator==( const MPBVector& v ) const
{
	if (
		x == v.x &&
		y == v.y &&
		z == v.z
	)
	{
		return true;
	}

	return false;
}

bool MPBVector::operator!=( const MPBVector& v ) const
{
	return !(*this == v);
}

MPBFLOAT MPBVector::dotProduct( const MPBVector& v ) const
{
//	MPB_ASSERT( getLength() != 0 );
//	MPB_ASSERT( v.getLength() != 0 );

	return	x * v.x + 
				y * v.y + 
				z * v.z;
}

MPBVector MPBVector::crossProduct( const MPBVector& v ) const
{
	MPBVector result(	y * v.z - z * v.y,
							z * v.x - x * v.z,
							x * v.y - y * v.x	);

	return result;
}

MPBVector MPBVector::operator+( const MPBVector& v ) const
{
	MPBVector result;
	result.x = x + v.x;
	result.y = y + v.y;
	result.z = z + v.z;

	return result;
}

MPBVector MPBVector::operator-( const MPBVector& v ) const
{
	MPBVector result;
	result.x = x - v.x;
	result.y = y - v.y;
	result.z = z - v.z;

	return result;
}

MPBVector MPBVector::operator*( const MPBFLOAT v ) const
{
	MPBVector result;
	result.x = x * v;
	result.y = y * v;
	result.z = z * v;

	return result;
}

MPBVector MPBVector::operator/( const MPBFLOAT v ) const
{
//	if (v == 0) return *this;
	MPB_ASSERT( v != 0 );

	MPBVector result;
	result.x = x / v;
	result.y = y / v;
	result.z = z / v;

	return result;
}

MPBVector MPBVector::operator*( const MPBQuaternion& q ) const
{
	MPBQuaternion p( 0, x, y, z );
	MPBQuaternion r = p * q;

	return r.v;
}

MPBVector MPBVector::operator*( const MPBMatrix& m ) const
{
	MPBVector result;
	result.x = m.m[0] * x + m.m[1] * y + m.m[2] * z + m.m[3] * 1;
	result.y = m.m[4] * x + m.m[5] * y + m.m[6] * z + m.m[7] * 1;
	result.z = m.m[8] * x + m.m[9] * y + m.m[10] * z + m.m[11] * 1;
//	result.w = m.m[3] * x + m.m[7] * y + m.m[11] * z + m.m[15] * 1;

	return result;
}

MPBFLOAT MPBVector::getLength() const
{
	MPBFLOAT l = (float)sqrt( pow( x, 2 ) + pow( y, 2 ) + pow( z, 2 ) );
	MPB_ASSERT( l >= 0 );
	return l;
}

void MPBVector::setLength( MPBFLOAT length )
{
	normalize();
	*this = *this * length;
}

void MPBVector::normalize()
{
	MPBFLOAT length = getLength();
	MPB_ASSERT( length != 0 );
	*this = *this / length;
}

void MPBVector::move( const MPBVector& direction, MPBFLOAT distance )
{
	x += direction.x * distance;
	y += direction.y * distance;
	z += direction.z * distance;
}

void MPBVector::translate( MPBFLOAT px, MPBFLOAT py, MPBFLOAT pz )
{
	x += px;
	y += py;
	z += pz;
}

void MPBVector::scale( MPBFLOAT px, MPBFLOAT py, MPBFLOAT pz )
{
	x *= px;
	y *= py;
	z *= pz;
}

void MPBVector::rotateXRadians( MPBFLOAT radians )
{
	MPBFLOAT cosine = (float)cos( radians );
	MPBFLOAT sine = (float)sin( radians );

	MPBVector result;
	result.x = x;
	result.y = y * cosine - z * sine;
	result.z = y * sine + z * cosine;
	*this = result;
}

void MPBVector::rotateX( MPBFLOAT degrees )
{
	rotateXRadians( degreesToRadians( degrees ) );
}

void MPBVector::rotateYRadians( MPBFLOAT radians )
{
	MPBFLOAT cosine = (float)cos( radians );
	MPBFLOAT sine = (float)sin( radians );

	MPBVector result;
	result.x = x * cosine + sine * z;
	result.y = y;
	result.z = x * -sine + z * cosine;
	*this = result;
}

void MPBVector::rotateY( MPBFLOAT degrees )
{
	rotateYRadians( degreesToRadians( degrees ) );
}

void MPBVector::rotateZRadians( MPBFLOAT radians )
{
	MPBFLOAT cosine = (float)cos( radians );
	MPBFLOAT sine = (float)sin( radians );

	MPBVector result;
	result.x = x * cosine - y * sine;
	result.y = x * sine + y * cosine;
	result.z = z;
	*this = result;
}

void MPBVector::rotateZ( MPBFLOAT degrees )
{
	rotateZRadians( degreesToRadians( degrees ) );
}

void MPBVector::doOpenGLRotation()
{
	MPBQuaternion q;
	q.fromVector( normalized() );
	MPBFLOAT matrix[16];
	q.toMatrix( matrix );
	glMultMatrixf( matrix );
}

// makes a rotation from this vector (pointing up) and a vector pointing right
void MPBVector::doOpenGLRotation( MPBVector right )
{
	MPBQuaternion q;
	q.fromVectors( normalized(), right.normalized() );
	MPBFLOAT matrix[16];
	q.toMatrix( matrix );
	glMultMatrixf( matrix );
}

// returns radians between -PI and PI
MPBFLOAT MPBVector::getXRotation()
{
//	MPB_ASSERT( 0 );
	MPBFLOAT length = (float)sqrt( pow( y, 2 ) + pow( z, 2 ) );
	if (length == 0) return 0;
	MPBFLOAT cosine = -z / length;
	MPB_ASSERT( cosine <= 1 );
	MPB_ASSERT( cosine >= -1 );
	MPBFLOAT radians = (float)acos( cosine );
	if (y < 0) radians = -radians;
//	radians = radians - PI_OVER_2;
	return radians;
}

// returns radians between -PI and PI
MPBFLOAT MPBVector::getYRotation()
{
	MPBFLOAT length = (float)sqrt( pow( x, 2 ) + pow( z, 2 ) );
	if (length == 0) return 0;
	MPBFLOAT cosine = x / length;
	MPB_ASSERT( cosine <= 1 );
	MPB_ASSERT( cosine >= -1 );
	MPBFLOAT radians = (float)acos( cosine );
	if (z > 0) radians = -radians;
	radians = radians - PI_OVER_2;
	return radians;
}

// returns radians between -PI and PI
MPBFLOAT MPBVector::getZRotation()
{
//	MPB_ASSERT( 0 );
	MPBFLOAT length = (float)sqrt( pow( x, 2 ) + pow( y, 2 ) );
	if (length == 0) return 0;
	MPBFLOAT cosine = x / length;
	MPB_ASSERT( cosine <= 1 );
	MPB_ASSERT( cosine >= -1 );
	MPBFLOAT radians = (float)acos( cosine );
	if (y < 0) radians = -radians;
//	radians = radians - PI_OVER_2;
	return radians;
}

void MPBVector::rotateMatrix( MPBMatrix& m )
{
	*this = *this * m;
}

void MPBVector::getMidpoint( const MPBVector& lhs, const MPBVector& rhs )
{
	*this = (lhs + rhs) / 2;
}

void MPBVector::toArray( MPBFLOAT array[3] ) const
{
	array[0] = x;
	array[1] = y;
	array[2] = z;
}

void MPBVector::toString( char* buf ) const
{
	sprintf( buf, "%+12.12f, %+12.12f, %+12.12f", x, y, z );
}

void MPBVector::draw2D( const void* hdc, int xFrom, int yFrom ) const
{
	SelectObject( *(HDC*)hdc, GetStockObject( BLACK_PEN ) );

	POINT p;

	MoveToEx( *(HDC*)hdc, xFrom, yFrom, &p );
	LineTo( *(HDC*)hdc, xFrom + (int)x, yFrom + (int)y );
}

void MPBVector::drawAsVertex() const
{
	glVertex3d( x, y, z );
}


/////////////////////////////////////////////////////////////////////////

void MPBVector::test()
{
	// getYRotation()

	MPBVector testVector( 0, 0, -1 );
	testVector.rotateY( 45 );
	MPBFLOAT rot = radiansToDegrees( testVector.getYRotation() );
	MPB_ASSERT( floatEQ( rot, 45 ) );

	// getXRotation()

	testVector.set( 0, 0, -1 );
	testVector.rotateX( 45 );
	rot = radiansToDegrees( testVector.getXRotation() );
	MPB_ASSERT( floatEQ( rot, 45 ) );

	testVector.set( 0, 0, -1 );
	testVector.rotateX( 135 );
	rot = radiansToDegrees( testVector.getXRotation() );
	MPB_ASSERT( floatEQ( rot, 135 ) );

	testVector.set( 0, 0, -1 );
	testVector.rotateX( -45 );
	rot = radiansToDegrees( testVector.getXRotation() );
	MPB_ASSERT( floatEQ( rot, -45 ) );

	testVector.set( 0, 0, -1 );
	testVector.rotateX( -135 );
	rot = radiansToDegrees( testVector.getXRotation() );
	MPB_ASSERT( floatEQ( rot, -135 ) );

	testVector.set( 0, 0, -1 );
	testVector.rotateX( 225 );
	rot = radiansToDegrees( testVector.getXRotation() );
	MPB_ASSERT( floatEQ( rot, -135 ) );

	// getZRotation()

	testVector.set( 1, 0, 0 );
	testVector.rotateZ( 45 );
	rot = radiansToDegrees( testVector.getZRotation() );
	MPB_ASSERT( floatEQ( rot, 45 ) );

	testVector.set( 1, 0, 0 );
	testVector.rotateZ( 135 );
	rot = radiansToDegrees( testVector.getZRotation() );
	MPB_ASSERT( floatEQ( rot, 135 ) );

	testVector.set( 1, 0, 0 );
	testVector.rotateZ( -45 );
	rot = radiansToDegrees( testVector.getZRotation() );
	MPB_ASSERT( floatEQ( rot, -45 ) );

	testVector.set( 1, 0, 0 );
	testVector.rotateZ( -135 );
	rot = radiansToDegrees( testVector.getZRotation() );
	MPB_ASSERT( floatEQ( rot, -135 ) );

	testVector.set( 1, 0, 0 );
	testVector.rotateZ( 225 );
	rot = radiansToDegrees( testVector.getZRotation() );
	MPB_ASSERT( floatEQ( rot, -135 ) );

	// multiple rotations

	testVector.set( -50, 50, -50 );
	rot = radiansToDegrees( testVector.getYRotation() );
	MPB_ASSERT( floatEQ( rot, 45 ) );
	rot = radiansToDegrees( testVector.getXRotation() );
	MPB_ASSERT( floatEQ( rot, 45 ) );

	testVector.set( 50, 50, -50 );
	rot = radiansToDegrees( testVector.getYRotation() );
	MPB_ASSERT( floatEQ( rot, -45 ) );
	rot = radiansToDegrees( testVector.getXRotation() );
	MPB_ASSERT( floatEQ( rot, 45 ) );

	testVector.set( -50, 50, 50 );
	rot = radiansToDegrees( testVector.getYRotation() );
	MPB_ASSERT( floatEQ( rot, -225 ) );
	rot = radiansToDegrees( testVector.getXRotation() );
	MPB_ASSERT( floatEQ( rot, 135 ) );

	testVector.set( 50, 50, 50 );
	rot = radiansToDegrees( testVector.getYRotation() );
	MPB_ASSERT( floatEQ( rot, -135 ) );
	rot = radiansToDegrees( testVector.getXRotation() );
	MPB_ASSERT( floatEQ( rot, 135 ) );
}


