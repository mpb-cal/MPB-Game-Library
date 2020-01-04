
#include "MPBLibrary.h"
#include "MPBGlobal.h"
#include "MPBQuaternion.h"
#include <math.h>
#include "mmgr.h"


MPBQuaternion::MPBQuaternion():
	n( 1 )
{
}

MPBQuaternion::MPBQuaternion( const MPBQuaternion& q ):
	v( q.v ),
	n( q.n )
{
}

MPBQuaternion::MPBQuaternion( MPBFLOAT pn, const MPBVector& pv ):
	v( pv ),
	n( pn )
{
}

MPBQuaternion::MPBQuaternion( MPBFLOAT pn, MPBFLOAT vx, MPBFLOAT vy, MPBFLOAT vz ):
	v( vx, vy, vz ),
	n( pn )
{
}

const MPBQuaternion& MPBQuaternion::operator=( const MPBQuaternion& q )
{
	n = q.n;
	v = q.v;

	return *this;
}

// combine 2 rotations
// q(ph) = (qp)h
// qp != pq
MPBQuaternion MPBQuaternion::operator*( const MPBQuaternion& p ) const
{
	MPBQuaternion result(	n * p.n - v.x * p.v.x - v.y * p.v.y - v.z * p.v.z,
									n * p.v.x + v.x * p.n + v.y * p.v.z - v.z * p.v.y,
									n * p.v.y + v.y * p.n + v.z * p.v.x - v.x * p.v.z,
									n * p.v.z + v.z * p.n + v.x * p.v.y - v.y * p.v.x	);

	return result;
}

// used by rotateVector() and when calculating angular velocity
MPBQuaternion MPBQuaternion::operator*( const MPBVector& u ) const
{
	MPBQuaternion q( 0, u.x, u.y, u.z );
	MPBQuaternion r = *this * q;

	return r;
}

MPBQuaternion MPBQuaternion::operator*( MPBFLOAT s ) const
{
	MPBQuaternion result(	n * s,
									v.x * s,
									v.y * s,
									v.z * s	);

	return result;
}

MPBQuaternion MPBQuaternion::operator/( MPBFLOAT s ) const
{
	MPBQuaternion result(	n / s,
									v.x / s,
									v.y / s,
									v.z / s	);

	return result;
}

MPBQuaternion MPBQuaternion::operator+( const MPBQuaternion& p ) const
{
	MPBQuaternion result(	n + p.n,
									v.x + p.v.x,
									v.y + p.v.y,
									v.z + p.v.z	);

	return result;
}

MPBQuaternion MPBQuaternion::operator~() const
{
	return MPBQuaternion( n, -v.x, -v.y, -v.z );
}

MPBFLOAT MPBQuaternion::getMagnitude() const
{
	return sqrt( pow( n, 2 ) + pow( v.x, 2 ) + pow( v.y, 2 ) + pow( v.z, 2 ) );
}

void MPBQuaternion::normalize()
{
	MPBFLOAT magnitude = getMagnitude();
	MPB_ASSERT( magnitude );
	if (magnitude) *this = *this / magnitude;
	MPB_ASSERT( floatEQ( getMagnitude(), 1 ) );
}

void MPBQuaternion::toAxisAngle( MPBVector& axis, MPBFLOAT& angle ) const
{
	angle = 2 * acos( n );
//	MPBFLOAT scale = v.x * v.x + v.y * v.y + v.z * v.z;
	MPBFLOAT scale = sqrt( 1.0 - n * n );

	if (floatEQ( scale, 0 ))
	{
		scale = 1;
//		angle = 0;
//		axis.set( 0, 0, -1 );
	}
//	else
//	{
		axis.set( v.x / scale, v.y / scale, v.z / scale );
//	}
}

// axis must be normalized
void MPBQuaternion::fromAxisAngle( const MPBVector& axis, MPBFLOAT radians )
{
//	MPB_ASSERT( floatEQ( axis.getLength(), 1 ) );
	MPBFLOAT rOver2 = radians / 2;
	MPBFLOAT sine = sin( rOver2 );
	n = cos( rOver2 );
	v.set( axis.x * sine, axis.y * sine, axis.z * sine );
	normalize();
}

