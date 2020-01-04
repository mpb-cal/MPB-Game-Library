
#include "MPBGlobal.h"
#include "MPBOpenGL.h"
#include "MPBShape.h"
#include "MPBShapeBox.h"
#include "MPBShapeFrustum.h"
#include "MPBCompositeShapeCell.h"
//#include "MPBPortal.h"
#include "mmgr.h"



MPBCompositeShapeCell::MPBCompositeShapeCell():
	MPBCompositeShape(),
	m_processed( false ),
	m_light( NULL )
{
}

MPBCompositeShapeCell::~MPBCompositeShapeCell()
{
	MPB_LIST_DELETE( portals, MPBPortal* );
}

void MPBCompositeShapeCell::addShape( MPBShape* s )
{
	MPBCompositeShape::addShape( s );
	s->setCell( this );
}

void MPBCompositeShapeCell::addPortal( MPBPortal* p )
{
	portals.push_back( p );
}

void MPBCompositeShapeCell::findPortalFrustums()
{
	if (m_processed) return;
	m_processed = true;

	MPB_LIST_LOOP( portals, portal, MPBPortal* )
	{
		portal->findFrustum();
	}
}

bool MPBCompositeShapeCell::aabbInPortalFrustum( bool isCurrentCell, const MPBAABB& aabb )
{
	if (m_processed) return false;
	m_processed = true;

	if (isCurrentCell && getAABB().intersects( aabb )) return true;

	MPB_LIST_LOOP( portals, portal, MPBPortal* )
	{
		if (portal->aabbInFrustum( aabb )) return true;
	}

	return false;
}

bool MPBCompositeShapeCell::sphereInPortalFrustum( bool isCurrentCell, const MPBVector& center, MPBFLOAT radius )
{
	if (m_processed) return false;
	m_processed = true;

	if (isCurrentCell && getAABB().intersectsSphere( center, radius )) return true;

	MPB_LIST_LOOP( portals, portal, MPBPortal* )
	{
		if (portal->sphereInFrustum( center, radius )) return true;
	}

	return false;
}






