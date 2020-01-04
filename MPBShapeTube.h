#ifndef __SHAPETUBE_H__
#define __SHAPETUBE_H__

#include "MPBLibrary.h"
#include "MPBShape.h"


class MPBShapeTube: public MPBShape
{
public:

	MPBShapeTube(	MPBFLOAT baseRadius, 
						MPBFLOAT topRadius, 
						MPBFLOAT height, 
						int slices, 
						bool capped,
						const char* texture	);

	virtual ~MPBShapeTube();


private:

};


#endif
