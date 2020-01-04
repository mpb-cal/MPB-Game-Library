
#include "MPBGlobal.h"
#include "MPBCompositeShapeWindowQuad.h"
#include "MPBShapeQuad.h"
#include "MPBTexturedPlane.h"
#include "mmgr.h"


MPBCompositeShapeWindowQuad::MPBCompositeShapeWindowQuad( 	MPBVector& LL,
																				MPBVector& LR,
																				MPBVector& UR,
																				MPBVector& UL, 
																				MPBFLOAT leftInset,
																				MPBFLOAT rightInset,
																				MPBFLOAT topInset,
																				MPBFLOAT bottomInset,
																				MPBFLOAT frameWidth,		// 0 for no window frame
																				const char* texture ):
	MPBCompositeShape(),
	windowPlane( NULL )
{
	MPBShapeQuad* sq;
	
	MPBVector v1 = LR - LL;
	MPBVector v2 = UL - LL;
	MPBFLOAT width = v1.getLength();
	MPBFLOAT height = v2.getLength();
	v1.normalize();
	v2.normalize();

	/*

				UL			w1		w2			UR
		^
		|					w3		w4
		v2
		|					w5		w6
		|
				LL			w7		w8			LR

				--- v1 --->

	*/

	MPB_ASSERT( leftInset + rightInset < width );
	MPB_ASSERT( topInset + bottomInset < height );

	MPBVector w1 = UL + v1 * leftInset;// * width;
	MPBVector w7 = LL + v1 * leftInset;// * width;
	MPBVector w2 = UR - v1 * rightInset;// * width;
	MPBVector w8 = LR - v1 * rightInset;// * width;

	MPBVector w3 = w1 - v2 * topInset;// * height;
	MPBVector w4 = w2 - v2 * topInset;// * height;
	MPBVector w5 = w7 + v2 * bottomInset;// * height;
	MPBVector w6 = w8 + v2 * bottomInset;// * height;

	windowPlane = new Plane( w5, w6, w4, w3 );

	if (leftInset)
	{
		sq = new MPBShapeQuad( LL, w7, w1, UL, texture );
		addShape( sq );
	}

	if (rightInset)
	{
		sq = new MPBShapeQuad( w8, LR, UR, w2, texture );
		addShape( sq );
	}

	if (topInset)
	{
		sq = new MPBShapeQuad( w3, w4, w2, w1, texture );
		addShape( sq );
	}

	if (bottomInset)
	{
		sq = new MPBShapeQuad( w7, w8, w6, w5, texture );
		addShape( sq );
	}

	if (frameWidth)
	{
		MPBShapeQuad* sq;
		Plane plane( LL, LR, UR, UL );
		MPBVector extrusion = plane.getNormal() * -frameWidth;

		// top
		sq = new MPBShapeQuad( 
			w3 + extrusion, 
			w4 + extrusion,
			w4, 
			w3, 
			texture );
		addShape( sq );

		// bottom
		sq = new MPBShapeQuad( 
			w5,
			w6,
			w6 + extrusion,
			w5 + extrusion, 
			texture );
		addShape( sq );

		// left
		sq = new MPBShapeQuad( 
			w5, 
			w5 + extrusion, 
			w3 + extrusion, 
			w3,
			texture );
		addShape( sq );

		// right
		sq = new MPBShapeQuad( 
			w6 + extrusion, 
			w6, 
			w4,
			w4 + extrusion,
			texture );
		addShape( sq );

		windowPlane->setCorners( w5 + extrusion, w6 + extrusion, w4 + extrusion, w3 + extrusion );
	}
}


MPBCompositeShapeWindowQuad::~MPBCompositeShapeWindowQuad()
{
	delete windowPlane;
}





