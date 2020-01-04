
#include "MPBPhysics.h"
//#include "MPBPhysicsParticle.h"
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
//#include "MPBHardSpring.h"
#include "MPBHeightMap.h"
#include "MPBRigidBody.h"
#include "MPBProfile.h"
#include "MPBNavigationMesh.h"
#include "mmgr.h"


#pragma comment(lib, "ode.lib")

const MPBFLOAT SHADOW_LENGTH = 15.0;


MPBList				MPBPhysics::objects;
MPBList				MPBPhysics::newObjects;
MPBList				MPBPhysics::particleSystems;
MPBList				MPBPhysics::newParticleSystems;
MPBList				MPBPhysics::rigidbodies;
MPBList				MPBPhysics::particles;
bool					MPBPhysics::m_updating = false;

MPBFLOAT 			MPBPhysics::gravity = 9.8f;

void MPBPhysics::cleanUp()
{
	MPB_LIST_DELETE( objects, MPBPhysicsObject* );
	MPB_LIST_DELETE( newObjects, MPBPhysicsObject* );
	MPB_LIST_DELETE( particleSystems, MPBParticleSystem* );
	MPB_LIST_DELETE( newParticleSystems, MPBParticleSystem* );
	MPB_LIST_DELETE( rigidbodies, MPBRigidBody* );
	MPB_LIST_DELETE( particles, MPBPhysicsParticle* );
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

	MPB_LIST_LOOP( particles, pp, MPBPhysicsParticle* )
	{
		pp->draw();
	}

	MPB_LIST_LOOP( rigidbodies, rb, MPBRigidBody* )
	{
		glPushMatrix();
		rb->draw();
		glPopMatrix();
	}
}

void MPBPhysics::drawParticleSystems( bool solid )
{
	MPB_LIST_LOOP( particleSystems, ps, MPBParticleSystem* )
	{
		ps->draw( false, solid, NULL, MPBVector() );
	}
}

void MPBPhysics::drawProjectionShadows( MPBVector lightPos )
{
	MPBShadow::projectionShadowBegin();

	MPBList v;
	MPB_LIST_LOOP( objects, po, MPBPhysicsObject* ) { v.push_back( po ); }
	MPB_LIST_LOOP( particleSystems, ps, MPBParticleSystem* ) { v.push_back( ps ); }

	MPBFLOAT polyOffset = -2.0;
	{MPB_LIST_LOOP( v, po, MPBPhysicsObject* )
	{
		if (!po->getCastsShadow()) continue;

		MPBVector shadowVector = po->getPosition() - lightPos;
		shadowVector.normalize();
		shadowVector.setLength( SHADOW_LENGTH );

		// find planes that receive this object's shadow
		MPBVector extents( po->getRadius(), po->getRadius(), po->getRadius() );

		MPBAABB startBox;
		startBox.setCenterAndExtents( po->getPosition(), extents );

		MPBAABB destBox;
		destBox.setCenterAndExtents( po->getPosition() + shadowVector, extents );

		MPBAABB moveBox;
		moveBox.bound( startBox, destBox );

		MPBList shadowReceivers;
		MPBGameEntities::getCollisionPlanes( shadowReceivers, moveBox );

		glPolygonOffset( polyOffset, polyOffset );
		polyOffset += .1f;

		int index = 1;
		MPB_LIST_LOOP( shadowReceivers, receiver, Plane* )
		{
			if (receiver->isPointOutside( po->getPosition() ))
			{
				MPBShadow::projectionShadowPrepReceiver( receiver, index++ );
				po->draw( true, receiver, lightPos, false );
			}
		}
	}}

	MPBShadow::projectionShadowEnd();
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
	PROFILE_START( "MPBPhysics::updateFrame" )

	// update all objects
	m_updating = true;

	MPB_LIST_LOOP( rigidbodies, rb, MPBRigidBody* )
	{
		rb->updateFrame();
	}

	MPB_LIST_LOOP( particles, ps, MPBPhysicsParticle* )
	{
		ps->updateFrame1();
	}

	for (int i=0; i<10; i++)
	{
		MPB_LIST_LOOP( particles, pp, MPBPhysicsParticle* )
		{
			pp->updateFrame2();
		}
	}

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

void MPBPhysics::addRigidBody( MPBRigidBody* o )
{
	rigidbodies.push_back( o );
}

void MPBPhysics::addParticle( MPBPhysicsParticle* o )
{
	particles.push_back( o );
}

void MPBPhysics::removeParticle( MPBPhysicsParticle* o )
{
	if (o) particles.remove( o );
}





