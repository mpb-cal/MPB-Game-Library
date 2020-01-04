
#include "MPBGlobal.h"
#include "MPBOpenGL.h"
#include "MPBShape.h"
#include "MPBShapeBox.h"
#include "MPBShapeFrustum.h"
#include "MPBCompositeShapeTube.h"
#include "mmgr.h"


// starts out running from origin up Y axis

// radii are inner radii

MPBCompositeShapeTube::MPBCompositeShapeTube(	MPBFLOAT baseRadius, 
																MPBFLOAT topRadius, 
																MPBFLOAT height, 
																MPBFLOAT thickness, 
																int slices, 
																const char* texture	) :
	MPBCompositeShape()
{
	MPB_ASSERT( slices > 2 );

/*	addShape( new MPBShapeFrustum(
		MPBVector( -baseRadius, 0, baseRadius ),
		MPBVector(  baseRadius, 0, baseRadius ),
		MPBVector(  baseRadius, height, baseRadius ),
		MPBVector( -baseRadius, height, baseRadius ),
		MPBVector( -baseRadius, 0, -baseRadius ),
		MPBVector(  baseRadius, 0, -baseRadius ),
		MPBVector(  baseRadius, height, -baseRadius ),
		MPBVector( -baseRadius, height, -baseRadius ),
		texture
	));
*/
	MPBFLOAT incrementDegrees = 360.0 / slices;

	MPBVector startBottomNear( 0, 0, -baseRadius );
	MPBVector NLL( startBottomNear );
	MPBVector NLR( startBottomNear );

	MPBVector startTopNear( 0, height, -topRadius );
	MPBVector NUL( startTopNear );
	MPBVector NUR( startTopNear );

	MPBVector startBottomFar( 0, 0, -baseRadius - thickness );
	MPBVector FLL( startBottomFar );
	MPBVector FLR( startBottomFar );

	MPBVector startTopFar( 0, height, -topRadius - thickness );
	MPBVector FUL( startTopFar );
	MPBVector FUR( startTopFar );

	for (int i=0; i<slices; i++)
	{
		if (i == slices - 1) 
		{
			NLR = startBottomNear;
			NUR = startTopNear;
			FLR = startBottomFar;
			FUR = startTopFar;
		}
		else 
		{
			NLR.rotateY( -incrementDegrees );
			NUR.rotateY( -incrementDegrees );
			FLR.rotateY( -incrementDegrees );
			FUR.rotateY( -incrementDegrees );
		}

		addShape( new MPBShapeFrustum( NLL, NLR, NUR, NUL, FLL, FLR, FUR, FUL, texture ) );

		NLL = NLR;
		NUL = NUR;
		FLL = FLR;
		FUL = FUR;
	}
}

MPBCompositeShapeTube::~MPBCompositeShapeTube()
{
}

