
#include "MPBNavigationMesh.h"
#include "MPBGameEntities.h"
#include "MPBShape.h"
#include "MPBMessageWindow.h"
#include "MPBVectorList.h"
#include "mmgr.h"


MPBNavigationNode::MPBNavigationNode():
	m_distanceFromStart( 0 ),
	m_score( 0 ),
	m_estimatedDistanceToGoal( 0 ),
	m_parent( NULL ),
	m_cellSize( 0 ),
	m_blocked( false )
{
}

MPBNavigationNode::~MPBNavigationNode()
{
}

void MPBNavigationNode::draw( MPBFLOAT y )
{
	if (m_blocked) glColor4f( 1, 0, 0, 1 );
	else glColor4f( 0, 1, 0, 1 );
	glBegin( GL_QUADS );
	glVertex3d( m_center.x - m_cellSize/2, y, m_center.z + m_cellSize/2 );
	glVertex3d( m_center.x + m_cellSize/2, y, m_center.z + m_cellSize/2 );
	glVertex3d( m_center.x + m_cellSize/2, y, m_center.z - m_cellSize/2 );
	glVertex3d( m_center.x - m_cellSize/2, y, m_center.z - m_cellSize/2 );
	glEnd();
	glColor4f( 1, 1, 1, 1 );
}


//////////////////

MPBNavigationMesh::MPBNavigationMesh( MPBFLOAT floor, MPBFLOAT ceiling ):
	m_nodes( NULL ),
	m_cellSize( 1.0 ),
	m_floor( floor ), 
	m_ceiling( ceiling )
{
}

MPBNavigationMesh::~MPBNavigationMesh()
{
	int gridWidth = getAABB().getWidth() / m_cellSize;
	int gridDepth = getAABB().getDepth() / m_cellSize;

	for (int z=0; z<gridDepth; z++)
	{
		delete[] m_nodes[z];
	}

	delete[] m_nodes;
}

void MPBNavigationMesh::startGameLoop()
{
	MPBMessageWindow::writeMessage( "Creating navigation mesh" );

	int gridWidth = getAABB().getWidth() / m_cellSize;
	int gridDepth = getAABB().getDepth() / m_cellSize;
	m_nodes = new MPBNavigationNode*[gridDepth];
	int unblockedNodes = 0;

	for (int z=0; z<gridDepth; z++)
	{
		sprintf( g_logBuf, "row %d", z );
		MPBMessageWindow::writeMessage();

		m_nodes[z] = new MPBNavigationNode[gridWidth];

		for (int x=0; x<gridWidth; x++)
		{
			MPBFLOAT worldX =  x * m_cellSize + getAABB().getNLL().x;
			MPBFLOAT worldZ = -z * m_cellSize + getAABB().getNLL().z;
			m_nodes[z][x].m_center.set( worldX, 0, worldZ );
			m_nodes[z][x].m_cellSize = m_cellSize;

			// find out if cell contains an obstacle
			MPBAABB nodeAABB( 
				MPBVector( worldX - m_cellSize/2, m_floor, worldZ + m_cellSize/2 ), 
				MPBVector( worldX + m_cellSize/2, m_ceiling, worldZ - m_cellSize/2 )
			);

			MPB_LIST_LOOP( MPBGameEntities::shapes, shape, MPBShape* )
			{
//				if (VECTOR_CONTAINS( m_ignoredShapes, shape ))
//					continue;

//				if (VECTOR_CONTAINS( m_shapes, shape ))
//					continue;

				if (nodeAABB.intersects( shape->getAABB() ))
				{
					m_nodes[z][x].m_blocked = true;
					break;
				}
			}

			if (!m_nodes[z][x].m_blocked)
			{
				unblockedNodes++;

				if (x > 0)
				{
					m_nodes[z][x].m_neighbors.push_back( &m_nodes[z][x - 1] );
					if (!m_nodes[z][x - 1].m_blocked)
						m_nodes[z][x - 1].m_neighbors.push_back( &m_nodes[z][x] );
				}

				if (z > 0)
				{
					m_nodes[z][x].m_neighbors.push_back( &m_nodes[z - 1][x] );
					if (!m_nodes[z - 1][x].m_blocked)
						m_nodes[z - 1][x].m_neighbors.push_back( &m_nodes[z][x] );
				}

				if (x > 0 && z > 0)
				{
					m_nodes[z][x].m_neighbors.push_back( &m_nodes[z - 1][x - 1] );
					if (!m_nodes[z - 1][x - 1].m_blocked)
						m_nodes[z - 1][x - 1].m_neighbors.push_back( &m_nodes[z][x] );

					if (x < gridWidth - 1)
					{
						m_nodes[z][x].m_neighbors.push_back( &m_nodes[z - 1][x + 1] );
						if (!m_nodes[z - 1][x + 1].m_blocked)
							m_nodes[z - 1][x + 1].m_neighbors.push_back( &m_nodes[z][x] );
					}
				}
			}
		}
	}
}

