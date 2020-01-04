
#include "MPBNavigationMesh.h"
#include "MPBGameEntities.h"
#include "MPBShape.h"
#include "MPBMessageWindow.h"


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
					m_nodes[z][x].m_neig