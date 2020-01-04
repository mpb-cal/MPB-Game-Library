#ifndef __MPB_GEOMETRY_H__
#define __MPB_GEOMETRY_H__


#include "MPBVector.h"


MPBVector cylindricalToCartesian( MPBFLOAT rho, MPBFLOAT theta );
void cartesianToCylindrical( MPBFLOAT x, MPBFLOAT y, MPBFLOAT rho, MPBFLOAT& theta );
MPBVector sphericalToCartesian( MPBFLOAT rho, MPBFLOAT theta, MPBFLOAT phi );
void cartesianToSpherical( const MPBVector cartesian, MPBFLOAT& theta, MPBFLOAT& phi );

MPBVector findPlaneNormal( const MPBVector& P1, const MPBVector& P2, const MPBVector& P3 );
MPBVector rotationToVector( const MPBVector& rotation );

int roundToInt( MPBFLOAT d );
void matrixMultiply( MPBFLOAT* a, MPBFLOAT* b, MPBFLOAT* result );
void matrixCopy( MPBFLOAT* src, MPBFLOAT* dest );



#endif

