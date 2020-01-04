#ifndef __HEIGHTMAP_H__
#define __HEIGHTMAP_H__

#include "MPBVector.h"
#include "MPBList.h"
#include <ode/ode.h>

class Plane;
class MPBAABB;
class PartitionTreeNode;
class MPBTriangleBinaryTreeNode;

class MPBHeightMap
{
public:

	MPBHeightMap();
	~MPBHeightMap();
	void init(	const char* heightMapFile, 
					char* textureFile,
					MPBFLOAT scaleXZ,
					MPBFLOAT scaleY	);

	void initLightmap( char* lightmapFile );
	void plotVertex( int column, int row ) const;
	void plotVertex( int column, int row, MPBFLOAT y ) const;
	void draw();

	MPBFLOAT getHeight() { return m_height; }
	int getPlaneIndex( MPBFLOAT xPos, MPBFLOAT zPos ) const;
	Plane* getPlane( MPBFLOAT xPos, MPBFLOAT zPos ) const;
	MPBFLOAT getHeight( MPBFLOAT xPos, MPBFLOAT zPos ) const;
	int getWidth() { return m_width; }
	MPBFLOAT getScaleXZ() { return m_scaleXZ; }
	MPBFLOAT getScaleY() { return m_scaleY; }
	MPBFLOAT getMaxX() { return m_width * m_scaleXZ; }
	void getAABB( MPBAABB& aabb );
	void getPlanes( const MPBAABB& moveBox, MPBList& planes );
	bool getPenetrationPoint( MPBVector& point ) const;
	void setDraw( bool b ) { m_draw = b; }
	MPBFLOAT findHighestPoint( MPBVector LL, MPBVector LR, MPBVector UR, MPBVector UL ) const;
	MPBFLOAT findHighestPoint( MPBFLOAT x, MPBFLOAT z, MPBFLOAT width, MPBFLOAT depth ) const;
	dGeomID getGeomID() { return m_geomID; }

	int								m_trianglesRendered;

private:

	Plane*							m_planes[2];
	int								m_width;
	int								m_height;
	MPBFLOAT m_scaleXZ;		// number of points between bitmap pixels
	MPBFLOAT m_scaleY;		// number of points between bitmap pixel values (height=0-255 * scaleY)
	BYTE*								m_heights;
	MPBFLOAT*						m_scaledHeights;

	BYTE								mapValue( BYTE* const& array, int row, int column ) const;
	MPBFLOAT 						scaledHeightValue( int row, int column ) const;
	BYTE								heightValue( int row, int column ) const;
	BYTE								lightValue( int row, int column ) const;
	BYTE								redValue( int row, int column ) const;
	BYTE								greenValue( int row, int column ) const;
	BYTE								blueValue( int row, int column ) const;
	int								getIndex( int row, int column ) const;

	char								m_texture[MAX_PATH];
	char								m_lightMap[MAX_PATH];

	MPBFLOAT*						m_startHeights;
	MPBFLOAT*						m_morphs;
	BYTE*								m_lightValues;
	BYTE*								m_reds;
	BYTE*								m_greens;
	BYTE*								m_blues;
//	MPBVector*						m_normals;
	GLuint							m_displayList;

	MPBTriangleBinaryTreeNode* m_leftRootNode;
	MPBTriangleBinaryTreeNode* m_rightRootNode;
	MPBTriangleBinaryTreeNode*	m_triBinNodes;
	int								m_triBinNodesIndex;
	int								m_recursionDepth;
	int								m_maxRecursionDepth;
	bool								m_draw;

	void tessellateTriBintreeNode(	MPBTriangleBinaryTreeNode* node,
												POINT pointApex,
												POINT pointLeft,
												POINT pointRight/*,
												MPBFLOAT apexStartHeight*/	);

	void renderTriBintreeNode(	MPBTriangleBinaryTreeNode* node,
										POINT pointApex,
										POINT pointLeft,
										POINT pointRight	);

	void splitTriBintreeNode( MPBTriangleBinaryTreeNode* node );
	MPBTriangleBinaryTreeNode* allocateTriBintreeNode();

	dGeomID				m_geomID;
};


#endif

