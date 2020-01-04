
#include "MPBGlobal.h"
#include "MPBHeightMap.h"
#include "MPBPlane.h"
#include "MPBPartitionTreeNode.h"
#include "MPBTriangleBinaryTreeNode.h"
#include "MPBCollision.h"
#include "MPBOpenGL.h"
#include "MPBMessageWindow.h"
#include "mmgr.h"


// detail texture: GL_TEXTURE0
// big texture: GL_TEXTURE1


const bool USE_MULTI_TEXTURE = false;
const bool USE_COLORED_VERTEX = false;
const int SPLIT_DISTANCE_FACTOR = 16;
const int HEIGHT_VALUES = 256;
const int TRIBIN_POOL_SIZE = 20000;
const MPBFLOAT TEX_COORD_DIVISOR = 1;//30;

//int triCallback( dGeomID TriMesh, dGeomID RefObject, int TriangleIndex )
//{
//	return 0;
//}


#define SCALED_HEIGHT_VALUE( row, column )	scaledHeightValue( row, column )
#define HEIGHT_VALUE( row, column )				heightValue( row, column )
#define LIGHT_VALUE( row, column )				lightValue( row, column )
#define RED_VALUE( row, column )					redValue( row, column )
#define GREEN_VALUE( row, column )				greenValue( row, column )
#define BLUE_VALUE( row, column )				blueValue( row, column )

MPBFLOAT MPBHeightMap::scaledHeightValue( int row, int column ) const
{
	if (	row < 0 ||
			row >= m_width ||
			column < 0 ||
			column >= m_height	)
		return 0;

	return *(m_scaledHeights + (row * m_width + column));
}

BYTE MPBHeightMap::mapValue( BYTE* const& array, int row, int column ) const
{
	if (	row < 0 ||
			row >= m_width ||
			column < 0 ||
			column >= m_height	)
		return 0;

	return *(array + (row * m_width + column));
}

BYTE MPBHeightMap::heightValue( int row, int column ) const { return mapValue( m_heights, row, column ); }
BYTE MPBHeightMap::lightValue( int row, int column ) const { return mapValue( m_lightValues, row, column ); }
BYTE MPBHeightMap::redValue( int row, int column ) const { return mapValue( m_reds, row, column ); }
BYTE MPBHeightMap::greenValue( int row, int column ) const { return mapValue( m_greens, row, column ); }
BYTE MPBHeightMap::blueValue( int row, int column ) const { return mapValue( m_blues, row, column ); }

int MPBHeightMap::getIndex( int row, int column ) const
{
	if (	row < 0 ||
			row >= m_width ||
			column < 0 ||
			column >= m_height	)
		MPB_ASSERT( 0 );

	return row * m_width + column;
}


MPBHeightMap::MPBHeightMap():
	m_heights( NULL ),
	m_scaledHeights( NULL ),
	m_lightValues( NULL ),
	m_reds( NULL ),
	m_greens( NULL ),
	m_blues( NULL ),
	m_draw( true ),
	m_geomID( 0 )
{
}

MPBHeightMap::~MPBHeightMap()
{
	NULLIFY_ARRAY( m_heights );
	NULLIFY_ARRAY( m_scaledHeights );

	NULLIFY_ARRAY( m_lightValues );
	NULLIFY_ARRAY( m_reds );
	NULLIFY_ARRAY( m_greens );
	NULLIFY_ARRAY( m_blues );
}


