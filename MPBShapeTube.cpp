
#include "MPBGlobal.h"
#include "MPBShapeTube.h"
#include "MPBTexturedPlane.h"
#include "MPBOpenGL.h"
#include "mmgr.h"


// starts out running from origin down -Z axis

MPBShapeTube::MPBShapeTube(	MPBFLOAT baseRadius, 
										MPBFLOAT topRadius, 
										MPBFLOAT height, 
										int slices, 
										bool capped,
										const char* texture	) :
	MPBShape()
{
	MPB_ASSERT( slices > 2 );

	MPBFLOAT incrementDegrees = 360.0 / slices;

	MPBVector centerBottom( 0, 0, 0 );
	MPBVector startBottom( 0, 0, -baseRadius );
	MPBVector LL( startBottom );
	MPBVector LR( startBottom );

	MPBVector centerTop( 0, height, 0 );
	MPBVector startTop( 0, height, -topRadius );
	MPBVector UL( startTop );
	MPBVector UR( startTop );

	for (int i=0; i<slices; i++)
	{
		if (i == slices - 1) 
		{
			LR = startBottom;
			UR = startTop;
		}
		else 
		{
			LR.rotateY( incrementDegrees );
			UR.rotateY( incrementDegrees );
		}

		MPBTexturedPlane* tp;

		tp = new MPBTexturedPlane( LL, LR, UR, UL );
		tp->addTexture( texture );
		addPlane( tp );

		if (capped)
		{
			tp = new MPBTexturedPlane( LL, centerBottom, LR );
			tp->addTexture( texture );
			addPlane( tp );

			tp = new MPBTexturedPlane( UR, centerTop, UL );
			tp->addTexture( texture );
			addPlane( tp );
		}

		LL = LR;
		UL = UR;
	}

	rotateX( -90 );
}

MPBShapeTube::~MPBShapeTube()
{
}




