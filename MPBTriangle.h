#ifndef __MPB_TRIANGLE_H__
#define __MPB_TRIANGLE_H__

#include "MPBVector.h"

class MPBAABB;

class MPBTriangle
{
public:

	MPBTriangle();

	MPBTriangle(	const MPBVector& _v1,
						const MPBVector& _v2,
						const MPBVector& _v3	);

	virtual ~MPBTriangle();

	static void test();
	void toString( char* buf ) const;

	void setNormal( const MPBVector& normal );
	void setD( MPBFLOAT d );
	void setCorners(	const MPBVector& LL,
							const MPBVector& LR,
							const MPBVector& UR	);
	void setCollidable( bool set ) { m_collidable = set; }
	void setPolygonOffset( GLfloat factor, GLfloat units );

	const MPBTriangle& operator=( const MPBTriangle& rhs );
	bool lineInsideCorners(	const MPBVector& start, 
									const MPBVector& end, 
									MPBVector& intersection ) const;
	bool lineIntersect(	const MPBVector& start, 
								const MPBVector& end, 
								MPBVector& intersection ) const;
	bool rayIntersect(	const MPBVector& start, 
								const MPBVector& end, 
								MPBVector& intersection ) const;
	bool AABBIntersect( const MPBAABB& aabb ) const;
	MPBFLOAT distanceToPoint( MPBVector point ) const;
	bool isPointOutside( MPBVector point ) const;
	bool isPointInside( MPBVector point ) const;
	bool arePointsInside( MPBVector* pointList, int numPoints ) const;
	bool isSphereOutside( MPBVector center, MPBFLOAT radius ) const;
	bool isSphereInside( MPBVector center, MPBFLOAT radius ) const;
	void translate( MPBFLOAT x, MPBFLOAT y, MPBFLOAT z );
	void rotateX( MPBFLOAT degrees );
	void rotateY( MPBFLOAT degrees );
	void rotateZ( MPBFLOAT degrees );
	void scale( MPBFLOAT x, MPBFLOAT y, MPBFLOAT z );

	MPBVector		getNormal() const { return m_normal; }
	MPBFLOAT 		getD() const { return m_d; }
	MPBFLOAT 		getYAtPoint( MPBFLOAT x, MPBFLOAT z ) const;

	bool				isCollidable() const { return m_collidable; }

	virtual void	draw();
	void				drawVertices() const;
	void				drawVertices2() const;

	MPBVector		vertex[3];

protected:

	void findEquation();

	MPBVector		m_normal;
	MPBFLOAT 		m_d;
	bool				m_collidable;
	MPBFLOAT 			m_polygonOffsetFactor;
	MPBFLOAT 			m_polygonOffsetUnits;

};



#endif
