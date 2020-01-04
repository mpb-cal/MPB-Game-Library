
#include "MPBUsableObject.h"
#include "MPBCollision.h"
#include "MPBSoundManager.h"
#include "MPBHumanPlayer.h"
#include "MPBGameEntities.h"
#include "mmgr.h"



MPBUsableObject::MPBUsableObject( MPBFLOAT radius ):
	MPBPhysicsObject( POT_AABB )
{
	setRadius( radius );
	setGravity( 0 );
	setImmovable( true );
	setCollidable( false );

	setTexture( "font2" );

//	setStreamingSound( new MPBStreamingSound2 );
//	((MPBStreamingSound2*)getStreamingSound())->m_amplitude = .05f;
}

MPBUsableObject::~MPBUsableObject()
{
}

void MPBUsableObject::updateFrame()
{
	MPBPhysicsObject::updateFrame();

}


void MPBUsableObject::handleCollision1( Collision* c )
{
}

void MPBUsableObject::handleCollision2( Collision* c )
{
}

void MPBUsableObject::use( MPBHumanPlayer* user )
{
	user->takeUsableObject( this );
	MPBGameEntities::removeUsableObject( this );
	MPBSoundManager::playSound( "drip.wav", user->getPosition(), MPBVector(), 1 );
}




