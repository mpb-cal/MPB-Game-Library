
/* coherent noise function over 1, 2 or 3 dimensions */
/* (copyright Ken Perlin) */

#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include "MPBPerlinNoise.h"
#include "MPBGlobal.h"
#include "mmgr.h"

#define B 0x100
#define BM 0xff

#define N 0x1000
#define NP 12   /* 2^N */
#define NM 0xfff

static p[B + B + 2];
static double g3[B + B + 2][3];
static double g2[B + B + 2][2];
static double g1[B + B + 2];
static start = 1;

static void init(void);

#define s_curve(t) ( t * t * (3.0 - 2.0f * t) )

#define lerp(t, a, b) ( a + t * (b - a) )

#define setup(i,b0,b1,r0,r1)\
	t = vec[i] + N;\
	b0 = ((int)t) & BM;\
	b1 = (b0+1) & BM;\
	r0 = t - (int)t;\
	r1 = r0 - 1.0f;

double noise1(double arg)
{
	int bx0, bx1;
	double rx0, rx1, sx, t, u, v, vec[1];

	vec[0] = arg;
	if (start) {
		start = 0;
		init();
	}

	setup(0, bx0,bx1, rx0,rx1);

	sx = (double)s_curve(rx0);

	u = rx0 * g1[ p[ bx0 ] ];
	v = rx1 * g1[ p[ bx1 ] ];

	return lerp(sx, u, v);
}

double noise2(double vec[2])
{
	int bx0, bx1, by0, by1, b00, b10, b01, b11;
	double rx0, rx1, ry0, ry1, *q, sx, sy, a, b, t, u, v;
	register i, j;

	if (start) {
		start = 0;
		init();
	}

	setup(0, bx0,bx1, rx0,rx1);
	setup(1, by0,by1, ry0,ry1);

	i = p[ bx0 ];
	j = p[ bx1 ];

	b00 = p[ i + by0 ];
	b10 = p[ j + by0 ];
	b01 = p[ i + by1 ];
	b11 = p[ j + by1 ];

	sx = s_curve(rx0);
	sy = s_curve(ry0);

#define at2(rx,ry) ( rx * q[0] + ry * q[1] )

	q = g2[ b00 ] ; u = at2(rx0,ry0);
	q = g2[ b10 ] ; v = at2(rx1,ry0);
	a = lerp(sx, u, v);

	q = g2[ b01 ] ; u = at2(rx0,ry1);
	q = g2[ b11 ] ; v = at2(rx1,ry1);
	b = lerp(sx, u, v);

	return lerp(sy, a, b);
}

double noise3(double vec[3])
{
	int bx0, bx1, by0, by1, bz0, bz1, b00, b10, b01, b11;
	double rx0, rx1, ry0, ry1, rz0, rz1, *q, sy, sz, a, b, c, d, t, u, v;
	register i, j;

	if (start) {
		start = 0;
		init();
	}

	setup(0, bx0,bx1, rx0,rx1);
	setup(1, by0,by1, ry0,ry1);
	setup(2, bz0,bz1, rz0,rz1);

	i = p[ bx0 ];
	j = p[ bx1 ];

	b00 = p[ i + by0 ];
	b10 = p[ j + by0 ];
	b01 = p[ i + by1 ];
	b11 = p[ j + by1 ];

	t  = s_curve(rx0);
	sy = s_curve(ry0);
	sz = s_curve(rz0);

#define at3(rx,ry,rz) ( rx * q[0] + ry * q[1] + rz * q[2] )

	q = g3[ b00 + bz0 ] ; u = at3(rx0,ry0,rz0);
	q = g3[ b10 + bz0 ] ; v = at3(rx1,ry0,rz0);
	a = lerp(t, u, v);

	q = g3[ b01 + bz0 ] ; u = at3(rx0,ry1,rz0);
	q = g3[ b11 + bz0 ] ; v = at3(rx1,ry1,rz0);
	b = lerp(t, u, v);

	c = lerp(sy, a, b);

	q = g3[ b00 + bz1 ] ; u = at3(rx0,ry0,rz1);
	q = g3[ b10 + bz1 ] ; v = at3(rx1,ry0,rz1);
	a = lerp(t, u, v);

	q = g3[ b01 + bz1 ] ; u = at3(rx0,ry1,rz1);
	q = g3[ b11 + bz1 ] ; v = at3(rx1,ry1,rz1);
	b = lerp(t, u, v);

	d = lerp(sy, a, b);

	return lerp(sz, c, d);
}

static void normalize2(double v[2])
{
	double s;

	s = sqrt(v[0] * v[0] + v[1] * v[1]);
	v[0] = v[0] / s;
	v[1] = v[1] / s;
}

static void normalize3(double v[3])
{
	double s;

	s = sqrt(v[0] * v[0] + v[1] * v[1] + v[2] * v[2]);
	v[0] = v[0] / s;
	v[1] = v[1] / s;
	v[2] = v[2] / s;
}

