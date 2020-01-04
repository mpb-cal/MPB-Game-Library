

#include "MPBLibrary.h"
#include "MPBGeometry.h"
#include "mmgr.h"


// mpb! broken?
// theta should be in radians
MPBVector cylindricalToCartesian( MPBFLOAT rho, MPBFLOAT theta )
{
	MPBVector cartesian;
	cartesian.x = rho * cos( theta );
	cartesian.y = rho * sin( theta );

	return cartesian;
}

// returns theta in radians
void cartesianToCylindrical( MPBFLOAT x, MPBFLOAT y, MPBFLOAT rho, MPBFLOAT& theta )
{
	// mpb!
	theta = acos( x / rho );
//	cos T = x/r
//	sin T = y/r
}

// mpb! broken?
// theta and phi should be in radians
MPBVector sphericalToCartesian( MPBFLOAT rho, MPBFLOAT theta, MPBFLOAT phi )
{
	MPBVector cartesian;
	cartesian.x = rho * cos( phi ) * sin( theta );
	cartesian.y = rho * cos( phi ) * cos( theta );
	cartesian.z = rho * sin( phi );

	return cartesian;
}

// mpb! broken?
// returns theta and phi in radians
void cartesianToSpherical( const MPBVector cartesian, MPBFLOAT& theta, MPBFLOAT& phi )
{
//	MPBFLOAT S = sqrt( cartesian.m_x * cartesian.m_x + cartesian.m_z * cartesian.m_z );
	MPBFLOAT rho = cartesian.getLength();

/*
	if (cartesian.m_x >= 0)
	{
		phi = asin( cartesian.m_z / S );
	}
	else
	{
		phi = cartesian.m_x - asin( cartesian.m_z / S );
	}
*/
	phi = asin( cartesian.y / rho );

	theta = asin( cartesian.z / rho );
	theta += PI_OVER_2;
	if (cartesian.x >= 0) theta = -theta;
}

int roundToInt( MPBFLOAT d )
{
	MPBFLOAT fraction = d - (int)d;
	if (fraction < .5) return (int)d;
	return (int)d + 1;
}

// 4 x 4 matrices in openGL format:
//		 1  5  9 13
//		 2  6 10 14
//		 3  7 11 15
//		 4  8 12 16
void matrixMultiply( MPBFLOAT* a, MPBFLOAT* b, MPBFLOAT* result )
{
	for (int col=0; col<4; col++)
	{
		for (int row=0; row<4; row++)
		{
//			int resultIndex = row * 4 + col;
			int resultIndex = col * 4 + row;
			result[resultIndex] = 0;
			for (int k=0; k<4; k++)
			{
//				result[resultIndex] += a[row * 4 + k] * b[k * 4 + col];
				result[resultIndex] += a[row + k * 4] * b[col * 4 + k];
			}
		}
	}
}

// 4 x 4 matrices
void matrixCopy( MPBFLOAT* src, MPBFLOAT* dest )
{
	for (int i=0; i<16; i++)
	{
		dest[i] = src[i];
	}
}

/*
#ifdef WIN32
// only works for 0 < slope <= 1
// called by generateLine(), below
void bresenhamLine(	const POINT& from, 
							const POINT& to, 
							vector<POINT>& pointVector	)
{
	POINT p1 = from;
	POINT p2 = to;

	LONG x = p1.x;
	LONG y = p1.y;

	POINT p = { x, y };
	pointVector.push_back( p );

	LONG dx = p2.x - p1.x;
	LONG dy = p2.y - p1.y;

	MPBFLOAT m = (float)dy / (float)dx;

	// special case: horizontal line
	if (m == 0)
	{
		while (x < p2.x)
		{
			x++;
			POINT p = { x, y };
			pointVector.push_back( p );
		}

		return;
	}

	// special case: slope = 1
	if (m == 1)
	{
		while (x < p2.x)
		{
			x++;
			y++;

			POINT p = { x, y };
			pointVector.push_back( p );
		}

		return;
	}

	LONG dT = 2 * (dy - dx);
	LONG dS = 2 * dy;
	LONG d = 2 * dy - dx;

	while (x < p2.x)
	{
		x++;
		if (d < 0) 
		{
			d = d + dS;
		}
		else
		{
			y++;
			d = d + dT;
		}

		POINT p = { x, y };
		pointVector.push_back( p );
	}
}

// points are always returned left-to-right
void generateLine( const POINT& from, const POINT& to, vector<POINT>& pointVector )
{
	POINT p1 = from;
	POINT p2 = to;

	if (p1.x > p2.x)
	{
		p1 = to;
		p2 = from;
	}

	LONG dx = p2.x - p1.x;
	LONG dy = p2.y - p1.y;

	// special case: vertical line
	if (dx == 0)
	{
		LONG x = p1.x;
		LONG y = p1.y;

		POINT p = { x, y };
		pointVector.push_back( p );

		LONG incr = 1;
		if (p2.y < p1.y) incr = -1;
		while (y != p2.y)
		{
			y += incr;

			POINT p = { x, y };
			pointVector.push_back( p );
		}

		return;
	}

	MPBFLOAT m = (float)dy / (float)dx;

	if (m >= 0 && m <= 1)
	{
		bresenhamLine( p1, p2, pointVector );
		return;
	}
	else if (m >= -1 && m <= 0)
	{
		p1.y = -p1.y;
		p2.y = -p2.y;
		bresenhamLine( p1, p2, pointVector );

		VECTOR_LOOP( pointVector, POINT )
		{
			(*itr).y = -(*itr).y;
		}}

		return;
	}
	else if (m > 1)
	{
		swap( p1.x, p1.y );
		swap( p2.x, p2.y );
		bresenhamLine( p1, p2, pointVector );

		VECTOR_LOOP( pointVector, POINT )
		{
			swap( ((*itr).x), ((*itr).y) );
		}}

		return;
	}
	else if (m < -1)
	{
		p1.y = -p1.y;
		p2.y = -p2.y;
		swap( p1.x, p1.y );
		swap( p2.x, p2.y );
		bresenhamLine( p1, p2, pointVector );

		VECTOR_LOOP( pointVector, POINT )
		{
			swap( ((*itr).x), ((*itr).y) );
			(*itr).y = -(*itr).y;
		}}

		return;
	}
}
#endif
*/

MPBVector findPlaneNormal( const MPBVector& P1, const MPBVector& P2, const MPBVector& P3 )
{
	MPBVector V1( P2 - P1 );
	MPBVector V2( P3 - P1 );

	// find cross product of V1 and V2 to get normal

	MPBVector normal = V1.crossProduct( V2 );
	normal.normalize();
	return normal;
}

MPBVector rotationToVector( const MPBVector& rotation )
{
	MPBVector v( 0, 0, -1 );
	v.rotateX( rotation.x );
	v.rotateY( rotation.y );
	v.rotateZ( rotation.z );
	
	return v;
}
