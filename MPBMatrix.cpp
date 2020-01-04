
#include "MPBGlobal.h"
#include "MPBMatrix.h"
#include "mmgr.h"

/*
uses standard order:
0	1	2	3
4	5	6	7
8	9	10	11
12	13	14	15

call toOpenGLArray() to convert to openGL order:
0	4	8	12
1	5	9	13
2	6	10	14
3	7	11	15


the faq uses:
00 01 02 03
10 11 12 13
20 21 22 23
30 31 32 33

the Bourg book uses:
e11 e12 e13
e21 e22 e23
e31 e32 e33

*/


MPBMatrix::MPBMatrix()
{
	loadIdentity();
}

MPBMatrix& MPBMatrix::operator=( const MPBMatrix& rhs )
{
	for (int col=0; col<4; col++)
	{
		for (int row=0; row<4; row++)
		{
			int index = col * 4 + row;
			m[index] = rhs.m[index];
		}
	}

	return *this;
}

MPBMatrix::MPBMatrix( const MPBMatrix& copyFrom )
{
	*this = copyFrom;
}

void MPBMatrix::fromOpenGLArray( MPBFLOAT* array )
{
	m[0] = array[0];
	m[4] = array[1];
	m[8] = array[2];
	m[12] = array[3];
	
	m[1] = array[4];
	m[5] = array[5];
	m[9] = array[6];
	m[13] = array[7];
	
	m[2] = array[8];
	m[6] = array[9];
	m[10] = array[10];
	m[14] = array[11];
	
	m[3] = array[12];
	m[7] = array[13];
	m[11] = array[14];
	m[15] = array[15];
}

void MPBMatrix::toOpenGLArray( MPBFLOAT* array ) const
{
	array[0] = m[0];
	array[1] = m[4];
	array[2] = m[8];
	array[3] = m[12];
	
	array[4] = m[1];
	array[5] = m[5];
	array[6] = m[9];
	array[7] = m[13];
	
	array[8] = m[2];
	array[9] = m[6];
	array[10] = m[10];
	array[11] = m[14];
	
	array[12] = m[3];
	array[13] = m[7];
	array[14] = m[11];
	array[15] = m[15];
}

void MPBMatrix::loadIdentity()
{
	m[0] =  1;
	m[1] =  0;
	m[2] =  0;
	m[3] =  0;

	m[4] =  0;
	m[5] =  1;
	m[6] =  0;
	m[7] =  0;

	m[8] =  0;
	m[9] =  0;
	m[10] = 1;
	m[11] = 0;

	m[12] = 0;
	m[13] = 0;
	m[14] = 0;
	m[15] = 1;
}

void MPBMatrix::translate( MPBVector trans )
{
	MPBMatrix m2;
	m2.m[0] = 1; m2.m[1] = 0; m2.m[2] = 0; m2.m[3] = trans.x;
	m2.m[4] = 0; m2.m[5] = 1; m2.m[6] = 0; m2.m[7] = trans.y;
	m2.m[8] = 0; m2.m[9] = 0; m2.m[10] = 1; m2.m[11] = trans.z;
	m2.m[12] = 0; m2.m[13] = 0; m2.m[14] = 0; m2.m[15] = 1;

	*this = *this * m2;
}

void MPBMatrix::rotateX( MPBFLOAT radians )
{
	MPBMatrix m2;
	MPBFLOAT cosine = (float)cos( radians );
	MPBFLOAT sine = (float)sin( radians );
	m2.m[0] = 1;		m2.m[1] = 0;		m2.m[2] = 0;			m2.m[3] = 0;
	m2.m[4] = 0;		m2.m[5] = cosine;	m2.m[6] = sine;		m2.m[7] = 0;
	m2.m[8] = 0;		m2.m[9] = -sine;	m2.m[10] = cosine;	m2.m[11] = 0;
	m2.m[12] = 0;		m2.m[13] = 0;		m2.m[14] = 0;			m2.m[15] = 1;

	*this = *this * m2;
}

