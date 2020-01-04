
#include "MPBLibrary.h"
#include "MPBGlobal.h"
#include "MPBVector2D.h"
#include "MPBQuaternion.h"
#include "MPBMatrix.h"
#include <math.h>
#include "mmgr.h"

#pragma comment(lib, "opengl32.lib")

MPBVector2D::MPBVector2D() :
	x( 0 ),
	y( 0 ),
	m_normalized( false )
{
}

MPBVector2D::MPBVector2D( const MPBVector2D& v ) :
	x( v.x ),
	y( v.y ),
	m_normalized( false )
{
}

MPBVector2D::MPBVector2D( MPBFLOAT px, MPBFLOAT py ) :
	x( px ),
	y( py ),
	m_normalized( false )
{
}

const MPBVector2D& MPBVector2D::operator=( const MPBVector2D& v )
{
	x = v.x;
	y = v.y;

	xn = v.xn;
	yn = v.yn;

	m_normalized = v.m_normalized;

	return *this;
}

// doesn't change x,y,z but stores normalized version in xn,yn,zn
MPBVector2D MPBVector2D::normalized()
{
	if (m_normalized)
	{
		return MPBVector2D( xn, yn );
	}

	MPBVector2D n = *this;
	n.normalize();
	this->xn = n.x;
	this->yn = n.y;
	m_normalized = true;
	return n;
}

void MPBVector2D::set( MPBFLOAT px, MPBFLOAT py )
{
	x = px;
	y = py;
}

bool MPBVector2D::operator==( const MPBVector2D& v ) const
{
	if (
		x == v.x &&
		y == v.y
	)
	{
		return true;
	}

	return false;
}

bool MPBVector2D::operator!=( const MPBVector2D& v ) const
{
	return !(*this == v);
}

MPBFLOAT MPBVector2D::dotProduct( const MPBVector2D& v ) const
{
//	MPB_ASSERT( getLength() != 0 );
//	MPB_ASSERT( v.getLength() != 0 );

	return	x * v.x + 
				y * v.y;
}
/*
MPBVector2D MPBVector2D::crossProduct( const MPBVector2D& v ) const
{
	MPBVector2D result(	y * v.z - z * v.y,
							z * v.x - x * v.z,
							x * v.y - y * v.x	);

	return result;
}
*/

MPBVector2D MPBVector2D::operator+( const MPBVector2D& v ) const
{
	MPBVector2D result;
	result.x = x + v.x;
	result.y = y + v.y;

	return result;
}

MPBVector2D MPBVector2D::operator-( const MPBVector2D& v ) const
{
	MPBVector2D result;
	result.x = x - v.x;
	result.y = y - v.y;

	return result;
}

MPBVector2D MPBVector2D::operator*( const MPBFLOAT v ) const
{
	MPBVector2D result;
	result.x = x * v;
	result.y = y * v;

	return result;
}

MPBVector2D MPBVector2D::operator/( const MPBFLOAT v ) const
{
//	if (v == 0) return *this;
	MPB_ASSERT( v != 0 );

	MPBVector2D result;
	result.x = x / v;
	result.y = y / v;

	return result;
}

MPBFLOAT MPBVector2D::getLength() const
{
	MPBFLOAT l = (float)sqrt( pow( x, 2 ) + pow( y, 2 ) );
	MPB_ASSERT( l >= 0 );
	return l;
}

void MPBVector2D::setLength( MPBFLOAT length )
{
	normalize();
	*this = *this * length;
}

void MPBVector2D::normalize()
{
	MPBFLOAT length = getLength();
	MPB_ASSERT( length != 0 );
	*this = *this / length;
}

void MPBVector2D::move( const MPBVector2D& direction, MPBFLOAT distance )
{
	x += direction.x * distance;
	y += direction.y * distance;
}

void MPBVector2D::translate( MPBFLOAT px, MPBFLOAT py )
{
	x += px;
	y += py;
}

void MPBVector2D::scale( MPBFLOAT px, MPBFLOAT py )
{
	x *= px;
	y *= py;
}

void MPBVector2D::rotateRadians( MPBFLOAT radians )
{
	MPBFLOAT cosine = (float)cos( radians );
	MPBFLOAT sine = (float)sin( radians );

	MPBVector2D result;
	result.x = x * cosine - y * sine;
	result.y = x * sine + y * cosine;
	*this = result;
}

void MPBVector2D::rotate( MPBFLOAT degrees )
{
	rotateRadians( degreesToRadians( degrees ) );
}

// returns radians
MPBFLOAT MPBVector2D::getRotation()
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

void MPBVector2D::getMidpoint( const MPBVector2D& lhs, const MPBVector2D& rhs )
{
	*this = (lhs + rhs) / 2;
}

void MPBVector2D::toArray( MPBFLOAT array[2] ) const
{
	array[0] = x;
	array[1] = y;
}

void MPBVector2D::toString( char* buf ) const
{
	sprintf( buf, "%+12.12f, %+12.12f", x, y );
}



