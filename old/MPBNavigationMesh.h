#ifndef __MPBNAVIGATIONMESH_H__
#define __MPBNAVIGATIONMESH_H__

#include "MPBLibrary.h"
#include "MPBCompositeShape.h"
#include "MPBList.h"
#include "MPBVectorList.h"

class MPBShape;


class MPBNavigationNode
{
public:

	MPBNavigationNode();
	~MPBNavigationNode();

	void draw( MPBFLOAT y );

	MPBList								m_neighbors;		// MPBNavigationNode*

//protected:

	MPBVector							m_center;
	MPBFLOAT 							m_distanceFromStart;
	MPBFLOAT 							m_score;
	MPBFLOAT 							m_estimatedDistanceToGoal;
	MPBNavigationNode*				m_parent;
	MPBFLOAT 							m_cellSize;
	bool									m_blocked;

};


class MPBNavigationMesh: public MPBCompositeShape
{
public:

	MPBNavigationMesh( MPBFLOAT floor, MPBFLOAT ceiling );
	~MPBNavigationMesh();

	void startGameLoop();
	MPBNavigationNode* getNode( MPBFLOAT worldX, MPBFLOAT worldZ );
	bool getASTAR( MPBVector start, MPBVector goal, MPBVectorList& path );
	void draw();

	MPBNavigationNode**				m_nodes;
	MPBList								m_path;		// MPBNavigationNode*
	MPBFLOAT 							m_cellSize;
	MPBFLOAT 							m_floor;
	MPBFLOAT 							m_ceiling;

protected:

};




#endif