// match rotation of vector
// vector must be normalized
void MPBQuaternion::fromVector( const MPBVector& v )
{
	MPBVector forward( 0, 0, -1 );

	MPBVector axis;
	MPBFLOAT angle;

	if (v == forward || v == forward * -1)
	{
		axis = v;
		angle = 0;
	}
	else
	{
		axis = v.crossProduct( forward );
		angle = -acos( v.dotProduct( forward ) );
	}

	fromAxisAngle( axis.normalized(), angle );
}

// match rotation of vector perpendicular to v1 and v2
// vectors must be normalized
void MPBQuaternion::fromVectors( const MPBVector& v1, const MPBVector& v2 )
{
	fromVector( v1.crossProduct( v2 ).normalized() );
}

void MPBQuaternion::fromEuler( MPBVector radians )
{
	fromEuler( radians.x, radians.y, radians.z );
}

void MPBQuaternion::fromEuler( MPBFLOAT xRadians, MPBFLOAT yRadians, MPBFLOAT zRadians )
{
	if (1)
	{
		MPBQuaternion qx( cos( xRadians/2.0 ), sin( xRadians/2.0 ), 0, 0 );
		MPBQuaternion qy( cos( yRadians/2.0 ), 0, sin( yRadians/2.0 ), 0 );
		MPBQuaternion qz( cos( zRadians/2.0 ), 0, 0, sin( zRadians/2.0 ) );

		*this = qz * qy * qx;
	}

	if (0)
	{
		MPBFLOAT roll = xRadians;
		MPBFLOAT pitch = yRadians;
		MPBFLOAT yaw = zRadians;

		MPBFLOAT croll = cos( .5 * roll );
		MPBFLOAT cpitch = cos( .5 * pitch );
		MPBFLOAT cyaw = cos( .5 * yaw );

		MPBFLOAT sroll = sin( .5 * roll );
		MPBFLOAT spitch = sin( .5 * pitch );
		MPBFLOAT syaw = sin( .5 * yaw );

		MPBFLOAT cyawcpitch = cyaw * cpitch;
		MPBFLOAT syawspitch = syaw * spitch;
		MPBFLOAT cyawspitch = cyaw * spitch;
		MPBFLOAT syawcpitch = syaw * cpitch;

		n = cyawcpitch * croll + syawspitch * sroll;
		v.x = cyawcpitch * sroll + syawspitch * croll;
		v.y = cyawspitch * croll + syawcpitch * sroll;
		v.z = syawcpitch * croll + cyawspitch * sroll;
	}

/*
MPBFLOAT c1 = Math.cos(heading/2);
MPBFLOAT s1 = Math.sin(heading/2);
MPBFLOAT c2 = Math.cos(attitude/2);
MPBFLOAT s2 = Math.sin(attitude/2);
MPBFLOAT c3 = Math.cos(bank/2);
MPBFLOAT s3 = Math.sin(bank/2);
MPBFLOAT c1c2 = c1*c2;
MPBFLOAT s1s2 = s1*s2;
w =c1c2*c3 + s1s2*s3;
x =c1c2*s3 - s1s2*c3;
y =c1*s2*c3 + s1*c2*s3;
z =s1*c2*c3 - c1*s2*s3;
*/
}

/*
void MPBQuaternion::toMatrix( MPBFLOAT matrix[16] ) const
{
	MPBFLOAT matrixd[16];
	toMatrix( matrixd );
	for (int i=0; i<16; i++) matrix[i] = matrixd[i];
}
*/

void MPBQuaternion::toMatrix( MPBMatrix& matrix ) const
{
	MPBFLOAT m[16];
	matrix.toOpenGLArray( m );
	toMatrix( m );
	matrix.fromOpenGLArray( m );
}

