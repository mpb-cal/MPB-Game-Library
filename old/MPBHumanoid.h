#ifndef __HUMANOID_H__
#define __HUMANOID_H__


#include "MPBCreature.h"

class MPBShapeBone;

class MPBHumanoid: public MPBCreature
{
public:

	MPBHumanoid( MPBVector radiusVector );
	virtual ~MPBHumanoid();

	virtual void updateFrame();

	void startWalkAnimation();
	void pauseWalkAnimation();
	void unpauseWalkAnimation();
	void stopWalkAnimation();

protected:

	MPBShapeBone*		m_chest;

	virtual void handleCollision1( Collision* c ) {}
	virtual void handleCollision2( Collision* c ) {}
};






#endif

