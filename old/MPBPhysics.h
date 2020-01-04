#ifndef __MPBPHYSICS_H__
#define __MPBPHYSICS_H__

#include "MPBLibrary.h"
#include "MPBList.h"


//class Plane;
//class Collision;
//class MPBParticleSystem;
//class MPBHardSpring;
class MPBRigidBody;
class MPBPhysicsObject;
class MPBPhysicsParticle;



class MPBPhysics
{
public:

	static void cleanUp();
	static void updateFrame();
	static void addObject( MPBPhysicsObject* o );
	static void removeObject( MPBPhysicsObject* o );
	static void addRigidBody( MPBRigidBody* o );
	static void addParticle( MPBPhysicsParticle* o );
	static void removeParticle( MPBPhysicsParticle* o );
	static void draw2D( HDC& hdc );
	static void draw( bool forShadowVolume );
	static void drawParticleSystems( bool solid );
	static void drawProjectionShadows( MPBVector lightPos );
	static void setGravity( MPBFLOAT metersPerSecondSquared ) { gravity = metersPerSecondSquared; }
	static void drawShadowVolumes( MPBVector lightPos );
	static void createShadowVolumes( MPBVector lightPos );

	static MPBList				objects;
	static MPBList				newObjects;
	static MPBList				particleSystems;
	static MPBList				newParticleSystems;
	static MPBList				rigidbodies;
	static MPBList				particles;

	static MPBFLOAT 	gravity;

private:

	static bool			m_updating;
};




#endif

