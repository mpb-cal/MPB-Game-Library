
#include "MPBGlobal.h"
#include "MPBOpenGL.h"
#include "MPBShape.h"
#include "MPBShapeBox.h"
#include "MPBShapeFrustum.h"
#include "MPBCompositeShapeRoom.h"
#include "mmgr.h"


MPBCompositeShapeRoom::MPBCompositeShapeRoom(
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
	bool visible	) :

	MPBCompositeShape()
{
	MPBShapeBox* sb;
	MPBFLOAT offset = wallThickness / 10;
	MPB_ASSERT( offset );

	if (southWall)
	{
		sb = new MPBShapeBox( width, height, wallThickness, textureWall,
			texSizeFrontX, texSizeFrontY, texSizeBackX, texSizeBackY, texSizeTopX, texSizeTopY, texSizeBottomX, texSizeBottomY, texSizeLeftX, texSizeLeftY, texSizeRightX, texSizeRightY,
			false );
		sb->translate( 0, 0, -wallThickness/2 );
		sb->scale( scale.x, scale.y, scale.z );
		sb->translate( 0, 0, depth/2 );
		sb->rotateX( rotation.x );
		sb->rotateY( rotation.y );
		sb->rotateZ( rotation.z );
		sb->translate( xPos, yPos, zPos );
		sb->setDraw( visible );
		addShape( sb );
	}

	if (northWall)
	{
		sb = new MPBShapeBox( width, height, wallThickness, textureWall,
			texSizeFrontX, texSizeFrontY, texSizeBackX, texSizeBackY, texSizeTopX, texSizeTopY, texSizeBottomX, texSizeBottomY, texSizeLeftX, texSizeLeftY, texSizeRightX, texSizeRightY,
			false );
		sb->translate( 0, 0, wallThickness/2 );
		sb->scale( scale.x, scale.y, scale.z );
		sb->translate( 0, 0, -depth/2 );
		sb->rotateX( rotation.x );
		sb->rotateY( rotation.y );
		sb->rotateZ( rotation.z );
		sb->translate( xPos, yPos, zPos );
		sb->setDraw( visible );
		addShape( sb );
	}

	if (westWall)
	{
		MPBFLOAT z;
		MPBFLOAT d;

		if (southWall) 
		{
			z = zPos - wallThickness;
			if (northWall) 
			{
				d = depth - wallThickness;
			}
			else
			{
				d = depth;
			}
		}
		else
		{
			z = zPos - depth/2;
			if (northWall) 
			{
				d = depth;
			}
			else
			{
				d = depth + wallThickness;
			}
		}

		sb = new MPBShapeBox( wallThickness-offset, height, depth-offset, textureWall,
			texSizeFrontX, texSizeFrontY, texSizeBackX, texSizeBackY, texSizeTopX, texSizeTopY, texSizeBottomX, texSizeBottomY, texSizeLeftX, texSizeLeftY, texSizeRightX, texSizeRightY,
			false );
		sb->translate( wallThickness/2, 0, 0 );
		sb->scale( scale.x, scale.y, scale.z );
		sb->translate( -width/2, 0, 0 );
		sb->rotateX( rotation.x );
		sb->rotateY( rotation.y );
		sb->rotateZ( rotation.z );
		sb->translate( xPos, yPos, zPos );
		sb->setDraw( visible );
		addShape( sb );
	}

	if (eastWall)
	{
		MPBFLOAT z;
		MPBFLOAT d;

		if (southWall) 
		{
			z = zPos - wallThickness;
			if (northWall) 
			{
				d = depth - wallThickness;
			}
			else
			{
				d = depth;
			}
		}
		else
		{
			z = zPos;
			if (northWall) 
			{
				d = depth;
			}
			else
			{
				d = depth + wallThickness;
			}
		}

		sb = new MPBShapeBox( wallThickness-offset, height, depth-offset, textureWall,
			texSizeFrontX, texSizeFrontY, texSizeBackX, texSizeBackY, texSizeTopX, texSizeTopY, texSizeBottomX, texSizeBottomY, texSizeLeftX, texSizeLeftY, texSizeRightX, texSizeRightY,
			false );
		sb->translate( -wallThickness/2, 0, 0 );
		sb->scale( scale.x, scale.y, scale.z );
		sb->translate( width/2, 0, 0 );
		sb->rotateX( rotation.x );
		sb->rotateY( rotation.y );
		sb->rotateZ( rotation.z );
		sb->translate( xPos, yPos, zPos );
		sb->setDraw( visible );
		addShape( sb );
	}

	if (roof)
	{
		sb = new MPBShapeBox( width, wallThickness, depth, textureRoof,
			texSizeFrontX, texSizeFrontY, texSizeBackX, texSizeBackY, texSizeTopX, texSizeTopY, texSizeBottomX, texSizeBottomY, texSizeLeftX, texSizeLeftY, texSizeRightX, texSizeRightY,
			false );
		sb->scale( scale.x, scale.y, scale.z );
		sb->translate( 0, height/2 + wallThickness/2, 0 );
		sb->rotateX( rotation.x );
		sb->rotateY( rotation.y );
		sb->rotateZ( rotation.z );
		sb->translate( xPos, yPos, zPos );
		sb->setDraw( visible );
		addShape( sb );
	}

	if (floor)
	{
		sb = new MPBShapeBox( width-offset, wallThickness, depth-offset, textureWall,
			texSizeFrontX, texSizeFrontY, texSizeBackX, texSizeBackY, texSizeTopX, texSizeTopY, texSizeBottomX, texSizeBottomY, texSizeLeftX, texSizeLeftY, texSizeRightX, texSizeRightY,
			false );
		sb->scale( scale.x, scale.y, scale.z );
		sb->translate( 0, -height/2 - wallThickness/2, 0 );
		sb->rotateX( rotation.x );
		sb->rotateY( rotation.y );
		sb->rotateZ( rotation.z );
		sb->translate( xPos, yPos, zPos );
		sb->setDraw( visible );
		addShape( sb );
	}
}

MPBCompositeShapeRoom::~MPBCompositeShapeRoom()
{
}



