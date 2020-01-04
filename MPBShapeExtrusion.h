#ifndef __SHAPEEXTRUSION_H__
#define __SHAPEEXTRUSION_H__

#include "MPBLibrary.h"
#include "MPBShape.h"

class Plane;

class MPBShapeExtrusion: public MPBShape
{
public:

	MPBShapeExtrusion(	Plane* plane, 
								MPBVector extrusion,
								const char* texture, 
								MPBFLOAT texSizeX, 
								MPBFLOAT texSizeY	);

	virtual ~MPBShapeExtrusion();

private:

	char  			m_texture[MAX_PATH];

	MPBFLOAT 			m_texCoordX;
	MPBFLOAT 			m_texCoordY;
};


#endif
