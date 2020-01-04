#ifndef __MPB_USABLE_OBJECT_H__
#define __MPB_USABLE_OBJECT_H__

#include "MPBOpenGL.h"
#include "MPBPhysics.h"
#include "MPBParticleSystem.h"
#include "MPBStreamingSounds.h"

class MPBHumanPlayer;


class MPBUsableObject: public MPBPhysicsObject
{
public:

	MPBUsableObject::MPBUsableObject( MPBFLOAT radius );
	virtual MPBUsableObject::~MPBUsableObject();

	virtual void updateFrame();
	virtual void handleCollision1( Collision* c );
	virtual void handleCollision2( Collision* c );
	virtual void use( MPBHumanPlayer* user );

protected:

};



#endif

