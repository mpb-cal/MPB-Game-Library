
#include "MPBGlobal.h"
//#include "MPBProfile.h"
#include "MPBOpenGL.h"
#include "MPBPartitionTreeNode.h"
#include "MPBShape.h"
#include "MPBTexturedPlane.h"
#include "MPBGameEntities.h"
#include "mmgr.h"


int								MPBPartitionTreeNode::s_drawDepth;
int								MPBPartitionTreeNode::s_drawDepthMax;
int								MPBPartitionTreeNode::s_maxShapes = 1;
int								MPBPartitionTreeNode::s_minWidth = 1;

MPBPartitionTreeNode::MPBPartitionTreeNode( PartitionTreeType type, int maxShapes, int minWidth ):
	m_type( type ),
	m_intersected( false )
{
	s_maxShapes = maxShapes;
	s_minWidth = minWidth;
}

MPBPartitionTreeNode::~MPBPartitionTreeNode()
{
	MPB_LIST_DELETE( m_children, MPBPartitionTreeNode* );
}

void MPBPartitionTreeNode::buildTree( MPBList& shapes, const MPBAABB& aabb )
{
	MPBList childShapes[8];
	MPBAABB childAABB[8];

	m_aabb = aabb;

	static int depth = -1;
	depth++;

	for (int i=0; i<depth; i++) writeLog( "\t" );
	writeLog( "start node\n" );

	// return if this node is empty
	if (shapes.size() == 0) 
	{
		depth--; return;
//		goto leaveNode;
	}

	// if this node too small, store the shapes in this node and return
	if (aabb.getWidth() <= s_minWidth)
	{
		MPB_LIST_LOOP( shapes, s, MPBShape* ) { m_shapes.push_back( s ); }

		depth--; return;
//		goto leaveNode;
	}

	// if this node contains maxShapes or fewer, store the shapes in this node and return
	if (shapes.size() <= s_maxShapes)
	{
		MPB_LIST_LOOP( shapes, s, MPBShape* ) { m_shapes.push_back( s ); }

		depth--; return;
//		goto leaveNode;
	}

	int children;
	if (m_type == OCTREE)
	{
		children = 8;
	}
	else if (m_type == QUADTREE)
	{
		children = 4;
	}

	// find dimensions of child volumes

	if (m_type == OCTREE)
	{
		MPBVector extents(	aabb.getExtentX() / 2, 
									aabb.getExtentY() / 2, 
									aabb.getExtentZ() / 2	);
		MPBVector center;
		int centers[8 * 3] = {
			-1,  1,  1,
			 1,  1,  1,
			-1, -1,  1,
			 1, -1,  1,
			-1,  1, -1,
			 1,  1, -1,
			-1, -1, -1,
			 1, -1, -1,
		};

		int j = 0;
		for (int i=0; i<children; i++)
		{
			center.x = aabb.getCenter().x + (aabb.getExtentX() / 2) * centers[j++];
			center.y = aabb.getCenter().y + (aabb.getExtentY() / 2) * centers[j++];
			center.z = aabb.getCenter().z + (aabb.getExtentZ() / 2) * centers[j++];
			childAABB[i].setCenterAndExtents( center, extents );
		}
	}
	else if (m_type == QUADTREE)
	{
		MPBVector extents(	aabb.getExtentX() / 2, 
									aabb.getExtentY(), 
									aabb.getExtentZ() / 2	);
		MPBVector center;
		int centers[4 * 2] = {
			-1,  1,
			 1,  1,
			 1, -1,
			-1, -1
		};

		int j = 0;
		for (int i=0; i<children; i++)
		{
			center.x = aabb.getCenter().x + (aabb.getExtentX() / 2) * centers[j++];
			center.y = aabb.getCenter().y;
			center.z = aabb.getCenter().z + (aabb.getExtentZ() / 2) * centers[j++];
			childAABB[i].setCenterAndExtents( center, extents );
		}
	}

	// put each shape into 1 or more lists based on which child it is in

	MPB_LIST_LOOP( shapes, shape, MPBShape* )
	{
		for (int i=0; i<children; i++)
		{
			if (1)
			{
				if (shape->getAABB().intersects( childAABB[i] ))
				{
					childShapes[i].push_back( shape );
				}
			}
			else
			{
				// check each plane of this shape

				MPB_LIST_LOOP( shape->getPlanes(), plane, MPBTexturedPlane* )
				{
					if (plane->AABBIntersect( childAABB[i] ))
					{
						childShapes[i].push_back( shape );
						break;
					}
				}
			}
		}
	}

	// create children
	for (i=0; i<children; i++) 
	{
		MPBPartitionTreeNode* child = new MPBPartitionTreeNode( m_type, s_maxShapes, s_minWidth );
		child->buildTree( childShapes[i], childAABB[i] );
		m_children.push_back( child );
	}
/*
leaveNode:

	for (i=0; i<depth; i++) writeLog( "\t" );
	writeLog( "end node: " );

	int numShapes = m_shapes.size();
	sprintf( g_logBuf, "%d shapes, ", numShapes );
	writeLog();

	char buf[100];
	m_aabb.getCenter().toString( buf );
	sprintf( g_logBuf, "center: %s, ", buf );
	writeLog();

	sprintf( g_logBuf, "width: %f, ", m_aabb.getWidth() );
	writeLog();

	sprintf( g_logBuf, "height: %f, ", m_aabb.getHeight() );
	writeLog();

	sprintf( g_logBuf, "depth: %f, ", m_aabb.getDepth() );
	writeLog();

	writeLog( "\n" );

	depth--;
*/
}