static void init(void)
{
	int i, j, k;

	for (i = 0 ; i < B ; i++) {
		p[i] = i;

		g1[i] = (double)((rand() % (B + B)) - B) / B;

		for (j = 0 ; j < 2 ; j++)
			g2[i][j] = (double)((rand() % (B + B)) - B) / B;
		normalize2(g2[i]);

		for (j = 0 ; j < 3 ; j++)
			g3[i][j] = (double)((rand() % (B + B)) - B) / B;
		normalize3(g3[i]);
	}

	while (--i) {
		k = p[i];
		p[i] = p[j = rand() % B];
		p[j] = k;
	}

	for (i = 0 ; i < B + 2 ; i++) {
		p[B + i] = p[i];
		g1[B + i] = g1[i];
		for (j = 0 ; j < 2 ; j++)
			g2[B + i][j] = g2[i][j];
		for (j = 0 ; j < 3 ; j++)
			g3[B + i][j] = g3[i][j];
	}
}

/*
// returns between -.1 and .1 for a given xy coordinate
double noise( int x, int y )
{
	int n = x + y * 57;
	n = (n<<13) ^ n;
	return ( 1.0 - ( (n * (n * n * 15731 + 789221) + 1376312589) & 0x7fffffff) / 1073741824.0);
}

// returns between -.1 and .1 for a given xy coordinate
double smoothNoise( int x, int y )
{
//	MPB_ASSERT( x >= 0 );
//	MPB_ASSERT( x < RANDOM_GRID_WIDTH );
//	MPB_ASSERT( y >= 0 );
//	MPB_ASSERT( y < RANDOM_GRID_WIDTH );

	double n;

	double corners = ( noise(x-1, y-1)+noise(x+1, y-1)+noise(x-1, y+1)+noise(x+1, y+1) ) / 16;
	double sides   = ( noise(x-1, y)  +noise(x+1, y)  +noise(x, y-1)  +noise(x, y+1) ) /  8;
	double center  =  noise(x, y) / 4;
	n = corners + sides + center;

	MPB_ASSERT( n >= -1 );
	MPB_ASSERT( n <= 1 );
	return n;
}

double cosineInterpolate( double a, double b, double x )
{
	double ft = x * 3.1415927;
	double f = (1 - cos(ft)) * .5;

	return  a*(1-f) + b*f;
}

double interpolate( double a, double b, double x )
{
	return cosineInterpolate( a, b, x );
}

// returns between -.1 and .1 for a given xy coordinate
double interpolatedNoise( double x, double y, bool smooth )
{
//	MPB_ASSERT( x >= 0 );
//	MPB_ASSERT( x < RANDOM_GRID_WIDTH );
//	MPB_ASSERT( y >= 0 );
//	MPB_ASSERT( y < RANDOM_GRID_WIDTH );

	int integer_X    = int(x);
	double fractional_X = x - integer_X;

	int integer_Y    = int(y);
	double fractional_Y = y - integer_Y;
	
	double v1;
	double v2;
	double v3;
	double v4;

	if (smooth)
	{
		v1 = smoothNoise(integer_X,     integer_Y);
		v2 = smoothNoise(integer_X + 1, integer_Y);
		v3 = smoothNoise(integer_X,     integer_Y + 1);
		v4 = smoothNoise(integer_X + 1, integer_Y + 1);
	}
	else
	{
		v1 = noise(integer_X,     integer_Y);
		v2 = noise(integer_X + 1, integer_Y);
		v3 = noise(integer_X,     integer_Y + 1);
		v4 = noise(integer_X + 1, integer_Y + 1);
	}

	double i1 = interpolate(v1 , v2 , fractional_X);
	double i2 = interpolate(v3 , v4 , fractional_X);

	double in = interpolate(i1 , i2 , fractional_Y);

	return in;
}

// returns between -.1 and .1 for a given xy coordinate
double perlinNoisePoint(	int x, 
									int y, 
									int octaves, 
									double amplitudeDivisor, 
									int startFrequencyDivisor,
									bool smooth	)
{
	MPB_ASSERT( amplitudeDivisor >= 1.0 );
//	MPB_ASSERT( startFrequencyDivisor >= pow( 2, (octaves - 1) ) );

	double total = 0;

	double frequency = 1.0 / startFrequencyDivisor;
	double amplitude = 1.0;

	for (int i=0; i<octaves; i++)
	{
		total += interpolatedNoise( x * frequency, y * frequency, smooth ) * amplitude;

		frequency *= 2;
		amplitude /= amplitudeDivisor;
	}

	if (total > 1) total = 1;
	if (total < -1) total = -1;

	return total;
}
*/

void perlinNoise(	BYTE* array, 
						int width, 
						int height, 
						int maxValue,
						double amplitude, 
						double frequency,
						bool smooth	)
{
	MPB_ASSERT( amplitude <= 1 );

	for (int y=0; y<height; y++)
	{
		for (int x=0; x<width; x++)
		{
			int index = y * width + x;

			// get value between -1 and 1
			double vec[2] = { (double)x * frequency / width, (double)y * frequency / height };
			double n = noise2( vec );
			sprintf( g_logBuf, "%f\n", n );
			writeLog( g_logBuf );
//			n = n * 2;
			MPB_ASSERT( n <= 1.0 );
			MPB_ASSERT( n >= -1.0 );

			// make it between 0 and maxValue
			double middle = maxValue / 2;
			double low = middle - maxValue * amplitude / 2;
			double high = middle + maxValue * amplitude / 2;
			array[index] = remap( 2, low, high, n + 1 );
		}
	}
}

