#ifndef __SPRINGTRIANGLE_H__
#define __SPRINGTRIANGLE_H__


#include "MPBPhysics.h"
#include "MPBList.h"

class MPBSpringTriangle
{
public:

	MPBSpringTriangle();
	~MPBSpringTriangle();

	void draw();

	MPBList		m_objects;
};



#endif