void MPBPartitionTreeNode::intersectBox(	const MPBAABB& moveBox, 
														MPBList& intersectedPlanes )
{
	m_intersected = false;
	if (!moveBox.intersects( m_aabb )) return;

	MPB_LIST_LOOP( m_shapes, s, MPBShape* )
	{
		if (moveBox.intersects( s->getAABB() ))
			s->getPlanes( intersectedPlanes );
	}

	if (m_children.size() == 0) m_intersected = true;

	// see which children we intersect with
	MPB_LIST_LOOP( m_children, c, MPBPartitionTreeNode* )
	{
		c->intersectBox( moveBox, intersectedPlanes );
	}
}

// only needs to be called when drawing the octree for debugging
void MPBPartitionTreeNode::resetIntersect()
{
	m_intersected = false;

	MPB_LIST_LOOP( m_children, c, MPBPartitionTreeNode* )
	{
		c->resetIntersect();
	}
}

void MPBPartitionTreeNode::draw()
{
	if (m_intersected)
	{
		GLfloat color[] = { 1, 0, 0, 1 };
		glMaterialfv( GL_FRONT, GL_AMBIENT, color );
		glMaterialfv( GL_FRONT, GL_DIFFUSE, color );
		glMaterialfv( GL_FRONT, GL_SPECULAR, color );
		glMaterialfv( GL_FRONT, GL_EMISSION, color );
		MPBAABB aabb = m_aabb;
		aabb.grow( .1 );
		aabb.drawLines();
	}

	GLfloat color[] = { 0, 1, 0, 1 };
	glMaterialfv( GL_FRONT, GL_AMBIENT, color );
	glMaterialfv( GL_FRONT, GL_DIFFUSE, color );
	glMaterialfv( GL_FRONT, GL_SPECULAR, color );
	glMaterialfv( GL_FRONT, GL_EMISSION, color );
	m_aabb.drawLines();

	MPB_LIST_LOOP( m_children, c, MPBPartitionTreeNode* )
	{
		c->draw();
	}
}

bool SortChildrenFunc( MPBPartitionTreeNode* a, MPBPartitionTreeNode* b ) 
{
	MPBVector aCenter = a->m_aabb.getCenter();
	MPBVector bCenter = b->m_aabb.getCenter();
	MPBVector cameraCenter = MPBOpenGL::getCameraPos();

	MPBVector aToCamera = aCenter - cameraCenter;
	MPBFLOAT aToCameraDist = pow( aToCamera.x, 2 ) + pow( aToCamera.y, 2 ) + pow( aToCamera.z, 2 );

	MPBVector bToCamera = bCenter - cameraCenter;
	MPBFLOAT bToCameraDist = pow( bToCamera.x, 2 ) + pow( bToCamera.y, 2 ) + pow( bToCamera.z, 2 );

	return aToCameraDist > bToCameraDist;
}

void MPBPartitionTreeNode::startDraw()
{
	s_drawDepth = 0;
	s_drawDepthMax = 0;
}

void MPBPartitionTreeNode::drawShapes( bool backToFront, bool shadowersOnly )
{
	s_drawDepth++;
	s_drawDepthMax = max( s_drawDepthMax, s_drawDepth );

	// frustum cull this node
	if (!MPBOpenGL::aabbInFrustum( m_aabb ))
	{
		s_drawDepth--;
		return;
	}

	// sort shapes back-to-front
//	if (backToFront)
//	{
//		sort( m_shapes.begin(), m_shapes.end(), SortShapeFunc );
//	}

	// draw this node's shapes
	MPB_LIST_LOOP( m_shapes, s, MPBShape* )
	{
		if (s->getAlpha() == false)
			s->draw( !shadowersOnly, shadowersOnly, true );	// we only get here if we have no children
	}

	// sort children back-to-front
//	if (backToFront)
//	{
//		sort( m_children.begin(), m_children.end(), SortChildrenFunc );
//	}

	// draw this node's children
	MPB_LIST_LOOP( m_children, c, MPBPartitionTreeNode* )
	{
		c->drawShapes( backToFront, shadowersOnly );
	}

	s_drawDepth--;
}