// heightMapFile should be relative to execute dir
// textureFile should be relative to texture dir
// heightMapFile: width, height must be the same
// heights will have y-values of 0 - 255 units, multiplied by scaleY
// each point will be separated by 1 unit, multiplied by scaleXZ
void MPBHeightMap::init(	const char* heightMapFile,
									bool isBMP,		// else RAW
									char* textureFile,
									char* lightmapFile,
									MPBFLOAT scaleXZ,
									MPBFLOAT scaleY,
									MPBFLOAT waterLevel	)
{
	MPBMessageWindow::writeMessage( "init height map..." );

	strcpy( m_texture, textureFile );
	strcpy( m_lightMap, lightmapFile );

//	m_patchQuadtree = NULL;

//	struct stat statBuf;
	m_width = 0;
	m_height = 0;
	m_scaleXZ = scaleXZ;
	m_scaleY = scaleY;

	int row, column;
   int i;

	MPBMessageWindow::writeMessage( "read height map file..." );

	// read heightmap

	if (isBMP)
		m_heights = readBitmapColors( heightMapFile, m_width, m_height );
	else
		m_heights = readRawFile8( heightMapFile, m_width, m_height );

//	MPB_ASSERT( m_width % 2 == 0 );
//	MPB_ASSERT( m_height % 2 == 0 );

	MPB_ASSERT( m_width > 0 );
	MPB_ASSERT( m_width == m_height );

	int size = m_width * m_height;

	// convert bitmap's array of color indexes to array of Planes

	m_scaledHeights = new MPBFLOAT[ size ];
	MPB_ASSERT( m_scaledHeights );
	for (i=0; i<size; i++)
	{
		m_scaledHeights[i] = m_heights[i] * m_scaleY;
	}


	/////////////////////////////////////////////////////////////
	// create colors

	if (USE_COLORED_VERTEX)
	{
		m_reds = new BYTE[ size ];
		MPB_ASSERT( m_reds );
		m_greens = new BYTE[ size ];
		MPB_ASSERT( m_greens );
		m_blues = new BYTE[ size ];
		MPB_ASSERT( m_blues );

		for (row=0; row<m_height; row++)
		{
			for (column=0; column<m_width; column++)
			{
				m_reds[ getIndex( row, column ) ] = (HEIGHT_VALUES - 1) - HEIGHT_VALUE( row, column ) / 2;
				m_greens[ getIndex( row, column ) ] = HEIGHT_VALUES/2 + HEIGHT_VALUE( row, column ) / 2;
				m_blues[ getIndex( row, column ) ] = 64;
			}
		}
	}

	/////////////////////////////////////////////////////////////
	// create ODE mesh

	MPBMessageWindow::writeMessage( "create ODE trimesh..." );

	dTriMeshDataID triMeshDataID = dGeomTriMeshDataCreate();

	int numVertices = size;
	dVector3* vertices = new dVector3[numVertices];

	int numIndices = 3 * 2 * (m_width - 1) * (m_height - 1);
	int* indices = new int[numIndices];

	i = 0;
	int v = 0;
	for (row=0; row<m_height; row++)
	{
		for (column=0; column<m_width; column++)
		{
			vertices[v][0] = column * scaleXZ;
			vertices[v][1] = HEIGHT_VALUE( row, column ) * scaleY;
			vertices[v][2] = -row * scaleXZ;

			if (column != m_width - 1 && row != m_height - 1)
			{
				indices[i++] = v;
				indices[i++] = v + 1;
				indices[i++] = v + m_width + 1;

				indices[i++] = v;
				indices[i++] = v + m_width + 1;
				indices[i++] = v + m_width;
			}

			v++;
		}
	}

	MPB_ASSERT( v == numVertices );
	MPB_ASSERT( i == numIndices );

	dGeomTriMeshDataBuildSimple( triMeshDataID, (const dReal*)vertices, numVertices, indices, numIndices );

	m_geomID = dCreateTriMesh( 0, triMeshDataID, NULL, NULL, NULL );

//	dGeomTriMeshSetCallback( m_geomID, triCallback );

	// set up display lists

	for (i=0; i<DISPLAY_LISTS; i++)
	{
		m_displayList[i] = glGenLists( 1 );
		glNewList( m_displayList[i], GL_COMPILE );

		for (row=0; row<m_height - 1; row++)
		{
			glBegin( GL_TRIANGLE_STRIP );

			for (column=0; column<m_width - 1; column++)
			{
				if (i == 0) glTexCoord3f( column / TEX_COORD_DIVISOR, (row + 1) / TEX_COORD_DIVISOR, 0 );
				else if (i == 1) glTexCoord3f( column / (float)m_width, (row + 1) / (float)m_height, 0 );
				plotVertex( column, row + 1 );

				if (i == 0) glTexCoord3f( column / TEX_COORD_DIVISOR, row / TEX_COORD_DIVISOR, 0 );
				else if (i == 1) glTexCoord3f( column / (float)m_width, row / (float)m_height, 0 );
				plotVertex( column, row );
			}

			glEnd();
		}

		glEndList();
	}

	// water

	const int INC = 8;
	const int WIDTH_FACTOR = 5;
	m_displayListWater = glGenLists( 1 );
	glNewList( m_displayListWater, GL_COMPILE );

	for (row=-m_height * (WIDTH_FACTOR - 1); row<m_height * WIDTH_FACTOR; row += INC)
	{
		glBegin( GL_QUAD_STRIP );

		for (column=-m_width * (WIDTH_FACTOR - 1); column<m_width * WIDTH_FACTOR; column += INC)
		{
			glVertex3d( column * m_scaleXZ, waterLevel, (row + INC) * -m_scaleXZ );
			glVertex3d( column * m_scaleXZ, waterLevel, row * -m_scaleXZ );
		}

		glEnd();
	}

	glEndList();
}


void MPBHeightMap::draw()
{
	if (!m_draw) return;

	glPushAttrib( GL_ALL_ATTRIB_BITS );

	// 1st pass

	MPBOpenGL::bindTexture( m_texture, GL_TEXTURE0 );
//	if (USE_MULTI_TEXTURE && MAX_TEXTURES > 1) MPBOpenGL::bindTexture( m_lightMap, GL_TEXTURE1 );

	glColor4f( 1, 1, 1, 1 );
//	glPolygonMode( GL_FRONT_AND_BACK, GL_LINE );
	glCallList( m_displayList[0] );

	// 2nd pass

	glDepthFunc( GL_LEQUAL );
	glEnable( GL_BLEND );
	glColor4f( 1, 1, 1, .5 );
	MPBOpenGL::bindTexture( m_lightMap, GL_TEXTURE0 );

	glCallList( m_displayList[1] );
	glPopAttrib();

	// water

	MPBOpenGL::bindTexture( "waterA.tex" );
	glColor4f( 1, 1, 1, 1 );
	glCallList( m_displayListWater );
}


