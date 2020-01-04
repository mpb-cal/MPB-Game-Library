#ifndef __HUMANPLAYER_H__
#define __HUMANPLAYER_H__


#include "MPBPhysics.h"
#include "MPBHumanoid.h"
#include "MPBCountdown.h"
#include "MPBList.h"


class MPBUsableObject;


class MPBHumanPlayer: public MPBHumanoid
{
public:

	MPBHumanPlayer( MPBVector radiusVector );
	~MPBHumanPlayer();

	virtual void setDamage( MPBFLOAT s ) { MPBHumanoid::setDamage( s ); }
	virtual void fireMissile();
	virtual void takeUsableObject( MPBUsableObject* o );
	bool hasUsableObject( MPBUsableObject* o );

protected:

	virtual void handleCollision1( Collision* c ) {}
	virtual void handleCollision2( Collision* c ) {}

	MPBCountdown*						m_countdownMissile;
	MPBList								m_usableObjects;	// MPBUsableObject*

};






#endif

