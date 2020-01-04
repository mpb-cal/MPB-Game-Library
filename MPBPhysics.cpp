
#include "MPBPhysics.h"
#include "MPBGlobal.h"
#include "MPBFrameCounter.h"
#include "MPBCollision.h"
#include "MPBGameEntities.h"
#include "MPBMessageWindow.h"
#include "MPBPlane.h"
#include "MPBOpenGL.h"
#include "MPBSoundManager.h"
#include "MPBShadow.h"
#include "MPBParticleSystem.h"
#include "MPBShape.h"
#include "MPBHeightMap.h"
#include "mmgr.h"


#pragma comment(lib, "ode.lib")

const MPBFLOAT SHADOW_LENGTH = 15.0;


MPBList				MPBPhysics::objects;
MPBList				MPBPhysics::newObjects;
MPBList				MPBPhysics::particleSystems;
MPBList				MPBPhysics::newParticleSystems;
bool					MPBPhysics::m_updating = false;

MPBFLOAT 			MPBPhysics::gravity = 9.8f;

void MPBPhysics::cleanUp()
{
	MPB_LIST_DELETE( objects, MPBPhysicsObject* );
	MPB_LIST_DELETE( newObjects, MPBPhysicsObject* );
	MPB_LIST_DELETE( particleSystems, MPBParticleSystem* );
	MPB_LIST_DELETE( newParticleSystems, MPBParticleSystem* );
}

void MPBPhysics::draw2D( HDC& hdc )
{
	MPB_LIST_LOOP( objects, po, MPBPhysicsObject* )
	{
//		po->draw2D( hdc );
	}
}

void MPBPhysics::draw( bool forShadowVolume )
{
	MPB_LIST_LOOP( objects, po, MPBPhysicsObject* )
	{
		po->draw( false, NULL, MPBVector(), forShadowVolume );
	}
}

void MPBPhysics::drawParticleSystems( bool solid )
{
	MPB_LIST_LOOP( particleSystems, ps, MPBParticleSystem* )
	{
		ps->draw( false, solid, NULL, MPBVector() );
	}
}

void MPBPhysics::createShadowVolumes( MPBVector lightPos )
{
	MPBList v;
	MPB_LIST_LOOP( objects, po, MPBPhysicsObject* ) { v.push_back( po ); }
	MPB_LIST_LOOP( particleSystems, ps, MPBParticleSystem* ) { v.push_back( ps ); }

	MPB_LIST_LOOP( v, o, MPBPhysicsObject* )
	{
		o->createShadowVolume( lightPos );
	}
}

void MPBPhysics::drawShadowVolumes( MPBVector lightPos )
{
	MPBList v;
	MPB_LIST_LOOP( objects, po, MPBPhysicsObject* ) { v.push_back( po ); }
	MPB_LIST_LOOP( particleSystems, ps, MPBParticleSystem* ) { v.push_back( ps ); }

	MPB_LIST_LOOP( v, o, MPBPhysicsObject* )
	{
		o->drawShadowVolume( lightPos );
	}
}

void MPBPhysics::updateFrame()
{
	// update all objects
	m_updating = true;

	MPB_LIST_LOOP( objects, o, MPBPhysicsObject* )
	{
		o->updateFrame();
	}

	{MPB_LIST_LOOP( particleSystems, o, MPBParticleSystem* )
	{
		o->updateFrame();
	}}

	m_updating = false;

	// add objects that were creating during update
	MPB_LIST_LOOP( newObjects, po, MPBPhysicsObject* )
	{
		addObject( po );
	}

	newObjects.clear();

	{MPB_LIST_LOOP( newParticleSystems, ps, MPBParticleSystem* )
	{
		addObject( ps );
	}}

	newParticleSystems.clear();

	// cull dead objects

	MPBList deadObjects;
	{MPB_LIST_LOOP( objects, po, MPBPhysicsObject* ) 
		if (!po->isAlive()) deadObjects.push_back( po );}
	{MPB_LIST_LOOP( deadObjects, po, MPBPhysicsObject* ) 
	{
		objects.remove( po );
		delete po;
	}}

	MPBList deadParticleSystems;
	{MPB_LIST_LOOP( particleSystems, po, MPBPhysicsObject* ) 
		if (!po->isAlive()) deadParticleSystems.push_back( po );}
	{MPB_LIST_LOOP( deadParticleSystems, po, MPBPhysicsObject* ) 
	{
		particleSystems.remove( po );
		delete po;
	}}
}

void MPBPhysics::addObject( MPBPhysicsObject* o )
{
	if (o->m_isParticleSystem)
	{
		if (m_updating)
			newParticleSystems.push_back( (MPBParticleSystem*)o );
		else
			particleSystems.push_back( (MPBParticleSystem*)o );
	}
	else
	{
		if (m_updating)
			newObjects.push_back( o );
		else
			objects.push_back( o );
	}
}

void MPBPhysics::removeObject( MPBPhysicsObject* o )
{
	if (o)
	{
		if (o->m_isParticleSystem)
		{
			particleSystems.remove( o );
		}
		else
		{
			objects.remove( o );
		}
	}
}






