#ifndef __MPBDOORWAY_H__
#define __MPBDOORWAY_H__


#include "MPBLibrary.h"
#include "MPBPlane.h"



class MPBDoorway: public Plane
{
public:

	MPBDoorway();
	MPBDoorway(	const MPBVector& LL,
					const MPBVector& LR,
					const MPBVector& UR,
					const MPBVector& UL	);
	MPBDoorway(	const MPBVector& LL,
					const MPBVector& LR,
					const MPBVector& UR	);
	virtual ~MPBDoorway();


protected:

};



#endif