void MPBQuaternion::toMatrix( MPBFLOAT matrix[16] ) const
{
	MPBFLOAT x = v.x;
	MPBFLOAT y = v.y;
	MPBFLOAT z = v.z;

	MPBFLOAT ns = n * n;
	MPBFLOAT xs = x * x;
	MPBFLOAT ys = y * y;
	MPBFLOAT zs = z * z;

	// read down

	// first column
	matrix[0] = ns + xs - ys - zs;
	matrix[1] = 2.0*x*y + 2.0*z*n;
	matrix[2] = 2.0*z*x - 2.0*y*n;
	matrix[3] = 0;

	matrix[4] = 2.0*x*y - 2.0*z*n;
	matrix[5] = ns - xs + ys - zs;
	matrix[6] = 2.0*z*y + 2.0*x*n;
	matrix[7] = 0;

	matrix[8] = 2.0*x*z + 2.0*y*n;
	matrix[9] = 2.0*y*z - 2.0*x*n;
	matrix[10] = ns - xs - ys + zs;
	matrix[11] = 0;

	matrix[12] = 0;
	matrix[13] = 0;
	matrix[14] = 0;
	matrix[15] = 1;

/*
	matrix[0] = n*n + v.z*v.z - v.x*v.x - v.y*v.y;
	matrix[1] = 2*v.z*v.x + 2*v.y*n;
	matrix[2] = 2*v.y*v.z - 2*v.x*n;
	matrix[3] = 0;

	matrix[4] = 2*v.z*v.x - 2*v.y*n;
	matrix[5] = n*n - v.z*v.z + v.x*v.x - v.y*v.y;
	matrix[6] = 2*v.y*v.x + 2*v.z*n;
	matrix[7] = 0;

	matrix[8] = 2*v.z*v.y + 2*v.x*n;
	matrix[9] = 2*v.x*v.y - 2*v.z*n;
	matrix[10] = n*n - v.z*v.z - v.x*v.x + v.y*v.y;
	matrix[11] = 0;

	matrix[12] = 0;
	matrix[13] = 0;
	matrix[14] = 0;
	matrix[15] = 1;
*/
}

void MPBQuaternion::toEuler( MPBFLOAT& xRadians, MPBFLOAT& yRadians, MPBFLOAT& zRadians ) const
{
	MPBFLOAT matrix[16];
	toMatrix( matrix );

	xRadians = atan2( matrix[6], matrix[10] );
	yRadians = asin( -matrix[2] );
	zRadians = atan2( matrix[1] , matrix[0] );

/*
	MPBFLOAT q00 = n * n;
	MPBFLOAT q11 = v.x * v.x;
	MPBFLOAT q22 = v.y * v.y;
	MPBFLOAT q33 = v.z * v.z;

	MPBFLOAT r11 = q00 + q11 - q22 - q33;
	MPBFLOAT r21 = 2 * (v.x * v.y + n * v.z);
	MPBFLOAT r31 = 2 * (v.x * v.z - n * v.y);
	MPBFLOAT r32 = 2 * (v.y * v.z + n * v.x);
	MPBFLOAT r33 = q00 - q11 - q22 + q33;

	MPBFLOAT tmp = fabs( r31 );
	if (tmp > 0.999999)
	{
		MPBFLOAT r12 = 2 * (v.x * v.y - n * v.z);
		MPBFLOAT r13 = 2 * (v.x * v.z + n * v.x);

		xRadians = 0;
		yRadians = -PI_OVER_2 * r31 / tmp;
		zRadians = atan2( -r12, -r31 * r13 );
	}
	else
	{
		xRadians = atan2( r32, r33 );
		yRadians = asin( -r31 );
		zRadians = atan2( r21, r11 );
	}
*/
}

void MPBQuaternion::rotateVector( MPBVector& u ) const
{
	MPBQuaternion conj( n, -v.x, -v.y, -v.z );
	MPBQuaternion q = *this * u * conj;
	u = q.v;
}


/////////////////////////////////////////////////////////////////////////

