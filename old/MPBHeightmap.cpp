
#include "MPBGlobal.h"
#include "MPBHeightMap.h"
#include "MPBPlane.h"
#include "MPBPartitionTreeNode.h"
#include "MPBTriangleBinaryTreeNode.h"
#include "MPBCollision.h"
#include "MPBOpenGL.h"
#include "MPBMessageWindow.h"
#include "mmgr.h"

const int HEIGHT_VALUES = 256;
const int TRIBIN_POOL_SIZE = 20000;
const MPBFLOAT TEX_COORD_DIVISOR = 30;

const bool USE_ODE = true;
const bool USE_TERRAGEN = false;
const char TERRAGEN_RAW_FILE[] = "terragen/terragenIsland1.raw";
const char TERRAGEN_TEXTURE_FILE[] = "terragenIsland1";

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
	m_startHeights( NULL ),
	m_morphs( NULL ),
	m_lightValues( NULL ),
	m_reds( NULL ),
	m_greens( NULL ),
	m_blues( NULL ),
	m_displayList( 0 ),
	m_draw( true )
{
}

MPBHeightMap::~MPBHeightMap()
{
	NULLIFY_ARRAY( m_planes[0] );
	NULLIFY_ARRAY( m_planes[1] );
	NULLIFY_ARRAY( m_heights );
	NULLIFY_ARRAY( m_scaledHeights );

	NULLIFY_ARRAY( m_startHeights );
	NULLIFY_ARRAY( m_morphs );
	NULLIFY_ARRAY( m_lightValues );
	NULLIFY_ARRAY( m_reds );
	NULLIFY_ARRAY( m_greens );
	NULLIFY_ARRAY( m_blues );
//	NULLIFY_ARRAY( m_normals );

	NULLIFY_ARRAY( m_triBinNodes );
}

