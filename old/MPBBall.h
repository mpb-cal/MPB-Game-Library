#ifndef __MPBBALL_H__
#define __MPBBALL_H__

#include "MPBOpenGL.h"
#include "MPBPhysics.h"
#include "MPBParticleSystem.h"
#include "MPBStreamingSounds.h"


class MPBCountdown;
class MPBSoundBite;

class MPBBall: public MPBPhysicsObject
{
public:

	MPBBall::MPBBall( MPBFLOAT radius );
	virtual MPBBall::~MPBBall();

	virtual void updateFrame();
	virtual void handleCollision1( Collision* c );
	virtual void handleCollision2( Collision* c );

protected:

	MPBCountdown*			m_countdownAlive;

};



#endif

