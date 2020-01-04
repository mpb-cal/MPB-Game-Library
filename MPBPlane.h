#ifndef __MPBPLANE_H__
#define __MPBPLANE_H__

#include "MPBGlobal.h"
#include "MPBVector.h"

class MPBAABB;
class MPBTriangle;


class Plane
{
public:

	Plane();

	Plane(	const MPBVector& LL,
				const MPBVector& LR,
				const MPBVector& UR,
				const MPBVector& UL	);

	Plane(	const MPBVector& LL,
				const MPBVector& LR,
				const MPBVector& UR	);

	virtual ~Plane();

	static void test();
	void toString( char* buf ) const;

	void setNormal( const MPBVector& normal );
	void setD( MPBFLOAT d );
	void setCorners(	const MPBVector& LL,
							const MPBVector& LR,
							const MPBVector& UR,
							const MPBVector& UL	);
	void setCorners(	const MPBVector& LL,
							const MPBVector& LR,
							const MPBVector& UR	);
	const Plane& operator=( const Plane& rhs );
	MPBFLOAT getYAtPoint( MPBFLOAT x, MPBFLOAT z ) const;
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
	void rotateMatrix( MPBMatrix& m );
	void scale( MPBFLOAT x, MPBFLOAT y, MPBFLOAT z );
	void flipHorizontal();
	MPBVector		getLL() const { return m_LL; }
	MPBVector		getLR() const { return m_LR; }
	MPBVector		getUR() const { return m_UR; }
	MPBVector		getUL() const { return m_UL; }
	MPBVector		getNormal() const { return m_normal; }
	MPBVector		getCenter() const { return m_center; }
	MPBFLOAT 		getWidth() const { return m_width; }
	MPBFLOAT 		getHeight() const { return m_height; }
	MPBFLOAT 		getD() const { return m_d; }
	MPBFLOAT 		getArea() const { return m_area; }
	void				draw2D( HDC hdc );
	virtual void	draw();
	void				drawVertices() const;
	void				drawVertices2() const;
	void				setColor( MPBFLOAT r, MPBFLOAT g, MPBFLOAT b, MPBFLOAT a ) { m_red = r; m_green = g; m_blue = b; m_alpha = a; }
	void				setCollidable( bool set ) { m_collidable = set; }
	bool				isCollidable() const { return m_collidable; }
	void				setPolygonOffset( GLfloat factor, GLfloat units );
	void				getTriangles( MPBTriangle& t1, MPBTriangle& t2 );

protected:

	void findEquation();

	MPBVector		m_LL;
	MPBVector		m_LR;
	MPBVector		m_UR;
	MPBVector		m_UL;
	MPBVector		m_normal;
	MPBVector		m_center;
	MPBFLOAT 		m_d;
	MPBFLOAT 		m_area;
	MPBFLOAT 		m_width;
	MPBFLOAT 		m_height;
	MPBFLOAT 		m_red;
	MPBFLOAT 		m_green;
	MPBFLOAT 		m_blue;
	MPBFLOAT 		m_alpha;
	bool				m_collidable;
	MPBFLOAT 		m_polygonOffsetFactor;
	MPBFLOAT 		m_polygonOffsetUnits;
};



#endif
