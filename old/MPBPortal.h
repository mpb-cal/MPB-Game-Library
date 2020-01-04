#ifndef __MPBPORTAL_H__
#define __MPBPORTAL_H__

#include "MPBPlane.h"
#include "MPBCollision.h"


class MPBCompositeShapeCell;

//const int MAX_PORTAL_FRUSTUMS = 10;
const int MAX_FRUSTUM_PLANES = 11;		// 5 for frustum, 6 for cell's AABB


class MPBPortal
{
public:

	MPBPortal( Plane& plane, MPBCompositeShapeCell* toCell );
	~MPBPortal();

	void findFrustum();
	bool aabbInFrustum( const MPBAABB& aabb );
	bool sphereInFrustum( const MPBVector& center, MPBFLOAT radius );

//protected:

	Plane								m_plane;
	MPBCompositeShapeCell*		m_toCell;
	Polyhedron						m_frustum;
	Plane								m_frustumPlanes[MAX_FRUSTUM_PLANES];
	bool								m_open;
};






#endif

