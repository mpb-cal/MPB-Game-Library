
#include "MPBGlobal.h"
#include "MPBPortal.h"
#include "MPBOpenGL.h"
#include "MPBAABB.h"
#include "MPBCompositeShapeCell.h"
#include "mmgr.h"



MPBPortal::MPBPortal( Plane& plane, MPBCompositeShapeCell* toCell )
{
	m_plane = plane;
	m_toCell = toCell;
	m_open = true;
}


MPBPortal::~MPBPortal()
{
}

void MPBPortal::findFrustum()
{
	m_frustum.planes.clear();

	if (!m_open) return;

	// this test doesn't work when the camera is very close to the portal
	//if (!MPBOpenGL::planeInFrustum( m_plane )) return;

	MPBVector cameraPos = MPBOpenGL::getCameraPos();
	MPBVector lookAt = MPBOpenGL::getLookAt();

	MPBVector lookVector = lookAt - cameraPos;
	lookVector.normalize();

	// these tests don't work when the camera is very close to the portal
//	if (m_plane.getNormal().dotProduct( lookVector ) > 0) 
//		return;
//	if (m_plane.isPointInside( cameraPos )) return;

	MPBVector v1 = m_plane.getLL() - cameraPos;
	MPBVector v2 = m_plane.getLR() - cameraPos;
	MPBVector v3 = m_plane.getUR() - cameraPos;
	MPBVector v4 = m_plane.getUL() - cameraPos;

	MPBVector NLL, NLR, NUR, NUL, FLL, FLR, FUR, FUL;

	NLL = m_plane.getLL();
	NLR = m_plane.getLR();
	NUR = m_plane.getUR();
	NUL = m_plane.getUL();

	FLL = m_plane.getLL() + v1;
	FLR = m_plane.getLR() + v2;
	FUR = m_plane.getUR() + v3;
	FUL = m_plane.getUL() + v4;

	m_frustumPlanes[0].setCorners( NLL, FLL, FUL, NUL );
	m_frustumPlanes[1].setCorners( FLR, NLR, NUR, FUR );
	m_frustumPlanes[2].setCorners( FUL, FUR, NUR, NUL );
	m_frustumPlanes[3].setCorners( NLL, NLR, FLR, FLL );
	m_frustumPlanes[4].setCorners( NLR, NLL, NUL, NUR );

	if (m_toCell)
	{
		MPBAABB cellAABB = m_toCell->getAABB();
		cellAABB.grow( .1 );

		m_frustumPlanes[5].setCorners( 
			cellAABB.getNLR(), cellAABB.getNLL(), cellAABB.getNUL(), cellAABB.getNUR() );
		m_frustumPlanes[6].setCorners( 
			cellAABB.getFLL(), cellAABB.getFLR(), cellAABB.getFUR(), cellAABB.getFUL() );
		m_frustumPlanes[7].setCorners( 
			cellAABB.getNLL(), cellAABB.getFLL(), cellAABB.getFUL(), cellAABB.getNUL() );
		m_frustumPlanes[8].setCorners( 
			cellAABB.getFLR(), cellAABB.getNLR(), cellAABB.getNUR(), cellAABB.getFUR() );
		m_frustumPlanes[9].setCorners( 
			cellAABB.getFUL(), cellAABB.getFUR(), cellAABB.getNUR(), cellAABB.getNUL() );
		m_frustumPlanes[10].setCorners( 
			cellAABB.getNLL(), cellAABB.getNLR(), cellAABB.getFLR(), cellAABB.getFLL() );

		for (int i=0; i<MAX_FRUSTUM_PLANES; i++)
			m_frustum.planes.push_back( &m_frustumPlanes[i] );

		m_toCell->findPortalFrustums();
	}
	else
	{
		for (int i=0; i<MAX_FRUSTUM_PLANES - 6; i++)
			m_frustum.planes.push_back( &m_frustumPlanes[i] );
	}
}

bool MPBPortal::aabbInFrustum( const MPBAABB& aabb )
{
	if (m_frustum.planes.size() == 0) return false;
	if (aabbInPolyhedron( aabb, m_frustum )) return true;
	if (m_toCell) return m_toCell->aabbInPortalFrustum( false, aabb );

	return false;
}

bool MPBPortal::sphereInFrustum( const MPBVector& center, MPBFLOAT radius )
{
	if (m_frustum.planes.size() == 0) return false;
	if (sphereInPolyhedron( center, radius, m_frustum )) return true;
	if (m_toCell) return m_toCell->sphereInPortalFrustum( false, center, radius );

	return false;
}

