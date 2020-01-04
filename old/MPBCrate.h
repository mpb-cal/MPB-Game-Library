#ifndef __MPBCRATE_H__
#define __MPBCRATE_H__

#include "MPBOpenGL.h"
#include "MPBPhysics.h"
#include "MPBParticleSystem.h"
#include "MPBStreamingSounds.h"


class MPBCountdown;
class MPBSoundBite;

class MPBCrate: public MPBPhysicsObject
{
public:

	MPBCrate::MPBCrate( MPBFLOAT radius );
	virtual MPBCrate::~MPBCrate();

	virtual void updateFrame();
	virtual void handleCollision1( Collision* c );
	virtual void handleCollision2( Collision* c );

protected:

	MPBCountdown*			m_countdownAlive;

};



#endif

