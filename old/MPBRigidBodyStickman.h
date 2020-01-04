#ifndef __MPBRIGIDBODYSTICKMAN_H__
#define __MPBRIGIDBODYSTICKMAN_H__

#include "MPBLibrary.h"
#include "MPBRigidBody.h"

class MPBPhysicsParticle;

class MPBRigidBodyStickman: public MPBRigidBody
{
public:

	MPBRigidBodyStickman();
	virtual ~MPBRigidBodyStickman();

	virtual void setCorners( const MPBVector& NLL, const MPBVector& FUR );
	virtual void updateFrame();
	virtual void draw();

private:

protected:

};



#endif