void MPBHeightMap::plotVertex( int column, int row ) const
{
	if (USE_COLORED_VERTEX)
		glColor3ub(	RED_VALUE( row, column ),
						GREEN_VALUE( row, column ),
						BLUE_VALUE( row, column )	);


	MPBFLOAT x = column * m_scaleXZ;
	MPBFLOAT z = row * -m_scaleXZ;
	MPBFLOAT y = SCALED_HEIGHT_VALUE( row, column );

	// detail texture
//	MPBOpenGL::glMultiTexCoord3f( GL_TEXTURE0, column / TEX_COORD_DIVISOR, row / TEX_COORD_DIVISOR, 0 );

	// big texture
//	if (USE_MULTI_TEXTURE) MPBOpenGL::glMultiTexCoord3f( GL_TEXTURE1, column / (float)m_width, row / (float)m_height, 0 );

	glVertex3d( x, y, z );
}

int MPBHeightMap::getColumn( MPBFLOAT x )
{
	return x / m_scaleXZ;
}

int MPBHeightMap::getRow( MPBFLOAT z )
{
	return -z / m_scaleXZ;
}

void MPBHeightMap::getAABB( MPBAABB& aabb )
{
	int stretch = 100;
	MPBVector NLL( 0, -stretch, 0 );
	MPBVector FUR( m_width * m_scaleXZ, HEIGHT_VALUES * m_scaleY + stretch, -m_height * m_scaleXZ );
	aabb.setCorners( NLL, FUR );
}

void MPBHeightMap::getPlane( Plane& plane, MPBFLOAT x, MPBFLOAT z )
{
	int column = getColumn( x );
	int row = getRow( z );

	MPBFLOAT columnOffset = x - column * m_scaleXZ;
	MPBFLOAT rowOffset = -z - row * m_scaleXZ;

	MPBFLOAT distanceToUpperLeft = columnOffset + (m_scaleXZ - rowOffset);
	MPBFLOAT distanceToLowerRight = (m_scaleXZ - columnOffset) + rowOffset;

	MPBVector LL;
	MPBVector LR;
	MPBVector UR;

	if (distanceToUpperLeft < distanceToLowerRight)
	{
		LL.set( column, 0, row );
		LR.set( column + 1, 0, row + 1 );
		UR.set( column, 0, row + 1 );
	}
	else
	{
		LL.set( column, 0, row );
		LR.set( column + 1, 0, row );
		UR.set( column + 1, 0, row + 1 );
	}

	LL.y = SCALED_HEIGHT_VALUE( LL.z, LL.x );
	LR.y = SCALED_HEIGHT_VALUE( LR.z, LR.x );
	UR.y = SCALED_HEIGHT_VALUE( UR.z, UR.x );

	LL.x = LL.x * m_scaleXZ;
	LR.x = LR.x * m_scaleXZ;
	UR.x = UR.x * m_scaleXZ;

	LL.z = -LL.z * m_scaleXZ;
	LR.z = -LR.z * m_scaleXZ;
	UR.z = -UR.z * m_scaleXZ;

	plane.setCorners( LL, LR, UR );
}

MPBVector MPBHeightMap::getNormal( MPBFLOAT x, MPBFLOAT z )
{
	Plane p;
	getPlane( p, x, z );

	return p.getNormal();
}

MPBFLOAT MPBHeightMap::getElevation( MPBFLOAT x, MPBFLOAT z )
{
	Plane p;
	getPlane( p, x, z );

	return p.getYAtPoint( x, z );
}


/*
void MPBHeightMap::collide()
{
	dTriMeshDataID triMeshDataID = dGeomTriMeshDataCreate();

	int numVertices = 16;
	dVector3* vertices = new dVector3[numVertices];

	int numIndices = 18;//3 * 2 * (m_width - 1) * (m_height - 1);
	int* indices = new int[numIndices];

	int i = 0;
	int v = 0;
	for (int row=10; row<13; row++)
	{
		for (int column=20; column<13; column++)
		{
			vertices[v][0] = column * m_scaleXZ;
			vertices[v][1] = SCALED_HEIGHT_VALUE( row, column );
			vertices[v][2] = -row * m_scaleXZ;

			if (column != m_width - 1 && row != m_height - 1)
			{
				indices[i++] = v;
				indices[i++] = v + 1;
				indices[i++] = v + m_width;

				indices[i++] = v;
				indices[i++] = v + m_width;
				indices[i++] = v + m_width - 1;
			}

			v++;
		}
	}

	MPB_ASSERT( v == numVertices );
	MPB_ASSERT( i == numIndices );

	dGeomTriMeshDataBuildSimple( triMeshDataID, (const dReal*)vertices, numVertices, indices, numIndices );

	dGeomID geomIDHeightMap = dCreateTriMesh( 0, triMeshDataID, NULL, NULL, NULL );

	dGeomDestroy( geomIDHeightMap );
	dGeomTriMeshDataDestroy( triMeshDataID );
	delete[] vertices;
	delete[] indices;
}
*/

