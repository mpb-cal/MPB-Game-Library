#ifndef __CREATURE_H__
#define __CREATURE_H__


#include "MPBPhysicsObject.h"

class MPBVector;

class MPBCreature: public MPBPhysicsObject
{
public:

	MPBCreature( MPBVector radiusVector );
	virtual ~MPBCreature();

	virtual void updateFrame();
	MPBVector getLookVector() const { return m_lookVector; }
	MPBVector getLookRotation() const { return m_lookRotation; }
	MPBVector getEyeball() const;
	void setLookRotation( MPBVector rotation );
	void rotateLook( MPBVector rotation );
	void setDesiredLookRotation( MPBVector rotation );
	void setMoveRotation( MPBVector rotation );
	MPBVector getMoveVector() const { return m_moveVector; }
	MPBVector getMoveRotation() const { return m_moveRotation; }
	virtual void setDamage( MPBFLOAT s ) { MPBPhysicsObject::setDamage( s ); }

protected:

	MPBVector			m_lookRotation;				// in radians
	MPBVector			m_desiredLookRotation;		// in radians
	MPBVector			m_lookVector;
	MPBVector			m_moveVector;
	MPBVector			m_moveRotation;				// in radians

	void updateLookVector();
	void updateMoveVector();
	virtual void handleCollision1( Collision* c ) {}
	virtual void handleCollision2( Collision* c ) {}
};






#endif

