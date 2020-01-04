#ifndef __MPBRIGIDBODYBOX_H__
#define __MPBRIGIDBODYBOX_H__

#include "MPBLibrary.h"
#include "MPBRigidBody.h"

class MPBPhysicsParticle;

class MPBRigidBodyBox: public MPBRigidBody
{
public:

	MPBRigidBodyBox();
	virtual ~MPBRigidBodyBox();

	virtual void setCorners( const MPBVector& NLL, const MPBVector& FUR );
	virtual void updateFrame();
	virtual void draw() const;

private:

protected:

};



#endif

