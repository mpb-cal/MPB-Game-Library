#ifndef __MPBSHAPEFRUSTUM_H__
#define __MPBSHAPEFRUSTUM_H__

#include "MPBLibrary.h"
#include "MPBShape.h"


class MPBShapeFrustum: public MPBShape
{
public:

	MPBShapeFrustum(	MPBVector NLL,	// points are relative to front view of frustum
							MPBVector NLR,
							MPBVector NUR,
							MPBVector NUL,
							MPBVector FLL,
							MPBVector FLR,
							MPBVector FUR,
							MPBVector FUL,
							const char* texture	);

private:

};


#endif

