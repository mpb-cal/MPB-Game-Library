#ifndef __COMPOSITE_SHAPE_STAIRS_H__
#define __COMPOSITE_SHAPE_STAIRS_H__

#include "MPBLibrary.h"
#include "MPBCompositeShape.h"


class MPBCompositeShapeStairs: public MPBCompositeShape
{
public:

	MPBCompositeShapeStairs(
		MPBFLOAT width, 
		MPBFLOAT height, 
		MPBFLOAT depth,
		const char* texture	);

	virtual ~MPBCompositeShapeStairs();

private:

};


#endif
