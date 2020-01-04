#ifndef __PARTITIONTREENODE_H__
#define __PARTITIONTREENODE_H__

#include "MPBAABB.h"
#include "MPBList.h"


class MPBShape;
class Plane;
class MPBTexturedPlane;

enum PartitionTreeType
{
	QUADTREE,
	OCTREE
};


class MPBPartitionTreeNode
{

public:

	MPBPartitionTreeNode( PartitionTreeType type, int maxShapes, int minWidth );
	~MPBPartitionTreeNode();

	void buildTree( MPBList& shapes, const MPBAABB& aabb );
	void intersectBox(	const MPBAABB& moveBox, 
								MPBList& intersectedPlanes );
	void resetIntersect();
	void startDraw();
	void draw();
	void drawShapes( bool backToFront, bool shadowersOnly );

	PartitionTreeType						m_type;
	MPBAABB									m_aabb;
	MPBList									m_shapes;	// MPBShape*
	MPBList									m_children;	// MPBPartitionTreeNode*
	bool										m_intersected;

	static int								s_drawDepth;
	static int								s_drawDepthMax;
	static int								s_maxShapes;
	static int								s_minWidth;
};



#endif

