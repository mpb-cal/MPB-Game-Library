#ifndef __MPBRIGIDBODY_H__
#define __MPBRIGIDBODY_H__

#include "MPBLibrary.h"
#include "MPBAABB.h"
#include "MPBMatrix.h"
#include "MPBList.h"

class MPBPhysicsParticle;
class MPBShape;

class MPBRigidBody: public MPBAABB
{
public:

	MPBRigidBody();
	virtual ~MPBRigidBody();

	virtual void setCorners( const MPBVector& NLL, const MPBVector& FUR );
	virtual void updateFrame();
	virtual void draw();
	MPBPhysicsParticle* getParticle( int i ) { return (MPBPhysicsParticle*)m_particles[i]; }
	void addShape( MPBShape* s );
	MPBMatrix getRotationMatrix() const { return m_rotationMatrix; }
	MPBMatrix getTranslationMatrix() const { return m_translationMatrix; }

private:

protected:

	MPBList			m_particles;		// MPBPhysicsParticle*
	MPBList			m_shapes;		// MPBShape*

	MPBMatrix		m_rotationMatrix;
	MPBMatrix		m_translationMatrix;
};



#endif

