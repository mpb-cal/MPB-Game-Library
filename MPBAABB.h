#ifndef __MPBAABB_H__
#define __MPBAABB_H__

#include "MPBVector.h"

class MPBAABB
{
public:

	MPBAABB();
	MPBAABB( const MPBVector& NLL, const MPBVector& FUR );
	~MPBAABB();

	const MPBAABB& operator=( const MPBAABB& rhs );
	virtual void setCorners( const MPBVector& NLL, const MPBVector& FUR );
	virtual void setCenter( const MPBVector& center );
	virtual void setCenterAndExtents( const MPBVector& center, const MPBVector& extents );
	bool intersects( const MPBAABB& rhs ) const;
	bool intersectsSphere( const MPBVector& center, MPBFLOAT radius ) const;
//	bool isInFrustum() const;
	virtual void draw() const;
	void drawLines() const;
	void scale( MPBFLOAT x, MPBFLOAT y, MPBFLOAT z );
	void grow( MPBFLOAT d );
	void getVertices( MPBFLOAT vertices[8][3] ) const;
	void bound( const MPBAABB& box1, const MPBAABB& box2 );
	
	MPBVector getNLL() const { return m_NLL; }
	MPBVector getNLR() const { return m_NLR; }
	MPBVector getNUR() const { return m_NUR; }
	MPBVector getNUL() const { return m_NUL; }

	MPBVector getFLL() const { return m_FLL; }
	MPBVector getFLR() const { return m_FLR; }
	MPBVector getFUR() const { return m_FUR; }
	MPBVector getFUL() const { return m_FUL; }
	
	MPBVector			getCenter() const { return m_center; }
	MPBFLOAT 			getExtentX() const { return m_extentX; }
	MPBFLOAT 			getExtentY() const{ return m_extentY; }
	MPBFLOAT 			getExtentZ() const { return m_extentZ; }

	MPBFLOAT 			getWidth() const { return m_width; }
	MPBFLOAT 			getHeight() const { return m_height; }
	MPBFLOAT 			getDepth() const { return m_depth; }
	void					getPlanes( MPBList& planes ) const;

protected:

	MPBVector			m_NLL;
	MPBVector			m_NLR;
	MPBVector			m_NUR;
	MPBVector			m_NUL;
	
	MPBVector			m_FLL;
	MPBVector			m_FLR;
	MPBVector			m_FUR;
	MPBVector			m_FUL;

	MPBVector			m_center;
	MPBFLOAT 			m_extentX;
	MPBFLOAT 			m_extentY;
	MPBFLOAT 			m_extentZ;

	MPBFLOAT 			m_width;
	MPBFLOAT 			m_height;
	MPBFLOAT 			m_depth;
};






#endif

