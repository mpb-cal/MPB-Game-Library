#ifndef __BOID_H__
#define __BOID_H__


#include "MPBCreature.h"


class MPBBoid: public MPBCreature
{
public:

	MPBBoid( MPBVector radiusVector );
	virtual ~MPBBoid();

protected:


	virtual void handleCollision1( Collision* c ) {}
	virtual void handleCollision2( Collision* c ) {}
};






#endif

