
#include "MPBDoorway.h"
#include "mmgr.h"



MPBDoorway::MPBDoorway():
	Plane()
{
}

MPBDoorway::MPBDoorway(	const MPBVector& LL,
								const MPBVector& LR,
								const MPBVector& UR,
								const MPBVector& UL	):
	Plane( LL, LR, UR, UL )
{
}

MPBDoorway::MPBDoorway(	const MPBVector& LL,
								const MPBVector& LR,
								const MPBVector& UR	):
	Plane( LL, LR, UR )
{
}

MPBDoorway::~MPBDoorway()
{
}


