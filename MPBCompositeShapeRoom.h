#ifndef __MPBCOMPOSITESHAPEROOM_H__
#define __MPBCOMPOSITESHAPEROOM_H__


#include "MPBCompositeShape.h"


class MPBShapeBox;
class MPBVector;

class MPBCompositeShapeRoom: public MPBCompositeShape
{
public:

	MPBCompositeShapeRoom(
		MPBFLOAT xPos, MPBFLOAT yPos, MPBFLOAT zPos, 
		MPBFLOAT width, MPBFLOAT height, MPBFLOAT depth,
		const MPBVector& rotation,
		const MPBVector& scale,
		MPBFLOAT wallThickness,
		const char* textureWall,
		const char* textureRoof,
		MPBFLOAT texSizeFrontX,
		MPBFLOAT texSizeFrontY,
		MPBFLOAT texSizeBackX,
		MPBFLOAT texSizeBackY,
		MPBFLOAT texSizeTopX,
		MPBFLOAT texSizeTopY,
		MPBFLOAT texSizeBottomX,
		MPBFLOAT texSizeBottomY,
		MPBFLOAT texSizeLeftX,
		MPBFLOAT texSizeLeftY,
		MPBFLOAT texSizeRightX,
		MPBFLOAT texSizeRightY,
		bool floor, 
		bool roof, 
		bool northWall, 
		bool southWall,
		bool eastWall,
		bool westWall,
		bool visible	);

	~MPBCompositeShapeRoom();

protected:

};





#endif

