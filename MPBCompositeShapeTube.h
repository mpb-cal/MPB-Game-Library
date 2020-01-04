#ifndef __MPBCOMPOSITESHAPETUBE_H__
#define __MPBCOMPOSITESHAPETUBE_H__


#include "MPBCompositeShape.h"



class MPBCompositeShapeTube: public MPBCompositeShape
{
public:

	MPBCompositeShapeTube(	MPBFLOAT baseRadius, 
									MPBFLOAT topRadius, 
									MPBFLOAT height, 
									MPBFLOAT thickness, 
									int slices, 
									const char* texture	);

	~MPBCompositeShapeTube();

protected:

};





#endif