void MPBMatrix::rotateY( MPBFLOAT radians )
{
	MPBMatrix m2;
	MPBFLOAT cosine = (float)cos( radians );
	MPBFLOAT sine = (float)sin( radians );
	m2.m[0] = cosine;	m2.m[1] = 0;		m2.m[2] = sine;		m2.m[3] = 0;
	m2.m[4] = 0;			m2.m[5] = 1;		m2.m[6] = 0;			m2.m[7] = 0;
	m2.m[8] = -sine;	m2.m[9] = 0;		m2.m[10] = cosine;	m2.m[11] = 0;
	m2.m[12] = 0;		m2.m[13] = 0;	m2.m[14] = 0;		m2.m[15] = 1;

	*this = *this * m2;
}

void MPBMatrix::rotateZ( MPBFLOAT radians )
{
	MPBMatrix m2;
	MPBFLOAT cosine = (float)cos( radians );
	MPBFLOAT sine = (float)sin( radians );
	m2.m[0] = cosine;	m2.m[1] = -sine;	m2.m[2] = 0;		m2.m[3] = 0;
	m2.m[4] = sine;		m2.m[5] = cosine;	m2.m[6] = 0;		m2.m[7] = 0;
	m2.m[8] = 0;			m2.m[9] = 0;			m2.m[10] = 1;	m2.m[11] = 0;
	m2.m[12] = 0;		m2.m[13] = 0;		m2.m[14] = 0;	m2.m[15] = 1;

	*this = *this * m2;
}

void MPBMatrix::createTranslation( MPBVector trans )
{
	loadIdentity();
	translate( trans );
}

void MPBMatrix::createRotation( MPBVector right, MPBVector up, MPBVector forward )
{
	right.normalize();
	up.normalize();
	forward.normalize();

//	right = right * -1;
//	up = up * -1;
	forward = forward * -1;

	m[0] = right.x;
	m[1] = up.x;
	m[2] = forward.x;
	m[3] = 0;

	m[4] = right.y;
	m[5] = up.y;
	m[6] = forward.y;
	m[7] = 0;

	m[8] = right.z;
	m[9] = up.z;
	m[10] = forward.z;
	m[11] = 0;

	m[12] = 0;
	m[13] = 0;
	m[14] = 0;
	m[15] = 1;
}

void MPBMatrix::createRotationFromVector( MPBVector v )
{
	MPB_ASSERT( 0 );	// doesn't work

	MPBVector forward( 0, 0, -1 );
	MPBVector axis = v.crossProduct( forward );
	MPBFLOAT angle = (float)acos( v.dotProduct( forward ) );
	fromAxisAngle( axis, angle );
}

void MPBMatrix::fromAxisAngle( MPBVector axis, MPBFLOAT radians )
{
	MPB_ASSERT( 0 );	// doesn't work

	MPBFLOAT rcos = (float)cos( radians );
	MPBFLOAT rsin = (float)sin( radians );

	MPBFLOAT u = axis.x;
	MPBFLOAT v = axis.y;
	MPBFLOAT w = axis.z;

	m[0] =      rcos + u*u*(1-rcos);
	m[1] = -w * rsin + u*v*(1-rcos);
	m[2] =  v * rsin + u*w*(1-rcos);
	m[3] = 0;

	m[4] =  w * rsin + v*u*(1-rcos);
	m[5] =      rcos + v*v*(1-rcos);
	m[6] = -u * rsin + v*w*(1-rcos);
	m[7] = 0;

	m[8] = -v * rsin + w*u*(1-rcos);
	m[9] =  u * rsin + w*v*(1-rcos);
	m[10] =      rcos + w*w*(1-rcos);
	m[11] = 0;

	m[12] = 0;
	m[13] = 0;
	m[14] = 0;
	m[15] = 1;
}