MPBNavigationNode* MPBNavigationMesh::getNode( MPBFLOAT worldX, MPBFLOAT worldZ )
{
	int x =  (worldX + (m_cellSize / 2.0) - getAABB().getNLL().x) / m_cellSize;
	int z = -(worldZ - (m_cellSize / 2.0) - getAABB().getNLL().z) / m_cellSize;
	if (x < 0 ) return NULL;
	if (z < 0 ) return NULL;
	if (x >= getAABB().getWidth() / m_cellSize ) return NULL;
	if (z >= getAABB().getDepth() / m_cellSize ) return NULL;

	return &m_nodes[z][x];
}

bool MPBNavigationMesh::getASTAR( MPBVector start, MPBVector goal, MPBVectorList& path )
{
	path.clear();

	MPBNavigationNode* startNode = getNode( start.x, start.z );
	MPBNavigationNode* goalNode = getNode( goal.x, goal.z );

	if (goalNode == startNode) return true;

	MPBList open;
	MPBList closed;

	startNode->m_distanceFromStart = 0;
	startNode->m_estimatedDistanceToGoal = (startNode->m_center - goal).getLength();
	startNode->m_score = startNode->m_distanceFromStart + startNode->m_estimatedDistanceToGoal;
	startNode->m_parent = NULL;

	open.push_back( startNode );
	while (open.size())
	{
		MPBNavigationNode* currentNode;
		MPBFLOAT lowestScore = 1000000;
		MPB_LIST_LOOP( open, n, MPBNavigationNode* )
		{
			if (n->m_score < lowestScore)
			{
				lowestScore = n->m_score;
				currentNode = n;
			}
		}

		open.remove( currentNode );

		if (currentNode == goalNode)
		{
			//	construct path and return
			m_path.clear();
			path.push_front( currentNode->m_center );

			while (currentNode->m_parent)
			{
				m_path.push_back( currentNode->m_parent );
				path.push_front( currentNode->m_parent->m_center );

				if (currentNode->m_parent == startNode)
				{
					return true;
				}

				currentNode = currentNode->m_parent;
			}

			MPB_ASSERT( 0 );
		}

		currentNode->m_distanceFromStart = (currentNode->m_center - start).getLength();

		MPB_LIST_LOOP( currentNode->m_neighbors, neighbor, MPBNavigationNode* )
		{
			MPBFLOAT distanceNodeToNeighbor = (currentNode->m_center - neighbor->m_center).getLength();
			MPBFLOAT newg = currentNode->m_distanceFromStart + distanceNodeToNeighbor;

			bool neighborInClosed = closed.contains( neighbor );
			bool neighborInOpen = open.contains( neighbor );

			if (neighborInClosed || neighborInOpen)
			{
				if (neighbor->m_distanceFromStart <= newg)
					continue;
			}

			neighbor->m_parent = currentNode;
			neighbor->m_distanceFromStart = newg;
			neighbor->m_estimatedDistanceToGoal = (neighbor->m_center - goal).getLength();
			neighbor->m_score = neighbor->m_distanceFromStart + neighbor->m_estimatedDistanceToGoal;

			if (neighborInClosed) closed.remove( neighbor );
			if (!neighborInOpen) open.push_back( neighbor );
		}

		closed.push_back( currentNode );
	}

	return false;

/*
f(n) = g(n) + h(n)
node score = start to node + node to goal

priorityqueue	Open
list		Closed

AStarSearch
	s.g = 0		// s is the start node
	s.h = GoalDistEstimate( s )
	s.f = s.g + s.h
	s.parent = null
	push s on Open
	while Open is not empty
		pop node n from Open  // n has the lowest f
		if n is a goal node 
			construct path 
			return success
		for each successor n' of n
			newg = n.g + cost(n,n')
			if n' is in Open or Closed,
			 and n'.g <= newg
				skip
			n'.parent = n
			n'.g = newg
			n'.h = GoalDistEstimate( n' )
			n'.f = n'.g + n'.h
			if n' is in Closed
				remove it from Closed
			if n' is not yet in Open
				push n' on Open
		push n onto Closed
	return failure  // if no path found
*/
}

void MPBNavigationMesh::draw()
{
	MPB_LIST_LOOP( m_path, nn, MPBNavigationNode* )
	{
		nn->draw( -2.6 );
	}
}




