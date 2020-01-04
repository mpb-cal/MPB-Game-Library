#ifndef __MPBCOMPOSITESHAPEDOORWALL_H__
#define __MPBCOMPOSITESHAPEDOORWALL_H__


#include "MPBCompositeShape.h"


class MPBShapeBox;
class MPBVector;

class MPBCompositeShapeDoorWall: public MPBCompositeShape
{
public:

	MPBCompositeShapeDoorWall(
		MPBFLOAT width, 
		MPBFLOAT height, 
		MPBFLOAT depth,
		const char* texture,
		MPBFLOAT texSizeX,
		MPBFLOAT texSizeY,
		bool doorFrame	);

	~MPBCompositeShapeDoorWall();

protected:

};





#endif

