#ifndef __MPBMISSILE_H__
#define __MPBMISSILE_H__

#include "MPBOpenGL.h"
#include "MPBPhysics.h"
#include "MPBParticleSystem.h"
#include "MPBStreamingSounds.h"


class MPBCountdown;
class MPBSoundBite;

class MPBMissile: public MPBParticleSystem
{
public:

	MPBMissile::MPBMissile( MPBFLOAT radius );
	virtual MPBMissile::~MPBMissile();

	virtual void updateFrame();
	virtual void handleCollision1( Collision* c );

protected:

	MPBCountdown*			m_countdownAlive;
	bool						m_grounded;
	bool						m_exploded;

	void explode( bool causedByTimeout );
};



#endif

