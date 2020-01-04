
#include "MPBGlobal.h"
#include "MPBShape.h"
#include "MPBCompositeShape.h"
#include "MPBTexturedPlane.h"
#include "mmgr.h"


MPBCompositeShape::MPBCompositeShape()
{
}

MPBCompositeShape::~MPBCompositeShape()
{
}

void MPBCompositeShape::findAABB()
{
	MPBList planes;

	MPB_LIST_LOOP( m_shapes, shape, MPBShape* )
	{
		MPB_LIST_LOOP( shape->getPlanes(), p, MPBTexturedPlane* )
		{
			planes.push_back( p );
		}
	}

	if (planes.size())
	{
		MPBVector LL = ((MPBTexturedPlane*)planes[0])->getLL();
		MPBVector LR;
		MPBVector UR;
		MPBVector UL;

		MPBVector NLL( LL.x, LL.y, LL.z );
		MPBVector FUR( LL.x, LL.y, LL.z );

		MPB_LIST_LOOP( planes, p, MPBTexturedPlane* )
		{
			LL = ((MPBTexturedPlane*)p)->getLL();
			LR = ((MPBTexturedPlane*)p)->getLR();
			UR = ((MPBTexturedPlane*)p)->getUR();
			UL = ((MPBTexturedPlane*)p)->getUL();

			NLL.x = min( 
				NLL.x, 
				min(
					min( LL.x, LR.x ),
					min( UR.x, UL.x )
				)
			);

			NLL.y = min( 
				NLL.y, 
				min(
					min( LL.y, LR.y ),
					min( UR.y, UL.y )
				)
			);

			NLL.z = max( 
				NLL.z, 
				max(
					max( LL.z, LR.z ),
					max( UR.z, UL.z )
				)
			);

			FUR.x = max( 
				FUR.x, 
				max(
					max( LL.x, LR.x ),
					max( UR.x, UL.x )
				)
			);

			FUR.y = max( 
				FUR.y, 
				max(
					max( LL.y, LR.y ),
					max( UR.y, UL.y )
				)
			);

			FUR.z = min( 
				FUR.z, 
				min(
					min( LL.z, LR.z ),
					min( UR.z, UL.z )
				)
			);
		}

		if (NLL.x == FUR.x) FUR.x += EPSILON;
		if (NLL.y == FUR.y) FUR.y += EPSILON;
		if (NLL.z == FUR.z) FUR.z -= EPSILON;

		m_aabb.setCorners( NLL, FUR );
		m_aabb.grow( .1 );
	}
}

void MPBCompositeShape::addShape( MPBShape* s )
{
	m_shapes.push_back( s );
	findAABB();
}

void MPBCompositeShape::addCompositeShape( MPBCompositeShape* s )
{
	MPBList l;
	s->getShapes( l );

	MPB_LIST_LOOP( l, shape, MPBShape* )
	{
		addShape( shape );
	}
}

void MPBCompositeShape::getShapes( MPBList& l )
{
	MPB_LIST_LOOP( m_shapes, shape, MPBShape* )
	{
		l.push_back( shape );
	}
}

MPBShape* MPBCompositeShape::getShape( int i )
{
	return (MPBShape*)m_shapes[i];
}

// call in a while loop
// definitely not thread-safe
MPBShape* MPBCompositeShape::getShape()
{
	static int i = 0;

	if (i < m_shapes.size())
	{
		MPBShape* shape = (MPBShape*)m_shapes[i];
		i++;
		return shape;
	}

	i = 0;
	return NULL;
}

/////////////////////////////////////////////////
// don't call these after the shape has been added
// to the level.
/////////////////////////////////////////////////

void MPBCompositeShape::translate( MPBFLOAT x, MPBFLOAT y, MPBFLOAT z )
{
	MPB_LIST_LOOP( m_shapes, shape, MPBShape* )
	{
		shape->translate( x, y, z );
	}

	findAABB();
}

void MPBCompositeShape::rotateX( MPBFLOAT degrees )
{
	MPB_LIST_LOOP( m_shapes, shape, MPBShape* )
	{
		shape->rotateX( degrees );
	}

	findAABB();
}

void MPBCompositeShape::rotateY( MPBFLOAT degrees )
{
	MPB_LIST_LOOP( m_shapes, shape, MPBShape* )
	{
		shape->rotateY( degrees );
	}

	findAABB();
}

void MPBCompositeShape::rotateZ( MPBFLOAT degrees )
{
	MPB_LIST_LOOP( m_shapes, shape, MPBShape* )
	{
		shape->rotateZ( degrees );
	}

	findAABB();
}

void MPBCompositeShape::scale( MPBFLOAT x, MPBFLOAT y, MPBFLOAT z )
{
	MPB_LIST_LOOP( m_shapes, shape, MPBShape* )
	{
		shape->scale( x, y, z );
	}

	findAABB();
}



