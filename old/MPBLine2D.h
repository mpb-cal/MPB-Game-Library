#ifndef __MPBLINE2D_H__
#define __MPBLINE2D_H__

#include "MPBGlobal.h"
#include "MPBVector2D.h"


class MPBLine2D
{
public:

	MPBLine2D();
	MPBLine2D(	const MPBVector2D& v1,
					const MPBVector2D& v2	);

	virtual ~MPBLine2D();

	void setPoints(	const MPBVector2D& v1,
							const MPBVector2D& v2	);
	bool pointInside( const MPBVector2D& point ) const;
	MPBFLOAT distanceToPoint( const MPBVector2D& point ) const;
	bool intersectLine( const MPBLine2D& line, MPBVector2D& intersection ) const;

//protected:

	MPBVector2D		m_v1;
	MPBVector2D		m_v2;
	MPBVector2D		m_line;
	MPBVector2D		m_normal;
};



#endif