// heightMapFile should be relative to execute dir
// textureFile should be relative to texture dir
// heightMapFile: width, height must be the same even number
// heights will have y-values of 0 - 255 units, multiplied by scaleY
// each point will be separated by 1 unit, multiplied by scaleXZ
void MPBHeightMap::init(	const char* heightMapFile,
									char* textureFile,
									MPBFLOAT scaleXZ,
									MPBFLOAT scaleY	)
{
	MPBMessageWindow::writeMessage( "init height map..." );

	strcpy( m_texture, textureFile );
	strcpy( m_lightMap, "" );

//	m_patchQuadtree = NULL;

//	struct stat statBuf;
	m_width = 0;
	m_height = 0;
	m_scaleXZ = scaleXZ;
	m_scaleY = scaleY;

	int row, column;
   int i;

	MPBMessageWindow::writeMessage( "read height map file..." );

	if (MPBOpenGL::is2D())
	{
		m_width = 32;
		m_height = 32;
		m_heights = new BYTE[ m_width * m_height ];

		for (column=0; column<m_width; column++)
		{
			BYTE b = RANDOM_FRACTION * 40.0;
			for (row=0; row<m_height; row++)
			{
				*(m_heights + (row * m_width + column)) = b;
			}
		}
	}
	else if (USE_TERRAGEN)
	{
		m_heights = readRawFile8( TERRAGEN_RAW_FILE, m_width, m_height );
	}
	else
	{
		// read heightmap

		m_heights = readBitmapColors( heightMapFile, m_width, m_height );

		MPB_ASSERT( m_width % 2 == 0 );
		MPB_ASSERT( m_height % 2 == 0 );
	}

	MPB_ASSERT( m_width > 0 );
	MPB_ASSERT( m_width == m_height );

	int size = m_width * m_height;

	m_maxRecursionDepth = log( m_width ) / log( 2 ) * 2;	// = 16 for width 256

	// convert bitmap's array of color indexes to array of Planes

//	m_normals = new MPBVector[ size ];
//	MPB_ASSERT( m_normals );

	m_scaledHeights = new MPBFLOAT[ size ];
	MPB_ASSERT( m_scaledHeights );
	for (i=0; i<size; i++)
	{
		m_scaledHeights[i] = m_heights[i] * m_scaleY;
	}

	m_startHeights = new MPBFLOAT[ size ];
	MPB_ASSERT( m_startHeights );
	for (i=0; i<size; i++)
	{
		m_startHeights[i] = 0;
	}

	m_morphs = new MPBFLOAT[ size ];
	MPB_ASSERT( m_morphs );
	for (i=0; i<size; i++)
	{
		m_morphs[i] = 1.0;
	}

	m_planes[0] = new Plane[ size ];
	m_planes[1] = new Plane[ size ];

	MPBVector LL;
	MPBVector LR;
	MPBVector UR;
	MPBVector UL;

	MPBMessageWindow::writeMessage( "create height map planes..." );

	for (row=0; row<m_height - 1; row++)
	{
		if (row % 16 == 0)
		{
			sprintf( g_logBuf, "row %d", row );
			MPBMessageWindow::writeMessage();
		}

		for (column=0; column<m_width - 1; column++)
		{
			int c, r;
			MPBFLOAT x0, x1, x2, x3;
			MPBFLOAT y0, y1, y2, y3;
			MPBFLOAT z0, z1, z2, z3;

			c = column; r = row + 1;
			x0 = c;
			y0 = HEIGHT_VALUE( r, c );
			z0 = -r;

			c = column; r = row;
			x1 = c;
			y1 = HEIGHT_VALUE( r, c );
			z1 = -r;

			c = column + 1; r = row + 1;
			x2 = c;
			y2 = HEIGHT_VALUE( r, c );
			z2 = -r;

			c = column + 1; r = row;
			x3 = c;
			y3 = HEIGHT_VALUE( r, c );
			z3 = -r;

//			if (	g_level->m_holeMap->m_bitmap->m_heights[row][column] == HOLE_COLOR	)
//			{
//				y0 = y1 = y2 = y3 = 0;
//			}

			x0 *= scaleXZ;
			y0 *= scaleY;
			z0 *= scaleXZ;

			x1 *= scaleXZ;
			y1 *= scaleY;
			z1 *= scaleXZ;

			x2 *= scaleXZ;
			y2 *= scaleY;
			z2 *= scaleXZ;

			x3 *= scaleXZ;
			y3 *= scaleY;
			z3 *= scaleXZ;

			int planeIndex = (row * m_width + column);

/* 

			Interpreting pixel data
				x0		x2
				x1		x3

				(x1 = current heightmap pixel)

			Plane configuration:

				row and column both even or both odd:
					index 0:
					0	2			LR		LL
					1				UR

					index 1:
						2					UR
					1	3			LL		LR

				otherwise:
					index 0:
					0				LL
					1	3			LR		UR

					index 1:
					0	2			UR		LR
						3					LL

*/

			if (column % 2 == row % 2)	// row and column both even or both odd
			{
				LL.set( x2, y2, z2 );
				LR.set( x0, y0, z0 );
				UR.set( x1, y1, z1 );

				m_planes[0][ planeIndex ].setCorners( LL, LR, UR );

				LL.set( x1, y1, z1 );
				LR.set( x3, y3, z3 );
				UR.set( x2, y2, z2 );

				m_planes[1][ planeIndex ].setCorners( LL, LR, UR );
			}
			else
			{
				LL.set( x0, y0, z0 );
				LR.set( x1, y1, z1 );
				UR.set( x3, y3, z3 );

				m_planes[0][ planeIndex ].setCorners( LL, LR, UR );

				LL.set( x3, y3, z3 );
				LR.set( x2, y2, z2 );
				UR.set( x0, y0, z0 );

				m_planes[1][ planeIndex ].setCorners( LL, LR, UR );
			}

/*			if (row >= 1 && column >= 1)
			{
				m_normals[ planeIndex ] = 
					(m_planes[0][ row * m_width + column ].getNormal() +
					m_planes[1][ row * m_width + column ].getNormal() +
					m_planes[1][ row * m_width + (column - 1) ].getNormal() +
					m_planes[0][ (row - 1) * m_width + (column - 1) ].getNormal() +
					m_planes[1][ (row - 1) * m_width + (column - 1) ].getNormal() +
					m_planes[0][ (row - 1) * m_width + column ].getNormal()) /
					6;
			}
			else
			{
				m_normals[ planeIndex ].set( 0, 1, 0 );
			}

			MPB_ASSERT( m_normals[ planeIndex ].getLength() != 0 );
*/
		}
	}


	/////////////////////////////////////////////////////////////
	// create colors

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
			m_reds[ getIndex( row, column ) ] = HEIGHT_VALUES - HEIGHT_VALUE( row, column ) / 2;
			m_greens[ getIndex( row, column ) ] = HEIGHT_VALUES/2 + HEIGHT_VALUE( row, column ) / 2;
			m_blues[ getIndex( row, column ) ] = 0;
		}
	}

	/////////////////////////////////////////////////////////////
	// preallocate bintree nodes

	m_triBinNodes = new MPBTriangleBinaryTreeNode[ TRIBIN_POOL_SIZE ];
	MPB_ASSERT( m_triBinNodes );
	m_triBinNodesIndex = 0;


	if (USE_ODE)
	{
		MPBMessageWindow::writeMessage( "create ODE trimesh..." );

		dTriMeshDataID triMeshDataID = dGeomTriMeshDataCreate();

		int numVertices = size;
		dVector3* vertices = new dVector3[numVertices];

		int numIndices = 3 * 2 * (m_width - 1) * (m_height - 1);
		int* indices = new int[numIndices];

		int i = 0;
		int v = 0;
		for (row=0; row<m_height; row++)
		{
			if (row % 16 == 0)
			{
				sprintf( g_logBuf, "row %d", row );
				MPBMessageWindow::writeMessage();
			}

			for (column=0; column<m_width; column++)
			{
				vertices[v][0] = column * scaleXZ;
				vertices[v][1] = HEIGHT_VALUE( row, column ) * scaleY;
				vertices[v][2] = -row * scaleXZ;

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

		m_geomID = dCreateTriMesh( 0, triMeshDataID, NULL, NULL, NULL );
	}
}

// call after all Shapes have been added.
// lightmapFile: must be same dimensions as heightMapFile
void MPBHeightMap::initLightmap( char* lightmapFile )
{
	MPBMessageWindow::writeMessage( "init height light map..." );

	struct stat statBuf;
	int row;
	int column;

	if (USE_TERRAGEN)
	{
		strcpy( m_lightMap, TERRAGEN_TEXTURE_FILE );
	}
	else
	{
		MPB_ASSERT( stat( lightmapFile, &statBuf ) == 0 );

		int width, height;
		m_lightValues = readBitmapColors( lightmapFile, width, height );
		MPB_ASSERT( width == m_width );
		MPB_ASSERT( height == m_height );

		/////////////////////////////////////////////////////////////
		// modulate color map with light map

		for (row=0; row<m_height; row++)
		{
			if (row % 16 == 0)
			{
				sprintf( g_logBuf, "row %d", row );
				MPBMessageWindow::writeMessage();
			}

			for (column=0; column<m_width; column++)
			{
				MPBFLOAT lightPercent = (float)(LIGHT_VALUE( row, column ) / (float)HEIGHT_VALUES);
				lightPercent = .5 + .5 * lightPercent;
				*(m_reds + (row * m_width + column)) *= lightPercent;
				*(m_greens + (row * m_width + column)) *= lightPercent;
				*(m_blues + (row * m_width + column)) *= lightPercent;
			}
		}
	}
}

void MPBHeightMap::draw()
{
	if (!m_draw) return;

	if (0)	// draw 1 large quad
	{
		glBegin( GL_QUADS );

		int column, row;
		MPBFLOAT x, y, z;

		column = 0;
		row = 0;
		x = column * m_scaleXZ;
		z = row * -m_scaleXZ;
		y = HEIGHT_VALUE( row, column ) * m_scaleY;
		glVertex3d( x, y+1, z );

		column = m_width - 1;
		row = 0;
		x = column * m_scaleXZ;
		z = row * -m_scaleXZ;
		y = HEIGHT_VALUE( row, column ) * m_scaleY;
		glVertex3d( x, y+1, z );

		column = m_width - 1;
		row = m_height - 1;
		x = column * m_scaleXZ;
		z = row * -m_scaleXZ;
		y = HEIGHT_VALUE( row, column ) * m_scaleY;
		glVertex3d( x, y+1, z );

		column = 0;
		row = m_height - 1;
		x = column * m_scaleXZ;
		z = row * -m_scaleXZ;
		y = HEIGHT_VALUE( row, column ) * m_scaleY;
		glVertex3d( x, y+1, z );

		glEnd();
	}
	else
	{
//		glColor3ub( 255, 255, 255 );

		// ground texture
	//	MPBOpenGL::bindTexture( "solidWhite", GL_TEXTURE0 );
		MPBOpenGL::bindTexture( m_texture, GL_TEXTURE0 );

		// lightmap texture
		if (MAX_TEXTURES > 1) MPBOpenGL::bindTexture( m_lightMap, GL_TEXTURE1 );

		// make triangles
		memset( m_triBinNodes, 0, TRIBIN_POOL_SIZE * sizeof( MPBTriangleBinaryTreeNode ) );
		m_triBinNodesIndex = 0;

		m_leftRootNode = allocateTriBintreeNode();
		m_rightRootNode = allocateTriBintreeNode();

		m_leftRootNode->m_baseNeighbor = m_rightRootNode;
		m_rightRootNode->m_baseNeighbor = m_leftRootNode;

		POINT pointApexL = { 0, 0 };
		POINT pointLeftL = { 0, m_height - 1 };
		POINT pointRightL = { m_width - 1, 0 };

		POINT pointApexR = { m_width - 1, m_height - 1 };
		POINT pointLeftR = { m_width - 1, 0 };
		POINT pointRightR = { 0, m_height - 1 };

		m_recursionDepth = 0;

		tessellateTriBintreeNode(	m_leftRootNode,
											pointApexL,
											pointLeftL,
											pointRightL
										);

		m_recursionDepth = 0;

		tessellateTriBintreeNode(	m_rightRootNode,
											pointApexR,
											pointLeftR,
											pointRightR
										);


		if (MPBOpenGL::is2D())
		{
			glLineWidth( 5 );
			glBegin( GL_LINE_STRIP );
		}
		else
		{
		//	glEnable( GL_COLOR_MATERIAL );
		//	glColorMaterial( GL_FRONT, GL_AMBIENT_AND_DIFFUSE );

	//		glPolygonMode( GL_FRONT_AND_BACK, GL_LINE );
			glBegin( GL_TRIANGLES );
		}

		glNormal3i( 0, 1, 0 );

		m_trianglesRendered = 0;

		renderTriBintreeNode(	m_leftRootNode,
										pointApexL,
										pointLeftL,
										pointRightL	);

		renderTriBintreeNode(	m_rightRootNode,
										pointApexR,
										pointLeftR,
										pointRightR	);

		glEnd();

		glPolygonMode( GL_FRONT_AND_BACK, GL_FILL );
	//	glDisable( GL_COLOR_MATERIAL );

		MPBOpenGL::unbindTexture( m_texture, GL_TEXTURE0 );
		if (MAX_TEXTURES > 1) MPBOpenGL::unbindTexture( m_lightMap, GL_TEXTURE1 );
	}
}

void MPBHeightMap::tessellateTriBintreeNode(	MPBTriangleBinaryTreeNode* node,
															POINT pointApex,
															POINT pointLeft,
															POINT pointRight/*,
															MPBFLOAT apexStartHeight*/	)
{
	m_recursionDepth++;

//	MPBFLOAT heightLeft = HEIGHT_VALUE( pointLeft.y, pointLeft.x ) * m_scaleY;
//	MPBFLOAT heightRight = HEIGHT_VALUE( pointRight.y, pointRight.x ) * m_scaleY;
//	MPBFLOAT heightMidpoint = (heightRight + heightLeft) / 2;

	POINT pointChildApex = {	(pointLeft.x + pointRight.x) / 2,
										(pointLeft.y + pointRight.y) / 2	};

	POINT pointMiddle = {	(pointChildApex.x + pointApex.x) / 2,
									(pointChildApex.y + pointApex.y) / 2	};

	MPBVector cameraPos = MPBOpenGL::getCameraPos() / m_scaleXZ;
	MPBFLOAT middleToCamera = 
		sqrt(
			pow( pointMiddle.x - cameraPos.x, 2 ) + 
			pow( pointMiddle.y - -cameraPos.z, 2 )
		);

	MPBFLOAT baseToApex = 
		sqrt(
			pow( pointChildApex.x - pointApex.x, 2 ) + 
			pow( pointChildApex.y - pointApex.y, 2 )
		);

//	for (int i=0; i<m_recursionDepth-1; i++)
//		writeLog( "   " );
//	sprintf( g_logBuf, "%f --- %f", baseToCamera, baseToApex );
//	writeLog();
//	writeLog( "\n" );

	bool isInFrustum = true;

	MPBVector points[3];
	points[0].set( pointApex.x * m_scaleXZ, 0, -pointApex.y * m_scaleXZ );
	points[1].set( pointLeft.x * m_scaleXZ, 0, -pointLeft.y * m_scaleXZ );
	points[2].set( pointRight.x * m_scaleXZ, 0, -pointRight.y * m_scaleXZ );
//	isInFrustum = MPBOpenGL::pointsInOpenFrustum( points, 3 );
	isInFrustum = MPBOpenGL::pointsInFrustum( points, 3 );

	points[0].set( pointApex.x * m_scaleXZ, HEIGHT_VALUES * m_scaleY, -pointApex.y * m_scaleXZ );
	points[1].set( pointLeft.x * m_scaleXZ, HEIGHT_VALUES * m_scaleY, -pointLeft.y * m_scaleXZ );
	points[2].set( pointRight.x * m_scaleXZ, HEIGHT_VALUES * m_scaleY, -pointRight.y * m_scaleXZ );
//	isInFrustum = isInFrustum || MPBOpenGL::pointsInOpenFrustum( points, 3 );
	isInFrustum = isInFrustum || MPBOpenGL::pointsInFrustum( points, 3 );

/*	points[0].set( pointApex.x * m_scaleXZ, pointApex.y * m_scaleY, -pointApex.y * m_scaleXZ );
	points[1].set( pointLeft.x * m_scaleXZ, pointLeft.y * m_scaleY, -pointLeft.y * m_scaleXZ );
	points[2].set( pointRight.x * m_scaleXZ, pointRight.y * m_scaleY, -pointRight.y * m_scaleXZ );

	isInFrustum = MPBOpenGL::pointsInFrustum( points, 3 );
*/
	if (isInFrustum)
	{
		// split the triangle if needed
		if (m_recursionDepth <= m_maxRecursionDepth && middleToCamera < 16 * baseToApex)
		{
			splitTriBintreeNode( node );

			tessellateTriBintreeNode(	node->m_leftChild,
												pointChildApex,
												pointApex,
												pointLeft/*,
												heightMidpoint*/	);

			tessellateTriBintreeNode(	node->m_rightChild,
												pointChildApex,
												pointRight,
												pointApex/*,
												heightMidpoint*/	);
		}
	}

	m_recursionDepth--;
}

void MPBHeightMap::splitTriBintreeNode( MPBTriangleBinaryTreeNode* node )
{
	if (node->m_leftChild) return;	// already split

	// force base neighbor to split if it's a different LOD
	// (ie, does not form a diamond with this node)
	if (node->m_baseNeighbor && node->m_baseNeighbor->m_baseNeighbor != node)
	{
		splitTriBintreeNode( node->m_baseNeighbor );
	}

	// create children
	node->m_leftChild = allocateTriBintreeNode();
	node->m_rightChild = allocateTriBintreeNode();

	// set neighbor relationships
	node->m_leftChild->m_baseNeighbor = node->m_leftNeighbor;
	node->m_leftChild->m_leftNeighbor = node->m_rightChild;
	node->m_leftChild->m_rightNeighbor = NULL;

	node->m_rightChild->m_baseNeighbor = node->m_rightNeighbor;
	node->m_rightChild->m_rightNeighbor = node->m_leftChild;
	node->m_rightChild->m_leftNeighbor = NULL;

	if (node->m_leftNeighbor)
	{
		if (node->m_leftNeighbor->m_baseNeighbor == node)
		{
			node->m_leftNeighbor->m_baseNeighbor = node->m_leftChild;
		}
		else if (node->m_leftNeighbor->m_leftNeighbor == node)
		{
			node->m_leftNeighbor->m_leftNeighbor = node->m_leftChild;
		}
		else if (node->m_leftNeighbor->m_rightNeighbor == node)
		{
			node->m_leftNeighbor->m_rightNeighbor = node->m_leftChild;
		}
	}

	if (node->m_rightNeighbor)
	{
		if (node->m_rightNeighbor->m_baseNeighbor == node)
		{
			node->m_rightNeighbor->m_baseNeighbor = node->m_rightChild;
		}
		else if (node->m_rightNeighbor->m_rightNeighbor == node)
		{
			node->m_rightNeighbor->m_rightNeighbor = node->m_rightChild;
		}
		else if (node->m_rightNeighbor->m_leftNeighbor == node)
		{
			node->m_rightNeighbor->m_leftNeighbor = node->m_rightChild;
		}
	}

	if (node->m_baseNeighbor)
	{
		if (node->m_baseNeighbor->m_leftChild)
		{
			node->m_baseNeighbor->m_leftChild->m_rightNeighbor = node->m_rightChild;
			node->m_baseNeighbor->m_rightChild->m_leftNeighbor = node->m_leftChild;
			node->m_leftChild->m_rightNeighbor = node->m_baseNeighbor->m_rightChild;
			node->m_rightChild->m_leftNeighbor = node->m_baseNeighbor->m_leftChild;
		}
		else
		{
			splitTriBintreeNode( node->m_baseNeighbor );
		}
	}
}

MPBTriangleBinaryTreeNode* MPBHeightMap::allocateTriBintreeNode()
{
	m_triBinNodesIndex++;
	MPB_ASSERT( m_triBinNodesIndex < TRIBIN_POOL_SIZE );
	return &(m_triBinNodes[ m_triBinNodesIndex ]);
}

void MPBHeightMap::renderTriBintreeNode(	MPBTriangleBinaryTreeNode* node,
														POINT pointApex,
														POINT pointLeft,
														POINT pointRight	)
{
	if (node->m_leftChild)
	{
		POINT pointChildApex = {	(pointLeft.x + pointRight.x) / 2,
											(pointLeft.y + pointRight.y) / 2	};

		renderTriBintreeNode(	node->m_leftChild,
										pointChildApex,
										pointApex,
										pointLeft	);

		renderTriBintreeNode(	node->m_rightChild,
										pointChildApex,
										pointRight,
										pointApex	);
	}
	else
	{
		m_trianglesRendered++;

		plotVertex( pointApex.x, pointApex.y );//, apexStartHeight );
		plotVertex( pointRight.x, pointRight.y );
		plotVertex( pointLeft.x, pointLeft.y );
	}
}

void MPBHeightMap::plotVertex( int column, int row ) const
{
	MPBFLOAT y;

	y = SCALED_HEIGHT_VALUE( row, column );

	plotVertex( column, row, y );
}

void MPBHeightMap::plotVertex( int column, int row, MPBFLOAT y ) const
{
	glColor3ub(	RED_VALUE( row, column ),
					GREEN_VALUE( row, column ),
					BLUE_VALUE( row, column )	);


	MPBFLOAT x = column * m_scaleXZ;
	MPBFLOAT z = row * -m_scaleXZ;

	// detail texture
	MPBOpenGL::glMultiTexCoord3f( GL_TEXTURE0, column / TEX_COORD_DIVISOR, row / TEX_COORD_DIVISOR, 0 );

	// lightmap texture
	MPBOpenGL::glMultiTexCoord3f( GL_TEXTURE1, column / (float)m_width, row / (float)m_height, 0 );

	glVertex3d( x, y, z );
}

void MPBHeightMap::getAABB( MPBAABB& aabb )
{
	int stretch = 100;
	MPBVector NLL( 0, -stretch, 0 );
	MPBVector FUR( m_width * m_scaleXZ, HEIGHT_VALUES * m_scaleY + stretch, -m_height * m_scaleXZ );
	aabb.setCorners( NLL, FUR );
}

// returns the plane index (0 or 1) at the given point
int MPBHeightMap::getPlaneIndex( MPBFLOAT xPos, MPBFLOAT zPos ) const
{
	MPB_ASSERT( xPos >= 0 );
	MPB_ASSERT( xPos < (m_width - 1) * m_scaleXZ );
	MPB_ASSERT( -zPos >= 0 );
	MPB_ASSERT( -zPos < (m_height - 1) * m_scaleXZ );

	int column = xPos / m_scaleXZ;
	int row = -zPos / m_scaleXZ;
	int columnOffset = xPos - column * m_scaleXZ;
	int rowOffset = -zPos - row * m_scaleXZ;

	int planeIndex;

	if (column % 2 == row % 2)
	{
		int distanceToUpperLeft = columnOffset + (m_scaleXZ - rowOffset);
		int distanceToLowerRight = (m_scaleXZ - columnOffset) + rowOffset;

		if (distanceToUpperLeft < distanceToLowerRight) planeIndex = 0;
		else planeIndex = 1;
	}
	else
	{
		int distanceToLowerLeft = columnOffset + rowOffset;
		int distanceToUpperRight = (m_scaleXZ - columnOffset) + (m_scaleXZ - rowOffset);

		if (distanceToLowerLeft < distanceToUpperRight) planeIndex = 0;
		else planeIndex = 1;
	}

	return planeIndex;
}

// returns the plane at the given point
Plane* MPBHeightMap::getPlane( MPBFLOAT xPos, MPBFLOAT zPos ) const
{
	if (!( xPos >= 0 )) return NULL;
	if (!( xPos < (m_width - 1) * m_scaleXZ )) return NULL;
	if (!( -zPos >= 0 )) return NULL;
	if (!( -zPos < (m_height - 1) * m_scaleXZ )) return NULL;

	MPB_ASSERT( xPos >= 0 );
	MPB_ASSERT( xPos < (m_width - 1) * m_scaleXZ );
	MPB_ASSERT( -zPos >= 0 );
	MPB_ASSERT( -zPos < (m_height - 1) * m_scaleXZ );

	int planeIndex = getPlaneIndex( xPos, zPos );
	if (planeIndex == -1) return NULL;
	int column = xPos / m_scaleXZ;
	int row = -zPos / m_scaleXZ;
	return &(m_planes[planeIndex][ row * m_width + column ]);
}

// returns the height at the given point
MPBFLOAT MPBHeightMap::getHeight( MPBFLOAT xPos, MPBFLOAT zPos ) const
{
	Plane* plane = getPlane( xPos, zPos );
	if (plane) return plane->getYAtPoint( xPos, zPos );

//	MPB_ASSERT( 0 );
	return 0;
}

// returns all planes underneath the box
void MPBHeightMap::getPlanes( const MPBAABB& moveBox, MPBList& planes )
{
	int column = moveBox.getCenter().x / m_scaleXZ;
	int row = -moveBox.getCenter().z / m_scaleXZ;

	for (int c = -1; c < 2; c++)
	{
		for (int r = -1; r < 2; r++)
		{
			for (int i = 0; i < 2; i++)
			{
				if (	row + r >= 0 &&
						row + r < getWidth() &&
						column + c >= 0 &&
						column + c < getHeight()	)
				{
					Plane* plane = &(m_planes[i][ (row + r) * m_width + (column + c) ]);
					if (plane->getNormal().getLength())
					{
						planes.push_back( plane );
					}
				}
			}
		}
	}
}

bool MPBHeightMap::getPenetrationPoint( MPBVector& point ) const
{
	Plane* plane = getPlane( point.x, point.z );
	MPB_ASSERT( plane );

	if (!plane || plane->isPointOutside( point )) return false;

	MPBVector ip;
	plane->lineIntersect(	point, 
									point + plane->getNormal(), 
									ip	);

	point = ip;
	return true;
}

// finds the highest terrain point in the area defined by 4 points
// points should be counterclockwise
// returns the height
MPBFLOAT MPBHeightMap::findHighestPoint( MPBVector LL, MPBVector LR, MPBVector UR, MPBVector UL ) const
{
	LL = LL / m_scaleXZ;
	LR = LR / m_scaleXZ;
	UR = UR / m_scaleXZ;
	UL = UL / m_scaleXZ;

	MPBVector LL2 = LL;
	LL.y = 10;
	LL2.y = 0;

	MPBVector LR2 = LR;
	LR.y = 10;
	LR2.y = 0;

	MPBVector UR2 = UR;
	UR.y = 10;
	UR2.y = 0;

	MPBVector UL2 = UL;
	UL.y = 10;
	UL2.y = 0;

	Plane p1( LL2, LR2, LR, LL );
	Plane p2( LR2, UR2, UR, LR );
	Plane p3( UR2, UL2, UL, UR );
	Plane p4( UL2, LL2, LL, UL );

	MPBFLOAT height = 0;

	for (int row=0; row<m_height; row++)
	{
		for (int column=0; column<m_width; column++)
		{
			MPBVector v( column, 5, -row );
			if (	p1.isPointInside( v ) &&
					p2.isPointInside( v ) &&
					p3.isPointInside( v ) &&
					p4.isPointInside( v )	)
				height = max( height, getHeight( column * m_scaleXZ, -row * m_scaleXZ ) );
		}
	}

	return height;
}

// finds the highest terrain point in the given area
// return the height
// x, z is center
MPBFLOAT MPBHeightMap::findHighestPoint( MPBFLOAT x, MPBFLOAT z, MPBFLOAT width, MPBFLOAT depth ) const
{
	int tileXFirst = ( x - width/2);
	int tileXLast =  ( x + width/2);
	int tileZFirst = (-z - depth/2);
	int tileZLast =  (-z + depth/2);

	MPBFLOAT height = 0;
		
	for (int tileX=tileXFirst; tileX<=tileXLast; tileX++)
	{
		for (int tileZ=tileZFirst; tileZ<=tileZLast; tileZ++)
		{
			height = max( height, getHeight( tileX, -tileZ ) );
		}
	}

	return height;
}

