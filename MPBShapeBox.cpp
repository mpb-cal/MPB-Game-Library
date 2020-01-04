
#include "MPBGlobal.h"
#include "MPBShapeBox.h"
#include "MPBTexturedPlane.h"
#include "MPBOpenGL.h"
#include "mmgr.h"

MPBShapeBox::MPBShapeBox(	MPBFLOAT width, MPBFLOAT height, MPBFLOAT depth,
									const char* textureFront,
									const char* textureBack,
									const char* textureTop,
									const char* textureBottom,
									const char* textureLeft,
									const char* textureRight,
									bool lowerLeftOrigin	):
	MPBShape()
{
	MPBVector texCoordFront[4] = { 
		MPBVector( 0, 0, 0 ),
		MPBVector( 1, 0, 0 ),
		MPBVector( 1, 1, 0 ),
		MPBVector( 0, 1, 0 )
	};

	MPBVector texCoordBack[4];
	MPBVector texCoordTop[4];
	MPBVector texCoordBottom[4];
	MPBVector texCoordLeft[4];
	MPBVector texCoordRight[4];

	for (int i=0; i<4; i++)
	{
		texCoordBack[i] = texCoordFront[i];
		texCoordTop[i] = texCoordFront[i];
		texCoordBottom[i] = texCoordFront[i];
		texCoordLeft[i] = texCoordFront[i];
		texCoordRight[i] = texCoordFront[i];
	}

	initialize(	width, height, depth,
					textureFront,
					textureBack,
					textureTop,
					textureBottom,
					textureLeft,
					textureRight,
					texCoordFront,
					texCoordBack,
					texCoordTop,
					texCoordBottom,
					texCoordLeft,
					texCoordRight,
					lowerLeftOrigin	);
}

MPBShapeBox::MPBShapeBox(	MPBFLOAT width, MPBFLOAT height, MPBFLOAT depth,
									const char* texture,
									bool lowerLeftOrigin	):
	MPBShape()
{
	MPBVector texCoordFront[4] = { 
		MPBVector( 0, 0, 0 ),
		MPBVector( 1, 0, 0 ),
		MPBVector( 1, 1, 0 ),
		MPBVector( 0, 1, 0 )
	};

	MPBVector texCoordBack[4];
	MPBVector texCoordTop[4];
	MPBVector texCoordBottom[4];
	MPBVector texCoordLeft[4];
	MPBVector texCoordRight[4];

	for (int i=0; i<4; i++)
	{
		texCoordBack[i] = texCoordFront[i];
		texCoordTop[i] = texCoordFront[i];
		texCoordBottom[i] = texCoordFront[i];
		texCoordLeft[i] = texCoordFront[i];
		texCoordRight[i] = texCoordFront[i];
	}

	initialize(	width, height, depth,
					texture,
					texture,
					texture,
					texture,
					texture,
					texture,
					texCoordFront,
					texCoordBack,
					texCoordTop,
					texCoordBottom,
					texCoordLeft,
					texCoordRight,
					lowerLeftOrigin	);
}

MPBShapeBox::MPBShapeBox(	MPBFLOAT width, MPBFLOAT height, MPBFLOAT depth,
									const char* texture,
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
									bool lowerLeftOrigin	):
	MPBShape()
{
	MPBVector texCoordFront[4] = { 
		MPBVector( 0, 0, 0 ),
		MPBVector( width / texSizeFrontX, 0, 0 ),
		MPBVector( width / texSizeFrontX, height / texSizeFrontY, 0 ),
		MPBVector( 0, height / texSizeFrontY, 0 )
	};

	MPBVector texCoordBack[4] = { 
		MPBVector( 0, 0, 0 ),
		MPBVector( width / texSizeBackX, 0, 0 ),
		MPBVector( width / texSizeBackX, height / texSizeBackY, 0 ),
		MPBVector( 0, height / texSizeBackY, 0 )
	};

	MPBVector texCoordTop[4] = { 
		MPBVector( 0, 0, 0 ),
		MPBVector( width / texSizeTopX, 0, 0 ),
		MPBVector( width / texSizeTopX, depth / texSizeTopY, 0 ),
		MPBVector( 0, depth / texSizeTopY, 0 )
	};

	MPBVector texCoordBottom[4] = { 
		MPBVector( 0, 0, 0 ),
		MPBVector( width / texSizeBottomX, 0, 0 ),
		MPBVector( width / texSizeBottomX, depth / texSizeBottomY, 0 ),
		MPBVector( 0, depth / texSizeBottomY, 0 )
	};

	MPBVector texCoordLeft[4] = { 
		MPBVector( 0, 0, 0 ),
		MPBVector( depth / texSizeLeftX, 0, 0 ),
		MPBVector( depth / texSizeLeftX, height / texSizeLeftY, 0 ),
		MPBVector( 0, height / texSizeLeftY, 0 )
	};

	MPBVector texCoordRight[4] = { 
		MPBVector( 0, 0, 0 ),
		MPBVector( depth / texSizeRightX, 0, 0 ),
		MPBVector( depth / texSizeRightX, height / texSizeRightY, 0 ),
		MPBVector( 0, height / texSizeRightY, 0 )
	};


	initialize(	width, height, depth,
					texture, texture, texture, texture, texture, texture,
					texCoordFront,
					texCoordBack,
					texCoordTop,
					texCoordBottom,
					texCoordLeft,
					texCoordRight,
					lowerLeftOrigin	);
}

