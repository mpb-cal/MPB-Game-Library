
#include "MPBGlobal.h"
#include "MPBShapeBall.h"
#include "MPBTexturedPlane.h"
#include "MPBOpenGL.h"
#include "mmgr.h"

MPBShapeBall::MPBShapeBall(	MPBFLOAT radius, 
										int slices, 
										int stacks,
										const char* texture,
										bool oneTexturePerPlane	):
	MPBShape()
{
	MPB_ASSERT( slices > 2 );
	MPB_ASSERT( stacks > 2 );

	// make even number of stacks
	stacks /= 2;
	stacks *= 2;

	MPBFLOAT incrementDegreesStacks = 180.0 / stacks;
	MPBVector startLL( 0, -radius, 0 );
	MPBVector startUL( startLL );
	startUL.rotateX( incrementDegreesStacks );

	// make rings around Y axis from bottom to top
	for (int j=0; j<stacks; j++)
	{
		MPBFLOAT incrementDegreesSlices = 360.0 / slices;

		MPBVector LL( startLL );
		MPBVector LR( startLL );

		MPBVector UR( startUL );
		MPBVector UL( startUL );

		// make a ring of planes around the Y axis
		for (int i=0; i<slices; i++)
		{
			if (i == slices - 1)	// avoid gaps after coming full circle
			{
				LR = startLL;
				UR = startUL;
			}
			else 
			{
				LR.rotateY( incrementDegreesSlices );
				UR.rotateY( incrementDegreesSlices );
			}

// mpb! broken
			MPBTexturedPlane* tp = new MPBTexturedPlane( LL, LR, UR, UL );
			tp->addTexture( texture );
			addPlane( tp );

			LL = LR;
			UL = UR;
		}

		startLL.rotateX( incrementDegreesStacks );
		startUL.rotateX( incrementDegreesStacks );
	}
}

MPBShapeBall::~MPBShapeBall()
{
}

