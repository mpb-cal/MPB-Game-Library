
#include "MPBGlobal.h"
#include "MPBShapeFrustum.h"
#include "MPBTexturedPlane.h"
#include "mmgr.h"


MPBShapeFrustum::MPBShapeFrustum(	MPBVector NLL,	// points are relative to front view of frustum
												MPBVector NLR,
												MPBVector NUR,
												MPBVector NUL,
												MPBVector FLL,
												MPBVector FLR,
												MPBVector FUR,
												MPBVector FUL,
												const char* texture	):
	MPBShape()
{
	// front
	MPBTexturedPlane* tp = new MPBTexturedPlane( NLL, NLR, NUR, NUL );
	tp->addTexture( texture );
	addPlane( tp );

	// back
	tp = new MPBTexturedPlane( FLR, FLL, FUL, FUR );
	tp->addTexture( texture );
	addPlane( tp );

	// top
	tp = new MPBTexturedPlane( NUL, NUR, FUR, FUL );
	tp->addTexture( texture );
	addPlane( tp );

	// bottom
	tp = new MPBTexturedPlane( FLL, FLR, NLR, NLL );
	tp->addTexture( texture );
	addPlane( tp );

	// left
	tp = new MPBTexturedPlane( FLL, NLL, NUL, FUL );
	tp->addTexture( texture );
	addPlane( tp );

	// right
	tp = new MPBTexturedPlane( NLR, FLR, FUR, NUR );
	tp->addTexture( texture );
	addPlane( tp );
}





