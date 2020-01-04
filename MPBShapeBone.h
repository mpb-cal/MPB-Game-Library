#ifndef __MPBSHAPEBONE_H__
#define __MPBSHAPEBONE_H__

#include "MPBLibrary.h"
#include "MPBShape.h"


class MPBShapeBone: public MPBShape
{
public:

	MPBShapeBone( MPBFLOAT length );
	MPBShapeBone( MPBShape* parent, MPBFLOAT length, const char* texture );
	MPBShapeBone( MPBShape* parent, MPBFLOAT length, MPBFLOAT radius, const char* texture );
	virtual ~MPBShapeBone();

protected:

};


#endif
