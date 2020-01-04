
#include "MPBGlobal.h"
#include "MPBOpenGL.h"
#include "MPBShape.h"
#include "MPBShapeBox.h"
#include "MPBShapeFrustum.h"
#include "MPBCompositeShapeRoom.h"
#include "MPBCompositeShapeDoorWall.h"
#include "mmgr.h"


MPBCompositeShapeDoorWall::MPBCompositeShapeDoorWall(
	MPBFLOAT width, 
	MPBFLOAT height, 
	MPBFLOAT depth,
	const char* texture,
	MPBFLOAT texSizeX,
	MPBFLOAT texSizeY,
	bool doorFrame	) :

	MPBCompositeShape()
{
	MPBShapeBox* sb;

	sb = new MPBShapeBox(
		width / 3, height, depth,
		texture,
		texSizeX, texSizeY, texSizeX, texSizeY, texSizeX, texSizeY, texSizeX, texSizeY, texSizeX, texSizeY, texSizeX, texSizeY, 
		false
	);
	sb->translate( -width/3, 0, 0 );
	addShape( sb );

	sb = new MPBShapeBox(
		width / 3, height, depth,
		texture,
		texSizeX, texSizeY, texSizeX, texSizeY, texSizeX, texSizeY, texSizeX, texSizeY, texSizeX, texSizeY, texSizeX, texSizeY, 
		false
	);
	sb->translate(  width/3, 0, 0 );
	addShape( sb );

	sb = new MPBShapeBox(
		width / 3, height / 2, depth,
		texture,
		texSizeX, texSizeY, texSizeX, texSizeY, texSizeX, texSizeY, texSizeX, texSizeY, texSizeX, texSizeY, texSizeX, texSizeY, 
		false
	);
	sb->translate( 0, height / 4, 0 );
	addShape( sb );

	if (doorFrame)
	{
		MPBCompositeShapeRoom* sr;

		sr = new MPBCompositeShapeRoom( 
			0, 0, 0,
			width / 3, height / 2, depth * 2, 
			MPBVector(), MPBVector( 1, 1, 1 ), depth / 2, "marble04", "marble04",
			texSizeX, texSizeY, texSizeX, texSizeY, texSizeX, texSizeY, texSizeX, texSizeY, texSizeX, texSizeY, texSizeX, texSizeY, 
			false, true, false, false, true, true, true );
		sr->translate( 0, -height / 4 - depth / 2, 0 );
		addCompositeShape( sr );
	}
}

MPBCompositeShapeDoorWall::~MPBCompositeShapeDoorWall()
{
}



