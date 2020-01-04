
#include "MPBLibrary.h"
#include "MPBCollision.h"
#include "MPBGlobal.h"
#include "MPBMessageWindow.h"
#include "MPBAABB.h"
#include "MPBPlane.h"
#include "MPBQuaternion.h"
#include "MPBPerformanceMonitor.h"
#include "MPBShape.h"
#include "MPBPhysics.h"
#include "MPBGameEntities.h"
#include "MPBPhysicsObject.h"
#include "mmgr.h"


const bool TRACE_SPHERE_PLANE = false;


//////////////////////////////////////////////////////////////

Collision::Collision(	MPBFLOAT distance, 
								const MPBVector& newDestination,
								const MPBVector& slideVector	):
	m_distance( distance ),
	m_newDestination( newDestination ),
	m_slideVector( slideVector ),
	m_targetObject( NULL ),
	m_targetPlane( NULL )
{
}

bool collisionLessThan( const Collision* lhs, const Collision* rhs )
{
	if (floatEQ( lhs->m_distance, rhs->m_distance ))
	{
		return floatLT( 
			rhs->m_slideVector.getLength(), lhs->m_slideVector.getLength() );
	}

	return floatLT( lhs->m_distance, rhs->m_distance );
}

bool collisionEquals( const Collision* lhs, const Collision* rhs )
{
	if (floatEQ( lhs->m_distance, rhs->m_distance ))
	{
		return true;
	}

	return false;
}





