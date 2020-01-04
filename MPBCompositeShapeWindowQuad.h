#ifndef __MPBCOMPOSITESHAPEWINDOWQUAD_H__
#define __MPBCOMPOSITESHAPEWINDOWQUAD_H__

#include "MPBLibrary.h"
#include "MPBCompositeShape.h"

class Plane;

class MPBCompositeShapeWindowQuad: public MPBCompositeShape
{
public:

	MPBCompositeShapeWindowQuad( 	MPBVector& LL,
											MPBVector& LR,
											MPBVector& UR,
											MPBVector& UL, 
											MPBFLOAT leftInset,
											MPBFLOAT rightInset,
											MPBFLOAT topInset,
											MPBFLOAT bottomInset,
											MPBFLOAT frameWidth,
											const char* texture );

	virtual ~MPBCompositeShapeWindowQuad();

	Plane*		windowPlane;
};


#endif
