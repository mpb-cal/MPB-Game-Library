#ifndef __MPBPHYSICS_H__
#define __MPBPHYSICS_H__

#include "MPBLibrary.h"
#include "MPBList.h"


class MPBPhysicsObject;



class MPBPhysics
{
public:

	static void cleanUp();
	static void updateFrame();
	static void addObject( MPBPhysicsObject* o );
	static void removeObject( MPBPhysicsObject* o );
	static void draw2D( HDC& hdc );
	static void draw( bool forShadowVolume );
	static void drawParticleSystems( bool solid );
	static void setGravity( MPBFLOAT metersPerSecondSquared ) { gravity = metersPerSecondSquared; }
	static void drawShadowVolumes( MPBVector lightPos );
	static void createShadowVolumes( MPBVector lightPos );

	static MPBList				objects;
	static MPBList				newObjects;
	static MPBList				particleSystems;
	static MPBList				newParticleSystems;

	static MPBFLOAT 	gravity;

private:

	static bool			m_updating;
};




#endif

