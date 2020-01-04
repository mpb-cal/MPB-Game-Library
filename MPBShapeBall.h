#ifndef __SHAPEBALL_H__
#define __SHAPEBALL_H__

#include "MPBLibrary.h"
#include "MPBShape.h"


class MPBShapeBall: public MPBShape
{
public:

	MPBShapeBall(	MPBFLOAT radius, 
						int slices, 
						int stacks,
						const char* texture,
						bool oneTexturePerPlane	);

	virtual ~MPBShapeBall();

private:

};


#endif
