
#include "MPBFlock.h"
#include "mmgr.h"


void MPBFlock::updateFrame()
{
	MPBVector averagePosition;
	MPBVector averageMoveVector;
	MPBVector target;

	if (m_target) target = m_target->getPosition();

	MPB_LIST_LOOP( m_npcs, npc, MPBNPC* )
	{
		averagePosition = averagePosition + npc->getCreature()->getPosition();
		averageMoveVector = averageMoveVector + npc->getCreature()->getMoveVector();
	}

	averagePosition = averagePosition / m_npcs.size();
	averageMoveVector = averageMoveVector / m_npcs.size();

	{MPB_LIST_LOOP( m_npcs, npc, MPBNPC* )
	{
		MPBVector averageAvoidance;

		MPB_LIST_LOOP( m_npcs, npc2, MPBNPC* )
		{
			if (npc2 == npc) continue;
			MPBVector v = npc2->getCreature()->getPosition() - npc->getCreature()->getPosition();
			MPBFLOAT dist = v.getLength();
			if (dist) v.setLength( 1.0 / dist );
			averageAvoidance = averageAvoidance + v;
		}

		averageAvoidance = averageAvoidance / (m_npcs.size() - 1);
		averageAvoidance = npc->getCreature()->getPosition() + averageAvoidance * -10;
		MPBVector alignment = npc->getCreature()->getPosition() + averageMoveVector * 2;
//		MPBVector goal = (averagePosition + averageAvoidance + alignment) / 3.0;
//		MPBVector goal = (target + averageAvoidance + alignment) / 3.0;
		MPBVector goal = (averagePosition + averageAvoidance) / 2.0;
//		MPBVector goal = (averagePosition + target) / 2.0;
//		MPBVector goal = alignment;
//		MPBVector goal = averagePosition;

		if (!npc->getCreature()->pathIsClear( goal, npc->getCreature()->getRadius(), false, NULL ))
		{
			MPBVector pathToGoal = goal - npc->getCreature()->getPosition();
			pathToGoal = pathToGoal * -1;
			goal = (goal + npc->getCreature()->getPosition() + pathToGoal) / 2.0;
		}

		npc->setCurrentGoal( goal );
	}}
}