MPBShapeBox::MPBShapeBox(	MPBFLOAT width, MPBFLOAT height, MPBFLOAT depth,
									const char* textureFront,
									const char* textureBack,
									const char* textureTop,
									const char* textureBottom,
									const char* textureLeft,
									const char* textureRight,
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
									bool lowerLeftOrigin	):
	MPBShape()
{
	MPBVector texCoordFront[4] = { 
		MPBVector( 0, 0, 0 ),
		MPBVector( width / texSizeFrontX, 0, 0 ),
		MPBVector( width / texSizeFrontX, height / texSizeFrontY, 0 ),
		MPBVector( 0, height / texSizeFrontY, 0 )
	};

	MPBVector texCoordBack[4] = { 
		MPBVector( 0, 0, 0 ),
		MPBVector( width / texSizeBackX, 0, 0 ),
		MPBVector( width / texSizeBackX, height / texSizeBackY, 0 ),
		MPBVector( 0, height / texSizeBackY, 0 )
	};

	MPBVector texCoordTop[4] = { 
		MPBVector( 0, 0, 0 ),
		MPBVector( width / texSizeTopX, 0, 0 ),
		MPBVector( width / texSizeTopX, depth / texSizeTopY, 0 ),
		MPBVector( 0, depth / texSizeTopY, 0 )
	};

	MPBVector texCoordBottom[4] = { 
		MPBVector( 0, 0, 0 ),
		MPBVector( width / texSizeBottomX, 0, 0 ),
		MPBVector( width / texSizeBottomX, depth / texSizeBottomY, 0 ),
		MPBVector( 0, depth / texSizeBottomY, 0 )
	};

	MPBVector texCoordLeft[4] = { 
		MPBVector( 0, 0, 0 ),
		MPBVector( depth / texSizeLeftX, 0, 0 ),
		MPBVector( depth / texSizeLeftX, height / texSizeLeftY, 0 ),
		MPBVector( 0, height / texSizeLeftY, 0 )
	};

	MPBVector texCoordRight[4] = { 
		MPBVector( 0, 0, 0 ),
		MPBVector( depth / texSizeRightX, 0, 0 ),
		MPBVector( depth / texSizeRightX, height / texSizeRightY, 0 ),
		MPBVector( 0, height / texSizeRightY, 0 )
	};


	initialize(	width, height, depth,
					textureFront, textureBack, textureTop, textureBottom, textureLeft, textureRight,
					texCoordFront,
					texCoordBack,
					texCoordTop,
					texCoordBottom,
					texCoordLeft,
					texCoordRight,
					lowerLeftOrigin	);
}

MPBShapeBox::MPBShapeBox(	MPBFLOAT width, MPBFLOAT height, MPBFLOAT depth,
									const char* textureFront,
									const char* textureBack,
									const char* textureTop,
									const char* textureBottom,
									const char* textureLeft,
									const char* textureRight,
									MPBVector texCoordFront[4],
									MPBVector texCoordBack[4],
									MPBVector texCoordTop[4],
									MPBVector texCoordBottom[4],
									MPBVector texCoordLeft[4],
									MPBVector texCoordRight[4],
									bool lowerLeftOrigin	):
	MPBShape()
{
	initialize(	width, height, depth,
					textureFront,
					textureBack,
					textureTop,
					textureBottom,
					textureLeft,
					textureRight,
					texCoordFront,
					texCoordBack,
					texCoordTop,
					texCoordBottom,
					texCoordLeft,
					texCoordRight,
					lowerLeftOrigin	);
}