MPBMatrix MPBMatrix::operator*( const MPBMatrix& rhs )
{
	MPBMatrix result;
/*
	for (int col=0; col<4; col++)
	{
		for (int row=0; row<4; row++)
		{
			int resultIndex = col * 4 + row;
			result.v[resultIndex] = 0;
			for (int k=0; k<4; k++)
			{
				result.v[resultIndex] += v[row + k * 4] * rhs.v[col * 4 + k];
			}
		}
	}*/

	result.m[0] = m[0] * rhs.m[0] + m[1] * rhs.m[4] + m[2] * rhs.m[8] + m[3] * rhs.m[12];
	result.m[1] = m[0] * rhs.m[1] + m[1] * rhs.m[5] + m[2] * rhs.m[9] + m[3] * rhs.m[13];
	result.m[2] = m[0] * rhs.m[2] + m[1] * rhs.m[6] + m[2] * rhs.m[10] + m[3] * rhs.m[14];
	result.m[3] = m[0] * rhs.m[3] + m[1] * rhs.m[7] + m[2] * rhs.m[11] + m[3] * rhs.m[15];

	result.m[4] = m[4] * rhs.m[0] + m[5] * rhs.m[4] + m[6] * rhs.m[8] + m[7] * rhs.m[12];
	result.m[5] = m[4] * rhs.m[1] + m[5] * rhs.m[5] + m[6] * rhs.m[9] + m[7] * rhs.m[13];
	result.m[6] = m[4] * rhs.m[2] + m[5] * rhs.m[6] + m[6] * rhs.m[10] + m[7] * rhs.m[14];
	result.m[7] = m[4] * rhs.m[3] + m[5] * rhs.m[7] + m[6] * rhs.m[11] + m[7] * rhs.m[15];

	result.m[8] = m[8] * rhs.m[0] + m[9] * rhs.m[4] + m[10] * rhs.m[8] + m[11] * rhs.m[12];
	result.m[9] = m[8] * rhs.m[1] + m[9] * rhs.m[5] + m[10] * rhs.m[9] + m[11] * rhs.m[13];
	result.m[10] = m[8] * rhs.m[2] + m[9] * rhs.m[6] + m[10] * rhs.m[10] + m[11] * rhs.m[14];
	result.m[11] = m[8] * rhs.m[3] + m[9] * rhs.m[7] + m[10] * rhs.m[11] + m[11] * rhs.m[15];

	result.m[12] = m[12] * rhs.m[0] + m[13] * rhs.m[4] + m[14] * rhs.m[8] + m[15] * rhs.m[12];
	result.m[13] = m[12] * rhs.m[1] + m[13] * rhs.m[5] + m[14] * rhs.m[9] + m[15] * rhs.m[13];
	result.m[14] = m[12] * rhs.m[2] + m[13] * rhs.m[6] + m[14] * rhs.m[10] + m[15] * rhs.m[14];
	result.m[15] = m[12] * rhs.m[3] + m[13] * rhs.m[7] + m[14] * rhs.m[11] + m[15] * rhs.m[15];

	return result;
}

MPBQuaternion MPBMatrix::getQuaternion() const
{
	MPBFLOAT T = 1 + m[0] + m[5] + m[10];

	MPBFLOAT S;
	MPBFLOAT X;
	MPBFLOAT Y;
	MPBFLOAT Z;
	MPBFLOAT W;

	if (T > 0.00000001)
	{
		S = (float)sqrt( T ) * 2;
		X = ( m[9] - m[6] ) / S;
		Y = ( m[2] - m[8] ) / S;
		Z = ( m[4] - m[1] ) / S;
		W = 0.25f * S;
	}
	else
	{
		if ( m[0] > m[5] && m[0] > m[10] )  
		{	// Column 0: 
			S  = (float)sqrt( 1.0 + m[0] - m[5] - m[10] ) * 2;
			X = 0.25f * S;
			Y = (m[4] + m[1] ) / S;
			Z = (m[2] + m[8] ) / S;
			W = (m[9] - m[6] ) / S;
		} 
		else if ( m[5] > m[10] ) 
		{			// Column 1: 
			S  = (float)sqrt( 1.0 + m[5] - m[0] - m[10] ) * 2;
			X = (m[4] + m[1] ) / S;
			Y = 0.25f * S;
			Z = (m[9] + m[6] ) / S;
			W = (m[2] - m[8] ) / S;
		} 
		else 
		{						// Column 2:
			S  = (float)sqrt( 1.0 + m[10] - m[0] - m[5] ) * 2;
			X = (m[2] + m[8] ) / S;
			Y = (m[9] + m[6] ) / S;
			Z = 0.25f * S;
			W = (m[4] - m[1] ) / S;
		}
	}

	return MPBQuaternion( W, X, Y, Z );
}

MPBVector MPBMatrix::getRightAxis() const
{
	return MPBVector( m[0], m[4], m[8] );
}

MPBVector MPBMatrix::getUpAxis() const
{
	return MPBVector( m[1], m[5], m[9] );
}

MPBVector MPBMatrix::getOutAxis() const
{
	return MPBVector( m[2], m[6], m[10] );
}








