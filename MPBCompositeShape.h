#ifndef __MPBCOMPOSITESHAPE_H__
#define __MPBCOMPOSITESHAPE_H__


#include "MPBGlobal.h"
#include "MPBAABB.h"
#include "MPBList.h"


class MPBShape;
class MPBCompositeShape;


class MPBCompositeShape
{
public:

	MPBCompositeShape();
	virtual ~MPBCompositeShape();

	virtual void addShape( MPBShape* s );
	void addCompositeShape( MPBCompositeShape* s );
	void getShapes( MPBList& l );
	MPBShape* getShape();
	MPBShape* getShape( int i );
	const MPBAABB& getAABB() { return m_aabb; }

	// transform shapes
	void translate( MPBFLOAT x, MPBFLOAT y, MPBFLOAT z );
	void rotateX( MPBFLOAT degrees );
	void rotateY( MPBFLOAT degrees );
	void rotateZ( MPBFLOAT degrees );
	void scale( MPBFLOAT x, MPBFLOAT y, MPBFLOAT z );

protected:

	MPBList					m_shapes;
	MPBAABB					m_aabb;

	void findAABB();
};





#endif