void MPBShapeBox::initialize(	MPBFLOAT width, MPBFLOAT height, MPBFLOAT depth,
										const char* textureFront,
										const char* textureBack,
										const char* textureTop,
										const char* textureBottom,
										const char* textureLeft,
										const char* textureRight,
										MPBVector texCoordFront[4],
										MPBVector texCoordBack[4],
										MPBVector texCoordTop[4],
										MPBVector texCoordBottom[4],
										MPBVector texCoordLeft[4],
										MPBVector texCoordRight[4],
										bool lowerLeftOrigin	)
{
	for (int i=0; i<4; i++)
	{
		m_texCoordFront[i] = texCoordFront[i];
		m_texCoordBack[i] = texCoordBack[i];
		m_texCoordTop[i] = texCoordTop[i];
		m_texCoordBottom[i] = texCoordBottom[i];
		m_texCoordLeft[i] = texCoordLeft[i];
		m_texCoordRight[i] = texCoordRight[i];
	}

	strcpy( m_textureFront, textureFront );
	strcpy( m_textureBack, textureBack );
	strcpy( m_textureTop, textureTop );
	strcpy( m_textureBottom, textureBottom );
	strcpy( m_textureLeft, textureLeft );
	strcpy( m_textureRight, textureRight );

	MPBVector NLL;
	MPBVector NLR;
	MPBVector NUR;
	MPBVector NUL;
	MPBVector FLL;
	MPBVector FLR;
	MPBVector FUR;
	MPBVector FUL;

	MPBTexturedPlane* plane;

	if (lowerLeftOrigin)
	{
		NLL.set( 0, 0, 0 );
		NLR.set( width, 0, 0 );
		NUR.set( width, height, 0 );
		NUL.set( 0, height, 0 );
		FLL.set( 0, 0, -depth );
		FLR.set( width, 0, -depth );
		FUR.set( width, height, -depth );
		FUL.set( 0, height, -depth );
	}
	else
	{
		NLL.set( -width/2.0, -height/2.0,  depth/2.0 );
		NLR.set(  width/2.0, -height/2.0,  depth/2.0 );
		NUR.set(  width/2.0,  height/2.0,  depth/2.0 );
		NUL.set( -width/2.0,  height/2.0,  depth/2.0 );
		FLL.set( -width/2.0, -height/2.0, -depth/2.0 );
		FLR.set(  width/2.0, -height/2.0, -depth/2.0 );
		FUR.set(  width/2.0,  height/2.0, -depth/2.0 );
		FUL.set( -width/2.0,  height/2.0, -depth/2.0 );
	}

	// front
	plane = new MPBTexturedPlane( NLL, NLR, NUR, NUL );
	plane->addTexture( textureFront );
	plane->setTexCoords( 0, m_texCoordFront[0], m_texCoordFront[1], m_texCoordFront[2], m_texCoordFront[3] );
	addPlane( plane );

	// back
	plane = new MPBTexturedPlane( FLR, FLL, FUL, FUR );
	plane->addTexture( textureBack );
	plane->setTexCoords( 0, m_texCoordBack[0], m_texCoordBack[1], m_texCoordBack[2], m_texCoordBack[3] );
	addPlane( plane );

	// top
	plane = new MPBTexturedPlane( NUL, NUR, FUR, FUL );
	plane->addTexture( textureTop );
	plane->setTexCoords( 0, m_texCoordTop[0], m_texCoordTop[1], m_texCoordTop[2], m_texCoordTop[3] );
	addPlane( plane );

	// bottom
	plane = new MPBTexturedPlane( FLL, FLR, NLR, NLL );
	plane->addTexture( textureBottom );
	plane->setTexCoords( 0, m_texCoordBottom[0], m_texCoordBottom[1], m_texCoordBottom[2], m_texCoordBottom[3] );
	addPlane( plane );

	// left
	plane = new MPBTexturedPlane( FLL, NLL, NUL, FUL );
	plane->addTexture( textureLeft );
	plane->setTexCoords( 0, m_texCoordLeft[0], m_texCoordLeft[1], m_texCoordLeft[2], m_texCoordLeft[3] );
	addPlane( plane );

	// right
	plane = new MPBTexturedPlane( NLR, FLR, FUR, NUR );
	plane->addTexture( textureRight );
	plane->setTexCoords( 0, m_texCoordRight[0], m_texCoordRight[1], m_texCoordRight[2], m_texCoordRight[3] );
	addPlane( plane );

	// ODE stuff

	if (1)
	{
		m_geomID = dCreateBox( 0, width, height, depth );
	}
	else if (0)	// trimesh testing
	{
		dTriMeshDataID triMeshDataID = dGeomTriMeshDataCreate();

		int numVertices = 4;
		int numIndices = 6;
		dVector3* vertices = new dVector3[numVertices];

		i = 0;
		vertices[0][0] = NUL.x;
		vertices[0][1] = NUL.y;
		vertices[0][2] = NUL.z;
		vertices[1][0] = NUR.x;
		vertices[1][1] = NUR.y;
		vertices[1][2] = NUR.z;
		vertices[2][0] = FUR.x;
		vertices[2][1] = FUR.y;
		vertices[2][2] = FUR.z;
		vertices[3][0] = FUL.x;
		vertices[3][1] = FUL.y;
		vertices[3][2] = FUL.z;

		int* indices = new int[numIndices];

		i = 0;
		indices[i++] = 0;
		indices[i++] = 1;
		indices[i++] = 2;
		indices[i++] = 2;
		indices[i++] = 3;
		indices[i++] = 0;

		dGeomTriMeshDataBuildSimple( triMeshDataID, (const dReal*)vertices, numVertices, indices, numIndices );

		m_geomID = dCreateTriMesh( 0, triMeshDataID, NULL, NULL, NULL );
	}
}

MPBShapeBox::~MPBShapeBox()
{
}





