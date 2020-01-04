#ifndef __SHAPEBOX_H__
#define __SHAPEBOX_H__

#include "MPBLibrary.h"
#include "MPBShape.h"

class Plane;

class MPBShapeBox: public MPBShape
{
public:

	MPBShapeBox(	MPBFLOAT width, MPBFLOAT height, MPBFLOAT depth,
						const char* textureFront,
						const char* textureBack,
						const char* textureTop,
						const char* textureBottom,
						const char* textureLeft,
						const char* textureRight,
						bool lowerLeftOrigin	);

	MPBShapeBox(	MPBFLOAT width, MPBFLOAT height, MPBFLOAT depth,
						const char* texture,
						bool lowerLeftOrigin	);

	MPBShapeBox(	MPBFLOAT width, MPBFLOAT height, MPBFLOAT depth,
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
						bool lowerLeftOrigin	);

	MPBShapeBox(	MPBFLOAT width, MPBFLOAT height, MPBFLOAT depth,
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
						bool lowerLeftOrigin	);

	MPBShapeBox(	MPBFLOAT width, MPBFLOAT height, MPBFLOAT depth,
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
						bool lowerLeftOrigin	);

	virtual ~MPBShapeBox();

private:

	char  			m_textureFront[MAX_PATH];
	char  			m_textureBack[MAX_PATH];
	char  			m_textureTop[MAX_PATH];
	char  			m_textureBottom[MAX_PATH];
	char	 		 	m_textureLeft[MAX_PATH];
	char			 	m_textureRight[MAX_PATH];

	MPBVector		m_texCoordFront[4];
	MPBVector		m_texCoordBack[4];
	MPBVector		m_texCoordTop[4];
	MPBVector		m_texCoordBottom[4];
	MPBVector		m_texCoordLeft[4];
	MPBVector		m_texCoordRight[4];


	void initialize(	MPBFLOAT width, MPBFLOAT height, MPBFLOAT depth,
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
							bool lowerLeftOrigin	);
};


#endif
