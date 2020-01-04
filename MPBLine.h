#ifndef __MPBLINE_H__
#define __MPBLINE_H__

#include "MPBGlobal.h"
#include "MPBVector.h"


class MPBLine
{
public:

	MPBLine(	const MPBVector& v1,
				const MPBVector& v2	);

	virtual ~MPBLine();
	void draw();


protected:

	MPBVector		m_v1;
	MPBVector		m_v2;

	MPBFLOAT 		m_red;
	MPBFLOAT 		m_green;
	MPBFLOAT 		m_blue;
};



#endif
