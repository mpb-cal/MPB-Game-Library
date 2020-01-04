
#include "MPBGlobal.h"
#include "MPBCompositeShapeStairs.h"
#include "MPBShapeFrustum.h"
#include "MPBShapeBox.h"
#include "MPBTexturedPlane.h"
#include "MPBOpenGL.h"
#include "mmgr.h"


MPBCompositeShapeStairs::MPBCompositeShapeStairs(	
	MPBFLOAT width, MPBFLOAT height, MPBFLOAT depth, const char* texture	):
	MPBCompositeShape()
{
	MPBShapeFrustum* sf = new MPBShapeFrustum(
		MPBVector( 0, -.01, 0 ),
		MPBVector( width, -.01, 0 ),
		MPBVector( width,  0, 0 ),
		MPBVector( 0,  0, 0 ),
		MPBVector( 0, -.01, -depth ),
		MPBVector( width, -.01, -depth ),
		MPBVector( width,  height, -depth ),
		MPBVector( 0,  height, -depth ),
		texture
	);

	sf->setDraw( false );
	addShape( sf );


	MPBShapeBox* sb;

	MPBFLOAT stepDepth = .5;
	MPBFLOAT steps = depth / stepDepth;
	MPBFLOAT stepHeight = height / steps;

	MPBFLOAT z = 0;

	for (int i=0; i<steps; i++)
	{
		sb = new MPBShapeBox( width, stepHeight * (i + 1), stepDepth, texture, true );
		sb->translate( 0, 0, z );
		sb->setCollidable( false );
		sb->setTexSize( 0, 4, 4 );
		addShape( sb );

		z -= stepDepth;
	}
}


MPBCompositeShapeStairs::~MPBCompositeShapeStairs()
{
}





