
#include "MPBNPC.h"
#include "MPBCollision.h"
#include "MPBPlane.h"
#include "MPBFrameCounter.h"
#include "MPBNavigationMesh.h"
#include "MPBOpenGL.h"
#include "mmgr.h"


const MPBFLOAT MAX_DAMAGE = 200000;
const int WAYPOINT_RADIUS = 1;
const int PLAYER_VISIBLE_COUNTDOWN = 500;
#define ENEMY_SPEED	((int)getMass() * 10)
const MPBFLOAT SPIN_RATE = .03;

MPBNPC::MPBNPC( MPBCreature* creature ):
	m_creature( creature ),
	m_targetVisible( false ),
	m_target( NULL ),
	m_intermediateGoal( -1 ),
	m_watchPoint( -1 ),
	m_thrust( 600000 ),
	m_inactive( false )
{
	m_countdownPlayerVisible = new MPBCountdown( PLAYER_VISIBLE_COUNTDOWN, true );
	m_countdownWandering = new MPBCountdown( 0, false );
	m_countdownScanning = new MPBCountdown( 0, false );
	m_creature->setLookRotation( MPBVector( 0, _2PI * RANDOM_FRACTION, 0 ) );
	m_thrust += 100000 * RANDOM_FRACTION;
}

MPBNPC::~MPBNPC()
{
	NULLIFY( m_countdownPlayerVisible );
	NULLIFY( m_countdownWandering );
	NULLIFY( m_countdownScanning );
}

void MPBNPC::addWatchPoint( MPBVector v )
{
	m_watchPoints.push_back( v );
}

// always use this method to change goal
// position must be set first
void MPBNPC::setCurrentGoal( MPBVector v ) 
{ 
	if (m_creature->pathIsClear( v, m_creature->getRadiusVector().x, false, NULL ))
	{
		m_intermediateGoals.clear();
		m_currentGoal = v;
		m_intermediateGoal = 0;
		m_intermediateGoals.push_back( v );
		return;
	}

	if (	!floatEQ( v.x, 0 ) &&
			!floatEQ( v.z, 0 ) &&
			floatEQ( m_currentGoal.x, v.x ) &&
			floatEQ( m_currentGoal.z, v.z )) return;

	m_currentGoal = v;
	m_intermediateGoal = -1;
	m_intermediateGoals.clear();

	if (MPBOpenGL::getNavMesh())
		if (MPBOpenGL::getNavMesh()->getASTAR( m_creature->getPosition(), m_currentGoal, m_intermediateGoals ))
			m_intermediateGoal = 0;
}

void MPBNPC::handleCollision1( Collision* c )
{
/*	bool turn = false;

	if (c->m_targetPlane)
	{
		MPBVector up( 0, 1, 0 );
		if (c->m_targetPlane->getNormal().dotProduct( up ) < .707)
			turn = true;
	}

	if (c->m_targetObject) turn = true;

	if (turn)
	{
		MPBVector look = getLookVector();
		look.y = 0;
//		look.rotateY( -90 );
		look.rotateY( rand() % 360 );
		look.setLength( 3 );
		setCurrentGoal( m_position + look );
	}
*/
}

void MPBNPC::updateFrame()
{
	if (m_inactive) 
	{
		m_creature->setDraw( false );
		m_creature->setCollidable( false );
		return;
	}
	else
	{
		m_creature->setDraw( true );
		m_creature->setCollidable( true );
	}

//	MPBCreature::updateFrame();


	if (m_creature->getDamage() > MAX_DAMAGE)
	{
		m_inactive = true;
	}

	if (m_targetVisible)
	{
//		if (rand() % 100 == 0) fireMissile();
	}

	if (m_target && m_countdownPlayerVisible->isDone())
	{
		m_countdownPlayerVisible->start();
		m_targetVisible = canSee( m_target );

		if (m_targetVisible)
		{
//			MPBVector playerPos = m_player->getPosition();
//			MPBVector toPlayer = playerPos - m_enemies[i]->getPosition();
//			MPBFLOAT distance = toPlayer.getLength();
//			MPBFLOAT y = (playerPos.y + sqrt( distance )) / 2.0;

			m_watchPoint = -1;
			setCurrentGoal( m_target->getPosition() );
		}
	}

	if (m_intermediateGoal != -1)
	{
		MPBVector desiredOrientation( m_intermediateGoals[m_intermediateGoal] - m_creature->getPosition() );
		if (desiredOrientation != MPBVector())
		{
			desiredOrientation.normalize();

			MPBFLOAT yRadians = desiredOrientation.getYRotation();
			desiredOrientation.rotateYRadians( -yRadians );
			MPBFLOAT xRadians = 0;//desiredOrientation.getXRotation();

			m_creature->setDesiredLookRotation( MPBVector( xRadians, yRadians, 0 ) );
	//		setMoveRotation( MPBVector( xRadians, yRadians, 0 ) );
		}
	}

	MPBVector pos;
	pos = m_creature->getPosition();
	pos.y = 0;
	MPBVector wp;

	// have we reached our current goal?
	wp = m_currentGoal;
	wp.y = 0;

	if ((pos - wp).getLength() < WAYPOINT_RADIUS)
	{
		if (m_watchPoints.size())
		{
			if (m_watchPoint == -1)
			{
				// just lost target; find closest watch point
				MPBFLOAT closest = 1000000;
				for (int i=0; i<m_watchPoints.size(); i++)
				{
					MPBFLOAT distance = (m_watchPoints[i] - m_creature->getPosition()).getLength();
					if (distance < closest)
					{
						closest = distance;
						m_watchPoint = i;
					}
				}
			}
			else
			{
				// making a circuit; find next watch point
				if (++m_watchPoint >= m_watchPoints.size()) m_watchPoint = 0;
			}

			setCurrentGoal( m_watchPoints[m_watchPoint] );
		}
		else
		{
			// spin randomly
//			setDesiredLookRotation( MPBVector( 0, _2PI * RANDOM_FRACTION, 0 ) );
		}
	}

	// have we reached our intermediate goal?
	if (m_intermediateGoal != -1)
	{
		wp = m_intermediateGoals[m_intermediateGoal];
		wp.y = 0;

		if ((pos - wp).getLength() < WAYPOINT_RADIUS) 
		{
			m_intermediateGoal++;
			if (m_intermediateGoal >= m_intermediateGoals.size())
				m_intermediateGoal = -1;
		}
	}

	MPBFLOAT distanceToTarget = 0;

	if (m_target)
		distanceToTarget = (m_target->getPosition() - m_creature->getPosition()).getLength();

	if (m_targetVisible && distanceToTarget < 3)
	{
	}
	else
	{
		if (m_creature->isGrounded()) 
			m_creature->applyForce( m_creature->getAxisVector( DIR_FORWARDS ) * m_thrust );
//			applyForce( getMoveVector() * m_thrust );
	}
}

bool MPBNPC::canSee( MPB3DObject* target )
{
//	return true;

	MPBVector v = target->getPosition() - m_creature->getPosition();
	v.normalize();
//	if (getLookVector().dotProduct( v ) < 0)
//		return false;

	const MPBFLOAT lineOfSightRadius = .01;

	return m_creature->pathIsClear(
		target->getPosition(),
		lineOfSightRadius,
		true,
		target
	);
}



