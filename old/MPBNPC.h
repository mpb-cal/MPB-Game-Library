#ifndef __MPBNPC_H__
#define __MPBNPC_H__

#include "MPBCreature.h"
#include "MPBCountdown.h"
#include "MPBList.h"
#include "MPBVectorList.h"

enum SpinType
{
	SPIN_NONE,
	SPIN_LEFT,
	SPIN_RIGHT
};

class MPBNPC
{
public:

	MPBNPC( MPBCreature* creature );
	~MPBNPC();

	MPBCreature* getCreature() { return m_creature; }
	void setCurrentGoal( MPBVector v );
	void addWatchPoint( MPBVector v );
	MPBVector getWatchPoint( int i ) { return m_watchPoints[i]; }
	virtual void updateFrame();
	virtual void handleCollision1( Collision* c );
	void setTarget( MPBPhysicsObject* o ) { m_target = o; }
	bool isTargetVisible() { return m_targetVisible; }

	MPBFLOAT 			m_thrust;
	bool					m_inactive;

private:

	bool canSee( MPB3DObject* target );

	MPBCreature*		m_creature;
	MPBCountdown*		m_countdownPlayerVisible;
	MPBCountdown*		m_countdownWandering;
	MPBCountdown*		m_countdownScanning;
	bool					m_targetVisible;
	MPBVector			m_currentGoal;
	MPBVectorList		m_intermediateGoals;
	int					m_intermediateGoal;
	MPBVectorList		m_watchPoints;
	int					m_watchPoint;
	MPBPhysicsObject*	m_target;

};



#endif

