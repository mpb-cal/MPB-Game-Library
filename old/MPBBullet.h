#ifndef __MPBBULLET_H__
#define __MPBBULLET_H__

#include "MPBOpenGL.h"
#include "MPBPhysics.h"
#include "MPBParticleSystem.h"
#include "MPBStreamingSounds.h"


class MPBCountdown;
class MPBSoundBite;

class MPBBullet: public MPBPhysicsObject
{
public:

	MPBBullet::MPBBullet( MPBFLOAT radius );
	virtual MPBBullet::~MPBBullet();

	virtual void updateFrame();
	virtual void handleCollision1( Collision* c );
	virtual void handleCollision2( Collision* c );

protected:

	MPBCountdown*			m_countdownAlive;

};



#endif

