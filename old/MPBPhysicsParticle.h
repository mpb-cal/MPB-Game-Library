#ifndef __MPBPHYSICSPARTICLE_H__
#define __MPBPHYSICSPARTICLE_H__

#include "MPBLibrary.h"
#include "MPBShape.h"
#include "MPBMatrix.h"
#include "MPB3DObject.h"
#include "MPBList.h"

class MPBPhysics;
class Plane;
class Collision;
class MPBParticleSystem;
class MPBHardSpring;
class MPBHardSpringAngular;
class MPBRigidBody;
class MPBPlaneSpring;

class MPBPhysicsParticle: public MPB3DObject
{
public:

	MPBPhysicsParticle( MPBVector position );
	virtual ~MPBPhysicsParticle();

	virtual void updateFrame1();
	virtual void updateFrame2();
	virtual void setCollisionTest( bool set ) { m_collisionTest = set; }
	virtual void draw();
	virtual void addSpring( MPBHardSpring* s ) ;
	virtual void addSpring( MPBPhysicsParticle* o );
	virtual void addSpring( MPBPhysicsParticle* o, MPBFLOAT restLength );
	virtual void addSpring( MPBPhysicsParticle* o, MPBFLOAT restLengthMin, MPBFLOAT restLengthMax );
	virtual void addSpringAngular( MPBHardSpringAngular* s );
	virtual void addPlaneSpring( MPBPlaneSpring* s );
	virtual void removeSprings();
	virtual void setRotationMatrix( MPBMatrix m ) { m_rotationMatrix = m; }
	virtual void setRotationMatrix2( MPBMatrix m ) { m_rotationMatrix2 = m; }
	virtual MPBMatrix getRotationMatrix() { return m_rotationMatrix * m_rotationMatrix2; }
	virtual MPBVector getAxisVector( DirectionType direction ) const;

protected:

	bool											m_collisionTest;
	MPBList										m_springs;
	MPBList										m_springAngulars;
	MPBList										m_planeSprings;
	MPBMatrix									m_rotationMatrix;
	MPBMatrix									m_rotationMatrix2;
	MPBVector									m_penetration;
};




#endif

