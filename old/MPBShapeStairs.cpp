
#include "MPBGlobal.h"
#include "MPBCompositeShapeStairs.h"
#include "MPBTexturedPlane.h"
#include "MPBOpenGL.h"


MPBCompositeShapeStairs::MPBCompositeShapeStairs(	
	MPBFLOAT width, MPBFLOAT height, MPBFLOAT depth, const char* texture	):
	MPBShape()
{
	MPBShapeFrustum* sf = new MPBShapeFrustum(
		MPBVector( 0, -1, 0 ),
		MPBVector( width, -1, 0 ),
		MPBVector( width,  0, 0 ),
		MPBVector( 0,  0, 0 ),
		MPBVector( 0, -1, -depth ),
		MPBVector( width, -1, -depth ),
		MPBVector( width,  height, -depth ),
		MPBVector( 0,  height, -depth ),
		texture
	);

	MPBTexturedPlane* plane;

	MPBFLOAT stepWidth = .5;
	MPBFLOAT steps = depth / stepWidth;
	MPBFLOAT stepHeight = height / steps;

	MPBFLOAT x = -width / 2;
	MPBFLOAT y = -height / 2 + stepHeight;
	MPBFLOAT z = depth / 2;

	while (z > -depth / 2)
	{
		plane = new MPBTexturedPlane( 
			MPBVector( x, -height / 2, z - stepWidth ),
			MPBVector( x, -height / 2, z ),
			MPBVector( x, y, z ),
			MPBVector( x, y, z - stepWidth )
		);
		plane->addTexture( texture );
		addPlane( plane );

		z -= stepWidth;
		y += stepHeight;
	}
}


MPBCompositeShapeStairs::~MPBCompositeShapeStairs()
{
}





