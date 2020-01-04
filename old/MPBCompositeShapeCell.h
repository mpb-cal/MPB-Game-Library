#ifndef __MPBCOMPOSITESHAPECELL_H__
#define __MPBCOMPOSITESHAPECELL_H__


#include "MPBCompositeShape.h"
#include "MPBList.h"


class MPBPortal;


class MPBCompositeShapeCell: public MPBCompositeShape
{
public:

	MPBCompositeShapeCell();
	~MPBCompositeShapeCell();

	void addShape( MPBShape* s );
	void addPortal( MPBPortal* p );
	void findPortalFrustums();
	bool aabbInPortalFrustum( bool isCurrentCell, const MPBAABB& aabb );
	bool sphereInPortalFrustum( bool isCurrentCell, const MPBVector& center, MPBFLOAT radius );
	GLuint getShadowVolumeDisplayList() { return m_displayListShadowVolume; }
	void setShadowVolumeDisplayList( GLuint i ) { m_displayListShadowVolume = i; }

	MPBList		portals;		// MPBPortal*

//protected:

	bool							m_processed;
	MPBPhysicsObject*			m_light;
	GLuint						m_displayListShadowVolume;
};





#endif

