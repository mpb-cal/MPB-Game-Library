#ifndef __HEIGHTMAP_H__
#define __HEIGHTMAP_H__

#include "MPBVector.h"
#include "MPBList.h"
#include <ode/ode.h>

class Plane;
class MPBAABB;
class PartitionTreeNode;
class MPBTriangleBinaryTreeNode;


const int DISPLAY_LISTS = 2;


class MPBHeightMap
{
public:

	MPBHeightMap();
	~MPBHeightMap();
	void init(	const char* heightMapFile, 
					bool isBMP,		// else RAW
					char* textureFile,
					char* lightmapFile,
					MPBFLOAT scaleXZ,
					MPBFLOAT scaleY,
					MPBFLOAT waterLevel	);

	void plotVertex( int column, int row ) const;
	void draw();

	int getHeight() { return m_height; }
	int getWidth() { return m_width; }
	MPBFLOAT getElevation( MPBFLOAT x, MPBFLOAT z );
	MPBFLOAT getScaleXZ() { return m_scaleXZ; }
	MPBFLOAT getScaleY() { return m_scaleY; }
	MPBFLOAT getMaxX() { return m_width * m_scaleXZ; }
	void getAABB( MPBAABB& aabb );
	void setDraw( bool b ) { m_draw = b; }
	dGeomID getGeomID() { return m_geomID; }
	int getColumn( MPBFLOAT x );
	int getRow( MPBFLOAT z );
	void getPlane( Plane& plane, MPBFLOAT x, MPBFLOAT z );
	MPBVector getNormal( MPBFLOAT x, MPBFLOAT z );

	int								m_trianglesRendered;

private:

	int								m_width;
	int								m_height;
	MPBFLOAT							m_scaleXZ;		// number of points between bitmap pixels
	MPBFLOAT							m_scaleY;		// number of points between bitmap pixel values (height=0-255 * scaleY)
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

	BYTE*								m_lightValues;
	BYTE*								m_reds;
	BYTE*								m_greens;
	BYTE*								m_blues;
	GLuint							m_displayList[DISPLAY_LISTS];
	GLuint							m_displayListWater;
	bool								m_draw;


	void initLightmap( char* lightmapFile );

	dGeomID				m_geomID;
};


#endif