class MPBQuaternionTestClass
{
public:
	MPBQuaternionTestClass()
	{
		MPBQuaternion q;
		MPBVector v( 0, 0, -1 );

		MPBFLOAT xRot = PI / 4;
		MPBFLOAT yRot = 0;
		MPBFLOAT zRot = 0;
		q.fromEuler( xRot, yRot, zRot );
		v.set( 0, 0, -1 );
		q.rotateVector( v );
		MPB_ASSERT( floatEQ( v.x, 0 ) );
		MPB_ASSERT( floatEQ( v.y, .707107 ) );
		MPB_ASSERT( floatEQ( v.z, -.707107 ) );

		MPBFLOAT xRadians, yRadians, zRadians;
		q.toEuler( xRadians, yRadians, zRadians );
		MPB_ASSERT( floatEQ( xRadians, xRot ) || floatEQ( floatABS( xRadians ) - PI, xRot ) );
		MPB_ASSERT( floatEQ( yRadians, yRot ) || floatEQ( floatABS( yRadians ) - PI, yRot ) );
		MPB_ASSERT( floatEQ( zRadians, zRot ) || floatEQ( floatABS( zRadians ) - PI, zRot ) );



		testToEuler( PI/4, 0, 0, 0, .707107, -.707107, true );
		testToEuler( PI/2, 0, 0, 0, 1, 0, true );
		testToEuler( 3*PI/4, 0, 0, 0, .707107, .707107, true );

		testToEuler( 0, 0, PI/4, 0, 0, -1, true );
		testToEuler( 0, 0, PI/2, 0, 0, -1, true );
		testToEuler( 0, 0, 3*PI/4, 0, 0, -1, true );

		testToEuler( 0, PI/4, 0, -.707107, 0, -.707107, true );
		testToEuler( 0, -PI/4, 0, .707107, 0, -.707107, true );
		testToEuler( 0, PI/3, 0, -.866025, 0, -.5, true );
		testToEuler( 0, -PI/3, 0, .866025, 0, -.5, true );
		testToEuler( 0, PI/2, 0, -1, 0, 0, true );
		testToEuler( 0, -PI/2, 0, 1, 0, 0, true );
//		testToEuler( 0, PI/2 + .1, 0, 0, 0, 0, false );
//		testToEuler( 0, 3*PI/4, 0, -.707107, 0, .707107, true );
		testToEuler( 0, -3*PI/4, 0, .707107, 0, .707107, true );



		MPBVector u( -.25, .5, -1 );
		u.normalize();
		q.fromAxisAngle( u, PI/8 );
		MPBVector axis;
		MPBFLOAT angle;
		q.toAxisAngle( axis, angle );
		MPB_ASSERT( floatEQ( axis.x, u.x ) );
		MPB_ASSERT( floatEQ( axis.y, u.y ) );
		MPB_ASSERT( floatEQ( axis.z, u.z ) );
		MPB_ASSERT( floatEQ( angle, PI/8 ) );
	}

private:

	void testToEuler(	MPBFLOAT xRot, 
							MPBFLOAT yRot, 
							MPBFLOAT zRot, 
							MPBFLOAT vectorX, 
							MPBFLOAT vectorY, 
							MPBFLOAT vectorZ, 
							bool checkVector	)
	{
		MPBQuaternion q;
		MPBVector v( 0, 0, -1 );

		q.fromEuler( xRot, yRot, zRot );

		if (checkVector)
		{
			q.rotateVector( v );
			MPB_ASSERT( floatEQ( v.x, vectorX ) );
			MPB_ASSERT( floatEQ( v.y, vectorY ) );
			MPB_ASSERT( floatEQ( v.z, vectorZ ) );
		}

		MPBFLOAT xRadians, yRadians, zRadians;
		q.toEuler( xRadians, yRadians, zRadians );
		MPB_ASSERT( floatEQ( xRadians, xRot ) || floatEQ( floatABS( xRadians ) - PI, xRot ) );
		MPB_ASSERT( floatEQ( yRadians, yRot ) || floatEQ( floatABS( yRadians ) - PI, yRot ) );
		MPB_ASSERT( floatEQ( zRadians, zRot ) || floatEQ( floatABS( zRadians ) - PI, zRot ) );
	}
};

static MPBQuaternionTestClass s_tc;

/////////////////////////////////////////////////////////////////////////


