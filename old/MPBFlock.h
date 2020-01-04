#ifndef __FLOCK_H__
#define __FLOCK_H__


#include "MPBLibrary.h"
#include "MPBPhysics.h"
#include "MPBNPC.h"
#include "MPBList.h"


class MPBFlock
{
public:

	MPBFlock::MPBFlock() :
		m_target( NULL )
	{}

	MPBFlock::~MPBFlock() {}

	void addNPC( MPBNPC* o ) { m_npcs.push_back( o ); }
	void updateFrame();

	MPBPhysicsObject*		m_target;

protected:

	MPBList			m_npcs;	// MPBNPC*
};



#endif
