
#include "MPBOpenGL.h"
#include "MPBTexture.h"
#include "MPBPhysics.h"
#include "MPBTextureGenerator.h"
#include "MPBGameEntities.h"
#include "MPBAABB.h"
#include "MPBFrameCounter.h"
//#include "MPBProfile.h"
#include "MPBCountdown.h"
#include "MPBPlane.h"
#include "MPBMessageWindow.h"
#include "MPBCollision.h"
#include "MPBShape.h"
#include "MPBPhysicsObject.h"
#include "MPBCompositeShapeCell.h"
#include "MPBTriangle.h"
#include "MPBShapeQuad.h"
#include "mmgr.h"

//#define USE_SINGLE_TEXTURE

#pragma comment(lib, "opengl32.lib")
#pragma comment(lib, "glu32.lib")
#pragma comment(lib, "glaux.lib")
#pragma comment(lib, "dinput.lib")
#pragma comment(lib, "dxguid.lib")
#pragma comment(lib, "dsound.lib")

const bool		ENABLE_FRUSTUM_CULLING		= true;
const bool		ENV_TEXTURE						= false;
const MPBFLOAT PIXELS_PER_METER_2D			= 400.0;
const bool		SHAPE_SHADOW_VOLUMES			= true;
const bool		OBJECT_SHADOW_VOLUMES		= true;
const bool		GRAY_QUAD_SHADOW_VOLUMES	= true;
const bool		USE_SHADOW_TEXTURES			= false;
const bool		NO_TEXTURES						= false;
const MPBFLOAT GROUND_FOG_Y					= 2.0;
const bool		USE_LIGHT_MAPS					= false;
const bool		USE_FOG_COORDS					= false;
const bool		USE_OPENGL_LIGHTING			= false;
const bool		DRAW_PROJECTION_SHADOWS		= false;
const DWORD		BITSPERPIXEL					= 16;
const BYTE		STENCIL_BUFFER_BITS			= 8;
const BYTE		DEPTH_BUFFER_BITS				= 16;
const char*		WNDCLASSNAME					= "MPBOpenGLWndClass";
const char*		PARTICLE_TEXTURE				= "GeneratedParticleTexture";
#ifdef USE_SINGLE_TEXTURE
	const char*		SINGLE_TEXTURE				= "planeTexture";
#else
	const char*		SINGLE_TEXTURE				= NULL;
#endif

HWND		m_hwnd = 0;
char		m_lastTextureFile[MAX_PATH][MAX_TEXTURES];

HDC									MPBOpenGL::m_hdc = 0;
int									MPBOpenGL::m_screenWidth;
int									MPBOpenGL::m_screenHeight;
GLfloat 								MPBOpenGL::m_perspectiveAngle = 70;
GLfloat 								MPBOpenGL::m_perspectiveFar = 200;//2000;
GLfloat 								MPBOpenGL::m_perspectiveNear = .1f;//.001f;
bool									MPBOpenGL::m_is2D = false;
GLuint								MPBOpenGL::m_fontListBase = 0;
char									MPBOpenGL::m_fontTexture[MAX_PATH];
char									MPBOpenGL::m_textureDir[MAX_PATH];
MPBList								MPBOpenGL::m_textures[MAX_TEXTURES];
GLUquadricObj*						MPBOpenGL::quadricObj = NULL;
GLUtesselator*						MPBOpenGL::tesselator = NULL;
MPBVector							MPBOpenGL::m_cameraPos;
MPBVector							MPBOpenGL::m_lookAt;
PFNGLACTIVETEXTUREARBPROC		MPBOpenGL::glActiveTexture = NULL;
PFNGLMULTITEXCOORD3FARBPROC	MPBOpenGL::glMultiTexCoord3f = NULL;
PFNGLTEXIMAGE3DPROC				MPBOpenGL::glTexImage3D = NULL;
PFNGLFOGCOORDDEXTPROC			MPBOpenGL::glFogCoordd = NULL;
Plane									MPBOpenGL::m_viewFrustumRight;
Plane									MPBOpenGL::m_viewFrustumLeft;
Plane									MPBOpenGL::m_viewFrustumTop;
Plane									MPBOpenGL::m_viewFrustumBottom;
Plane									MPBOpenGL::m_viewFrustumNear;
Plane									MPBOpenGL::m_viewFrustumFar;
MPBList								MPBOpenGL::m_pointLights;
MPBList								MPBOpenGL::m_staticPointLights;
GLuint								MPBOpenGL::m_displayListShadowShape = -1;
bool									MPBOpenGL::m_renderFromLight = false;
MPBFLOAT 							MPBOpenGL::m_lightModelViewMatrix[16];
MPBFLOAT 							MPBOpenGL::m_lightProjectionMatrix[16];
MPBFLOAT 							MPBOpenGL::m_fogRed = 1;
MPBFLOAT 							MPBOpenGL::m_fogGreen = 1;
MPBFLOAT 							MPBOpenGL::m_fogBlue = 1;
MPBFLOAT 							MPBOpenGL::m_fogDensity = .02f;
bool									MPBOpenGL::m_fogEnabled = false;
bool									MPBOpenGL::m_noShadows = false;
bool									MPBOpenGL::m_drawCrosshairs = true;
MPBList								MPBOpenGL::m_shadowVolumeVertexes;
GLuint								MPBOpenGL::m_displayListShadowVolume;
MPBVector							MPBOpenGL::clearColor;
MPBNavigationMesh*				MPBOpenGL::m_navMesh = NULL;
MPBVector							MPBOpenGL::planeSortPosition;
MPBPhysicsObject*					MPBOpenGL::m_player = NULL;

GLuint m_shadowMapTextureID;

void MPBOpenGL::errorCheck()
{
	GLenum error = glGetError();

	if (error != GL_NO_ERROR)
	{
		const GLubyte *errString = gluErrorString( error );

		if (error == GL_INVALID_ENUM)
		{
			MPB_ASSERT( 0 );
		}
		else if (error == GL_INVALID_VALUE)
		{
			MPB_ASSERT( 0 );
		}
		else if (error == GL_INVALID_OPERATION)
		{
			MPB_ASSERT( 0 );
		}
		else if (error == GL_STACK_OVERFLOW)
		{
			MPB_ASSERT( 0 );
		}
		else if (error == GL_STACK_UNDERFLOW)
		{
			MPB_ASSERT( 0 );
		}
		else if (error == GL_OUT_OF_MEMORY)
		{
			MPB_ASSERT( 0 );
		}
		else if (error == GL_TABLE_TOO_LARGE)
		{
			MPB_ASSERT( 0 );
		}
	}
}

HWND MPBOpenGL::createGLWindow(	WNDPROC wndProc,
											HINSTANCE hInstance,
											char* wndName,
											bool fullScreen,
											bool _2D,
											int width,
											int height,
											char* textureDir,
											HMENU hMenu	)
{
	m_screenWidth = width;
	m_screenHeight = height;
	m_is2D = _2D;
	strcpy( m_textureDir, textureDir );

	WNDCLASSEX wcex;
	memset( &wcex, 0, sizeof( WNDCLASSEX ) );

	wcex.cbSize				= sizeof(WNDCLASSEX);
	wcex.style				= CS_HREDRAW | CS_VREDRAW |  CS_OWNDC;
	wcex.lpfnWndProc		= wndProc;
	wcex.cbClsExtra		= 0;
	wcex.cbWndExtra		= 0;
	wcex.hInstance			= hInstance;
	wcex.hCursor			= LoadCursor(NULL, IDC_ARROW);
	wcex.hbrBackground	= (HBRUSH)(COLOR_WINDOW+1);
	wcex.lpszClassName	= WNDCLASSNAME;

	RegisterClassEx( &wcex );

	DWORD dwStyle;
	DWORD dwExStyle;

	if (fullScreen)
	{
		DEVMODE dmScreenSettings;
		memset( &dmScreenSettings, 0, sizeof( dmScreenSettings ) );
		dmScreenSettings.dmSize = sizeof( dmScreenSettings );
		dmScreenSettings.dmPelsWidth = width;
		dmScreenSettings.dmPelsHeight = height;
		dmScreenSettings.dmBitsPerPel	= BITSPERPIXEL;
		dmScreenSettings.dmFields = DM_BITSPERPEL | DM_PELSWIDTH | DM_PELSHEIGHT;

		// CDS_FULLSCREEN Gets Rid Of Start Bar.
		LONG rv = ChangeDisplaySettings( &dmScreenSettings, CDS_FULLSCREEN );
		MPB_ASSERT( rv == DISP_CHANGE_SUCCESSFUL );

		dwExStyle = WS_EX_APPWINDOW;
		dwStyle = WS_POPUP | WS_CLIPSIBLINGS | WS_CLIPCHILDREN,
		ShowCursor( false );
	}
	else
	{
		dwExStyle = WS_EX_APPWINDOW | WS_EX_WINDOWEDGE;
		dwStyle = WS_OVERLAPPEDWINDOW | WS_CLIPSIBLINGS | WS_CLIPCHILDREN;
	}

	m_hwnd = CreateWindowEx(
		dwExStyle,
		WNDCLASSNAME,
		wndName,
		dwStyle,
		0,
		0,
		width,
		height,
		NULL,
		hMenu,
		hInstance,
		NULL
	);

	MPB_ASSERT( m_hwnd );

	m_hdc = GetDC( m_hwnd );

	PIXELFORMATDESCRIPTOR pfd = {
		sizeof(PIXELFORMATDESCRIPTOR),	// Size Of This Pixel Format Descriptor
		1,                               // Version Number
		PFD_DRAW_TO_WINDOW |             // Format Must Support Window
		PFD_SUPPORT_OPENGL |             // Format Must Support OpenGL
		PFD_DOUBLEBUFFER,                // Must Support MPBFLOAT Buffering
		PFD_TYPE_RGBA,                   // Request An RGBA Format
		BITSPERPIXEL,                    // Select Our Color Depth
		0, 0, 0, 0, 0, 0,                // Color Bits Ignored
		0,                               // No Alpha Buffer
		0,                               // Shift Bit Ignored
		0,                               // No Accumulation Buffer
		0, 0, 0, 0,                      // Accumulation Bits Ignored
		DEPTH_BUFFER_BITS,               // 16Bit Z-Buffer (Depth Buffer)
		STENCIL_BUFFER_BITS,             // 8-bit Stencil Buffer
		0,                               // No Auxiliary Buffer
		PFD_MAIN_PLANE,                  // Main Drawing Layer
		0,                               // Reserved
		0, 0, 0                          // Layer Masks Ignored
	};

	GLuint pixelFormat = ChoosePixelFormat( m_hdc, &pfd );
	MPB_ASSERT( pixelFormat != 0 );
	if (!SetPixelFormat( m_hdc, pixelFormat, &pfd )) { MPB_ASSERT( 0 ); }
	HGLRC hglrc = wglCreateContext( m_hdc );
//	MPBOpenGL::errorCheck();
	MPB_ASSERT( hglrc );

	wglMakeCurrent( m_hdc, hglrc );
	MPBOpenGL::errorCheck();

	bool gl13Supported = atof( (const char*)glGetString( GL_VERSION ) ) >= 1.3;
	MPB_ASSERT( gl13Supported );

	ShowWindow( m_hwnd, SW_SHOWNORMAL );
	UpdateWindow( m_hwnd );

	/////////////////////
	// init openGL

	// set up GL extensions
	glActiveTexture = (PFNGLACTIVETEXTUREARBPROC)wglGetProcAddress( "glActiveTextureARB" );
	glMultiTexCoord3f	= (PFNGLMULTITEXCOORD3FARBPROC)wglGetProcAddress( "glMultiTexCoord3fARB" );
	glTexImage3D = (PFNGLTEXIMAGE3DPROC)wglGetProcAddress( "glTexImage3D" );
	glFogCoordd = (PFNGLFOGCOORDDEXTPROC)wglGetProcAddress( "glFogCoordd" );

	MPB_ASSERT( glActiveTexture );
	MPB_ASSERT( glMultiTexCoord3f );
	MPB_ASSERT( glTexImage3D );
//	MPB_ASSERT( glFogCoordd );


	strcpy( m_fontTexture, "font2" );
	quadricObj = gluNewQuadric();
	gluQuadricNormals( quadricObj, GLU_SMOOTH );
	gluQuadricTexture( quadricObj, GL_TRUE );
	tesselator = gluNewTess();
	MPBOpenGL::errorCheck();

	clearColor.set( 0, .5, .5 );

	glClearColor( (float)clearColor.x, (float)clearColor.y, (float)clearColor.z, 1 );

//	glPolygonMode( GL_FRONT_AND_BACK, GL_LINE );

	glDisable( GL_BLEND );
	glDisable( GL_ALPHA_TEST );
	glDisable( GL_STENCIL_TEST );
	glEnable( GL_CULL_FACE );
	glColorMask( GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE );
	glEnableClientState( GL_VERTEX_ARRAY );
	glEnableClientState( GL_TEXTURE_COORD_ARRAY );
	glShadeModel( GL_SMOOTH );

#define    LIGHT_MODEL_COLOR_CONTROL_EXT 0x81F8
#define    SINGLE_COLOR_EXT             0x81F9
#define    SEPARATE_SPECULAR_COLOR_EXT  0x81FA
	glLightModeli( LIGHT_MODEL_COLOR_CONTROL_EXT, SEPARATE_SPECULAR_COLOR_EXT );

	MPBOpenGL::errorCheck();

	if (is2D())
	{
//		glEnable( GL_BLEND );
		glBlendFunc( GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA );
	}
	else
	{
		glClearDepth( 1.0 );
		glEnable( GL_DEPTH_TEST );
		glDepthFunc( GL_LESS );
		glHint( GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST );
		glDepthMask( GL_TRUE );
		MPBOpenGL::errorCheck();
	}

	getGLSettings();

	/////////////////////

	return m_hwnd;
}

void MPBOpenGL::changeWindowSize( int width, int height )
{
	m_screenWidth = width;
	m_screenHeight = height;
	setupProjectionTransformation();
}

int sortPlaneFrontToBack( const void* a, const void* b ) 
{
	MPBTexturedPlane* planeA = *(MPBTexturedPlane**)a;
	MPBTexturedPlane* planeB = *(MPBTexturedPlane**)b;

	MPBFLOAT closestVertexDistanceA = 10000000;
	closestVertexDistanceA = min( closestVertexDistanceA, (planeA->getLL() - MPBOpenGL::planeSortPosition).getLength() );
	closestVertexDistanceA = min( closestVertexDistanceA, (planeA->getLR() - MPBOpenGL::planeSortPosition).getLength() );
	closestVertexDistanceA = min( closestVertexDistanceA, (planeA->getUR() - MPBOpenGL::planeSortPosition).getLength() );
	closestVertexDistanceA = min( closestVertexDistanceA, (planeA->getUL() - MPBOpenGL::planeSortPosition).getLength() );

	MPBFLOAT closestVertexDistanceB = 10000000;
	closestVertexDistanceB = min( closestVertexDistanceB, (planeB->getLL() - MPBOpenGL::planeSortPosition).getLength() );
	closestVertexDistanceB = min( closestVertexDistanceB, (planeB->getLR() - MPBOpenGL::planeSortPosition).getLength() );
	closestVertexDistanceB = min( closestVertexDistanceB, (planeB->getUR() - MPBOpenGL::planeSortPosition).getLength() );
	closestVertexDistanceB = min( closestVertexDistanceB, (planeB->getUL() - MPBOpenGL::planeSortPosition).getLength() );

	if (closestVertexDistanceA > closestVertexDistanceB) return 1;
	if (closestVertexDistanceA < closestVertexDistanceB) return -1;

	return 0;
}

void MPBOpenGL::startGameLoop()
{
	SetForegroundWindow( m_hwnd );

	if (glFogCoordd) glFogCoordd( 0 );

	char buf[MAX_PATH];
	char dirbuf[MAX_PATH];

	if (m_pointLights.size()) 
		generateLightmapTexture( "generatedLightmap", "generatedLightmap1D", 128, 128 );

	generateParticleTexture( PARTICLE_TEXTURE );


	// load textures from texture directory

	if (NO_TEXTURES)
	{
	}
	else if (SINGLE_TEXTURE)
	{
		GLenum texId = GL_TEXTURE0;
		for (int i=0; i<MAX_TEXTURES; i++)
		{
			createTextureFromFile( texId++, m_textureDir, SINGLE_TEXTURE, 255, true );
		}
	}
	else
	{
		GLenum texId = GL_TEXTURE0;
		for (int i=0; i<MAX_TEXTURES; i++)
		{
			sprintf( dirbuf, "%s/GL_TEXTURE%d", m_textureDir, i );
			sprintf( buf, "%s/*.tex", dirbuf, i );
			WIN32_FIND_DATA findData;
			HANDLE handle = FindFirstFile( buf, &findData );

			while (handle != INVALID_HANDLE_VALUE)
			{
				BYTE alpha = 255;

				bool mipmap = true;
				if (texId > GL_TEXTURE0) mipmap = false;
				strcpy( buf, findData.cFileName );
				buf[ strlen( buf ) - 4 ] = 0;
				createTextureFromFile( texId, dirbuf, buf, alpha, mipmap );
				if (!FindNextFile( handle, &findData )) break;
			}

			FindClose( handle );
			texId++;
		}
	}

	createFonts();

	MPBGameEntities::startGameLoop();
	MPBFrameCounter::startGameLoop();

	// radiosity

	if (0)
	{
		MPBList sourcePlanes;
		MPB_LIST_LOOP( MPBGameEntities::planes, plane, MPBTexturedPlane* )
		{
			plane->setRadiosity( plane->getEmissivity() );
			sourcePlanes.push_back( plane );
		}


		for (int i=0; i<MPBGameEntities::planes.size(); i++)
		{
			if (i % 10 == 0)
			{
				sprintf( g_logBuf, "Shooting from plane %d of %d", i, MPBGameEntities::planes.size() );
				MPBMessageWindow::writeMessage();
			}

			// find plane with highest radiosity
			MPBFLOAT highest = 0;
			MPBTexturedPlane* sourcePlane = NULL;
			MPB_LIST_LOOP( sourcePlanes, plane, MPBTexturedPlane* )
			{
				if (plane->getRadiosity() > highest)
				{
					sourcePlane = plane;
					highest = plane->getRadiosity();
				}
			}

			if (sourcePlane)
			{
				sourcePlanes.remove( sourcePlane );

				MPB_LIST_LOOP( MPBGameEntities::planes, destPlane, MPBTexturedPlane* )
				{
					if (destPlane == sourcePlane) continue;

					MPBVector sourceToDest = destPlane->getCenter() - sourcePlane->getCenter();
					MPBVector destToSource = sourcePlane->getCenter() - destPlane->getCenter();
					MPBFLOAT distance = sourceToDest.getLength() / 1.7f;
					sourceToDest.normalize();
					destToSource.normalize();
					MPBFLOAT destAngle = destPlane->getNormal().dotProduct( destToSource );
					MPBFLOAT sourceAngle = sourcePlane->getNormal().dotProduct( sourceToDest );

					if (destAngle >= 0.0 && sourceAngle >= 0.0)
					{
						MPBFLOAT relativeAngle = destAngle * sourceAngle;
						MPBFLOAT differentialArea = sourcePlane->getArea() / destPlane->getArea();
						MPBFLOAT visibility = 1.0;
//						if (pathIsClear( sourcePlane->getCenter(), destPlane->getCenter(), .1, false, NULL, NULL ))
//							visibility = 1.0;

						MPBFLOAT formFactor = 
							relativeAngle * differentialArea * visibility /
							(PI * distance * distance);

						destPlane->setRadiosity( 
							destPlane->getRadiosity() + 
							destPlane->getReflectivity() *
							sourcePlane->getRadiosity() * formFactor
						);
					}
				}
			}
		}
	}


	// create shadow volumes

	if (!m_pointLights.size() && m_staticPointLights.size())
	{
		if (USE_LIGHT_VOLUMES)
		{
			m_displayListShadowVolume = glGenLists( 1 );
			glNewList( m_displayListShadowVolume, GL_COMPILE );
			glBegin( GL_TRIANGLES );

			MPB_LIST_LOOP( MPBGameEntities::cells, cell, MPBCompositeShapeCell* )
			{
				if (cell->m_light)
				{
					MPBList shapes;
					cell->getShapes( shapes );

					MPBList planes;
					MPB_LIST_LOOP( shapes, shape, MPBShape* )
					{
//						if (!shape->getCastsShadow()) continue;
						if (!shape->getDraw()) continue;

						shape->getPlanes( planes );
					}

					planeSortPosition = cell->m_light->getPosition();
//					planes.sort( sortPlaneFrontToBack );

					MPBList triangles;
					MPBList shadowWedges;
					MPBList lightWedges;

					// convert planes to triangles

					MPB_LIST_LOOP( planes, p1, MPBTexturedPlane* )
					{
						// plane casts no shadow if light is inside plane
						if (p1->isPointOutside( cell->m_light->getPosition() ))
						{
							MPBTriangle* t1 = new MPBTriangle;
							MPBTriangle* t2 = new MPBTriangle;
							p1->getTriangles( *t1, *t2 );

							triangles.push_back( t1 );
							triangles.push_back( t2 );
						}
					}

					// find shadow wedge for each triangle

					MPB_LIST_LOOP( triangles, tri, MPBTriangle* )
					{
						MPBVector nudge = tri->getNormal() * -1;
						nudge.setLength( .01f );

						tri->vertex[0] = tri->vertex[0] + nudge;
						tri->vertex[1] = tri->vertex[1] + nudge;
						tri->vertex[2] = tri->vertex[2] + nudge;

						MPBList* polyPlanes = new MPBList;
						polyPlanes->push_back( new MPBTriangle( tri->vertex[0], tri->vertex[1], tri->vertex[2] ) );
						polyPlanes->push_back( new MPBTriangle( tri->vertex[0], tri->vertex[1], cell->m_light->getPosition() ) );
						polyPlanes->push_back( new MPBTriangle( tri->vertex[1], tri->vertex[2], cell->m_light->getPosition() ) );
						polyPlanes->push_back( new MPBTriangle( tri->vertex[2], tri->vertex[0], cell->m_light->getPosition() ) );
						shadowWedges.push_back( polyPlanes );
					}

					// find light wedge for each triangle that isn't occluded

					int t = -1;
					{MPB_LIST_LOOP( triangles, tri, MPBTriangle* )
					{
						t++;
						bool occluded = false;
						int s = -1;
						MPB_LIST_LOOP( shadowWedges, sw, MPBList* )
						{
							s++;
							if (s == t) break;	// don't test triangle against its own shadow wedge
							if (occluded) break;

							// test each point of this triangle
							// if it is outside any plane of the shadow wedge, it's not occluded
							for (int i=0; i<3; i++)
							{
								// each plane of the shadow wedge
								occluded = true;
								MPB_LIST_LOOP( *sw, tri2, MPBTriangle* )
								{
									if (tri2->distanceToPoint( tri->vertex[i] ) > -.1)
									{
										occluded = false;
										break;
									}
									else	// point is in shadow
									{
									}
								}

								if (occluded)	// point is occluded by this shadow wedge
								{
									int j = i + 1;
									if (j > 2) j = 0;

									MPB_LIST_LOOP( *sw, tri3, MPBTriangle* )
									{
										MPBVector intersection;
										if (tri3->rayIntersect( tri->vertex[i], tri->vertex[j], intersection ))
										{
											int x = 0;
										}
									}
								}
							}

							if (occluded)	// triangle is occluded by this shadow wedge
							{
							}
						}

						if (occluded) continue;


						MPBVector nudge = tri->getNormal() * -1;
						nudge.setLength( .01f );

						tri->vertex[0] = tri->vertex[0] + nudge;
						tri->vertex[1] = tri->vertex[1] + nudge;
						tri->vertex[2] = tri->vertex[2] + nudge;

						MPBList* polyPlanes = new MPBList;
						polyPlanes->push_back( new MPBTriangle( tri->vertex[0], tri->vertex[2], tri->vertex[1] ) );
						polyPlanes->push_back( new MPBTriangle( tri->vertex[0], tri->vertex[1], cell->m_light->getPosition() ) );
						polyPlanes->push_back( new MPBTriangle( tri->vertex[1], tri->vertex[2], cell->m_light->getPosition() ) );
						polyPlanes->push_back( new MPBTriangle( tri->vertex[2], tri->vertex[0], cell->m_light->getPosition() ) );
						lightWedges.push_back( polyPlanes );
					}}

					// draw light wedges

					int i = 0;
					MPB_LIST_LOOP( lightWedges, ph, MPBList* )
					{
						MPB_LIST_LOOP( *ph, tri3, MPBTriangle* )
						{
							tri3->vertex[0].drawAsVertex();
							tri3->vertex[1].drawAsVertex();
							tri3->vertex[2].drawAsVertex();
						}
//						if (i++ > 10) break;
					}

					MPB_LIST_DELETE( triangles, MPBTriangle* );
					MPB_LIST_DELETE( lightWedges, Polyhedron* );
				}
			}

			glEnd();
			glEndList();
		}
		else if (SHADOW_VOLUME_CELLS)
		{
			MPB_LIST_LOOP( MPBGameEntities::cells, cell, MPBCompositeShapeCell* )
			{
				if (cell->m_light)
				{
					MPBList shapes;
					cell->getShapes( shapes );

					MPB_LIST_LOOP( shapes, shape, MPBShape* )
					{
						shape->createShadowVolume( &(cell->getAABB()), cell->m_light->getPosition() );
					}

					GLuint l = glGenLists( 1 );
					glNewList( l, GL_COMPILE );
					cell->setShadowVolumeDisplayList( l );

					MPB_LIST_LOOP( shapes, shape2, MPBShape* )
					{
						shape2->drawShadowVolume();
					}

					glEndList();
				}
			}
		}
		else
		{
			createShadowVolume();

			m_displayListShadowVolume = glGenLists( 1 );
			glNewList( m_displayListShadowVolume, GL_COMPILE );

			MPB_LIST_LOOP( MPBGameEntities::shapes, shape, MPBShape* )
			{
				shape->drawShadowVolume();
			}

			glEndList();
		}



		// create light maps
		if (USE_LIGHT_MAPS)
		{
			glDisable( GL_TEXTURE_1D );
			glDisable( GL_TEXTURE_2D );
			glDisable( GL_TEXTURE_3D );
			glColor4f( 1, 1, 1, 1 );

			if (1)	// use openGL lighting
			{
				// set up light0 params

				glEnable( GL_LIGHTING );

				int glLight = GL_LIGHT0;
				MPB_LIST_LOOP( m_staticPointLights, po, MPBPhysicsObject* )
				{
					MPBVector light = po->getPosition();
					GLfloat position[] = { (GLfloat)light.x, (GLfloat)light.y, (GLfloat)light.z, 1 };
					glLightfv( glLight, GL_POSITION, position );

					GLfloat ambient[]= { 1, 1, 1, 1 };
					GLfloat diffuse[]= { 1, 1, 1, 1 };
					GLfloat specular[]= { .5, .5, .5, 1 };
					glLightfv( glLight, GL_AMBIENT, ambient );
					glLightfv( glLight, GL_DIFFUSE, diffuse );
					glLightfv( glLight, GL_SPECULAR, specular );

					glEnable( glLight );
					glLight++;
				}
			}

			int j = 0;
			MPB_LIST_LOOP( MPBGameEntities::shapes, shape, MPBShape* )
			{
				if (!shape->getReceivesShadow()) continue;

//				MPBTexturedPlane* plane = (MPBTexturedPlane*)(shape->getPlane( 0 ));
				int i = 0;
//				while (plane = shape->getPlanes())
				MPB_LIST_LOOP( shape->getPlanes(), plane, MPBTexturedPlane* )
				{
					glClear( GL_COLOR_BUFFER_BIT | GL_STENCIL_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );
					glEnable( GL_DEPTH_TEST );

					MPBVector lookAt = plane->getCenter();
					MPBVector camera = lookAt + plane->getNormal();
					MPBVector up( 0, 1, 0 );
					if (camera.x == lookAt.x && camera.z == lookAt.z)
					{
						if (camera.y > lookAt.y)
							up.set( 0, 0, -1 );
						else
							up.set( 0, 0, 1 );
					}

					MPBFLOAT width = (plane->getLR() - plane->getLL()).getLength() / 2;
					MPBFLOAT height = (plane->getUL() - plane->getLL()).getLength() / 2;

					int mapWidth = 64;
					int mapHeight = 64;

					glViewport( 0, 0, (int)mapWidth, (int)mapHeight );

					glMatrixMode( GL_PROJECTION );
					glLoadIdentity();

					glOrtho(	-width,		// left
								width,		// right
								-height,		// bottom
								height,		// top
								.1,			// near
								10	);			// far


					gluLookAt(	camera.x, camera.y, camera.z,
									lookAt.x, lookAt.y, lookAt.z,
									up.x, up.y, up.z	);

					plane->draw( false, true );

//					drawStencilShadows();

					int size = 3 * mapWidth * mapHeight;
					BYTE* pixels = new BYTE[ size ];
					glReadPixels(	0,
										0,
										mapWidth,
										mapHeight,
										GL_RGB,
										GL_UNSIGNED_BYTE,
										pixels	);

					BYTE* reds = new BYTE[ mapWidth * mapHeight ];
					BYTE* greens = new BYTE[ mapWidth * mapHeight ];
					BYTE* blues = new BYTE[ mapWidth * mapHeight ];
					BYTE* grays = new BYTE[ mapWidth * mapHeight ];
					for (int p=0; p<mapWidth * mapHeight; p++)
					{
						reds[p] = pixels[ p * 3 ];
						greens[p] = pixels[ p * 3 + 1 ];
						blues[p] = pixels[ p * 3 + 2 ];
						grays[p] = (int)((pixels[ p * 3 ] + pixels[ p * 3 + 1 ] + pixels[ p * 3 + 2 ]) / 3.0);
					}

					char texName[1000];
					sprintf( texName, "lightmaps/lightmap_%d_%d", j, i );
					writeGrayscaleBitmap( texName, grays, mapWidth, mapHeight );

					createTextureFromMemory( texName, reds, greens, blues, NULL, mapWidth, mapHeight, 1, GL_CLAMP, true );
					plane->addTexture( texName );

					delete[] pixels;
					delete[] grays;
					delete[] reds;
					delete[] greens;
					delete[] blues;
					i++;

		//			SwapBuffers( m_hdc );
		//			Sleep( 1000 );
				}

				j++;
			}
		}
	}

	if (USE_SHADOW_TEXTURES)
	{
		glGenTextures( 1, &m_shadowMapTextureID );
		glBindTexture( GL_TEXTURE_2D, m_shadowMapTextureID );
		glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR );
		glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR );

		MPB_LIST_LOOP( MPBGameEntities::planes, plane, MPBTexturedPlane* )
		{
			plane->addTexture( "generatedShadowTexture" );
		}
	}

	if (ENV_TEXTURE)
	{
		int mapWidth = 512;
		int mapHeight = 512;
		int size = 3 * mapWidth * mapHeight;
		BYTE* pixels = new BYTE[ size ];

		BYTE* reds = new BYTE[ mapWidth * mapHeight ];
		BYTE* greens = new BYTE[ mapWidth * mapHeight ];
		BYTE* blues = new BYTE[ mapWidth * mapHeight ];

		for (int p=0; p<mapWidth * mapHeight; p++)
		{
			reds[p] = 255;
			greens[p] = 0;
			blues[p] = 255;
		}

		createTextureFromMemory( "renderToTexture", reds, greens, blues, NULL, mapWidth, mapHeight, 1, GL_REPEAT, false );
	}
}

void MPBOpenGL::setNavMesh( MPBNavigationMesh* nm ) 
{ 
	m_navMesh = nm; 
	m_navMesh->startGameLoop(); 
}

// writes settings to log file
void MPBOpenGL::getGLSettings()
{
	char buf[1000];
	const unsigned char* str;

	writeLog( "------------- GL Settings -------------\n" );

	GLboolean b[1];
	GLint i[1];

	glGetBooleanv( GL_DOUBLEBUFFER, b );
	sprintf( buf, "floatBUFFER: %d\n", b[0] );
	writeLog( buf );

	glGetIntegerv( GL_AUX_BUFFERS, i );
	sprintf( buf, "AUX_BUFFERS: %d\n", i[0] );
	writeLog( buf );

	glGetIntegerv( GL_STENCIL_BITS, i );
	sprintf( buf, "GL_STENCIL_BITS: %d\n", i[0] );
	writeLog( buf );

	str = glGetString( GL_VENDOR );
	sprintf( buf, "GL_VENDOR: %s\n", str );
	writeLog( buf );

	str = glGetString( GL_RENDERER );
	sprintf( buf, "GL_RENDERER: %s\n", str );
	writeLog( buf );

	str = glGetString( GL_VERSION );
	sprintf( buf, "GL_VERSION: %s\n", str );
	writeLog( buf );

	glGetIntegerv( GL_MAX_LIGHTS, i );
	sprintf( buf, "GL_MAX_LIGHTS: %d\n", i[0] );
	writeLog( buf );

	glGetIntegerv( GL_MAX_TEXTURE_SIZE, i );
	sprintf( buf, "GL_MAX_TEXTURE_SIZE: %d\n", i[0] );
	writeLog( buf );

	str = glGetString( GL_EXTENSIONS );
	sprintf( buf, "GL_EXTENSIONS:\n" );
	writeLog( buf );
	writeLog( "   " );

	int j = 0;
	while (char c = str[j++])
	{
		if (c == ' ')
		{
			writeLog( "\n" );
			writeLog( "   " );
		}
		else
		{
			sprintf( buf, "%c", c );
			writeLog( buf );
		}
	}

	writeLog( "\n" );
	writeLog( "---------------------------------------\n" );
}

void MPBOpenGL::cleanUp()
{
	MPB_LIST_DELETE( m_staticPointLights, MPBPhysicsObject* );

	if (m_navMesh) delete m_navMesh;

	if (m_hdc) 
	{
		ReleaseDC( m_hwnd, m_hdc );
		m_hdc = NULL;
	}

	if (m_hwnd)
	{
		DestroyWindow( m_hwnd );
		m_hwnd = NULL;
	}

	for (int i=0; i<MAX_TEXTURES; i++)
	{
		MPB_LIST_DELETE( m_textures[i], MPBTexture* );
	}
}

void MPBOpenGL::switchFullScreen()
{
}

void MPBOpenGL::setupProjectionTransformation()
{
	glViewport( 0, 0, m_screenWidth, m_screenHeight );

	glMatrixMode( GL_PROJECTION );
	glLoadIdentity();

	if (is2D())
	{
		MPBFLOAT width = m_screenWidth / PIXELS_PER_METER_2D;
		MPBFLOAT height = m_screenHeight / PIXELS_PER_METER_2D;

		glOrtho(	-width,		// left
					width,		// right
					-height,		// bottom
					height,		// top
					m_perspectiveNear,			// near
					m_perspectiveFar	);			// far
	}
	else if (m_renderFromLight)		// render from light's POV
	{
		MPBFLOAT width = m_screenWidth / 20.0f;
		MPBFLOAT height = m_screenHeight / 20.0f;

		glOrtho(	-width,		// left
					width,		// right
					-height,		// bottom
					height,		// top
					m_perspectiveNear,			// near
					m_perspectiveFar	);			// far
	}
	else
	{
		MPB_ASSERT( m_perspectiveNear > 0.0f );
		MPB_ASSERT( m_perspectiveFar > m_perspectiveNear );

		gluPerspective(
			m_perspectiveAngle,					// FOV angle
			(GLfloat)m_screenWidth / (GLfloat)m_screenHeight,	// aspect ratio
			m_perspectiveNear,					// near
			m_perspectiveFar	);					// far
	}

//	MPBOpenGL::errorCheck();
}

void MPBOpenGL::resetCamera()
{
	glMatrixMode( GL_MODELVIEW );
	glLoadIdentity();

	if (0)//(is2D())
	{
		gluLookAt(	.01, .02, 1,		// camera pos
						0, 0, 0,		// look at
						0, 1, 0		// up vector
					);
	}
	else if (m_renderFromLight)		// render from light's POV
	{
		MPBVector light = ((MPBPhysicsObject*)m_staticPointLights[0])->getPosition();
		gluLookAt(	light.x, light.y, light.z,		// camera pos
						0, 0, 0,		// look at
						0, 1, 0		// up vector
					);
	}
	else
	{
		gluLookAt(	m_cameraPos.x, 
						m_cameraPos.y, 
						m_cameraPos.z,		// camera pos
						m_lookAt.x,
						m_lookAt.y,
						m_lookAt.z,							// look at
						0, 1, 0		// up vector
					);
	}

//	MPBOpenGL::errorCheck();
}

void MPBOpenGL::disableFog()
{
	m_fogEnabled = false;
}

void MPBOpenGL::enableFog( MPBFLOAT red, MPBFLOAT green, MPBFLOAT blue, MPBFLOAT density )
{
	m_fogRed = red;
	m_fogGreen = green;
	m_fogBlue = blue;
	m_fogDensity = density;
	m_fogEnabled = true;
}

void MPBOpenGL::setFogCoord( MPBVector point )
{
//	if (glFogCoordd) glFogCoordd( max( 0, GROUND_FOG_Y - point.y ) );
}


MPBCountdown*					m_countdownScreenshot = NULL;
int								m_indexScreenshot = 0;

void MPBOpenGL::drawSceneTest()
{
	glClear( GL_COLOR_BUFFER_BIT | GL_STENCIL_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );
	setupProjectionTransformation();
	resetCamera();
	extractFrustum();

	SwapBuffers( m_hdc );
}

void MPBOpenGL::drawScene( void(*customDraw)() )
{
	if (0)
	{
		drawSceneTest();
		return;
	}

	if (USE_SHADOW_TEXTURES) makeShadowMaps();


/*	if (m_countdownScreenshot == NULL)
	{
		m_countdownScreenshot = new MPBCountdown( 2000, false );
	}
	else
	{
		if (m_countdownScreenshot->isDone())
		{
			char buf[1000];
			sprintf( buf, "screenshot_%d.raw", m_indexScreenshot );
			m_indexScreenshot++;
			takeScreenShot( buf );

			m_countdownScreenshot->start( 2000 );
		}
	}
*/

	if (!m_noShadows && m_pointLights.size()) createShadowVolume();

	glClearColor( (float)clearColor.x, (float)clearColor.y, (float)clearColor.z, 1 );
	glClear( GL_COLOR_BUFFER_BIT | GL_STENCIL_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );
	glColorMask( GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE );
	setupProjectionTransformation();
	resetCamera();
	extractFrustum();

	glDepthMask( GL_TRUE );
	glEnable( GL_CULL_FACE );
	glCullFace( GL_BACK );
	glEnable( GL_DEPTH_TEST );
	if (glFogCoordd) glFogCoordd( 0 );

	if (NO_TEXTURES)
	{
		glDisable( GL_TEXTURE_1D );
		glDisable( GL_TEXTURE_2D );
		glDisable( GL_TEXTURE_3D );
	}

	if (0)//(USE_SHADOW_TEXTURES)
	{
		glActiveTexture( GL_TEXTURE1 );

		static GLfloat s_plane[] = { 1.0, 0.0, 0.0, 0.0 };
		static GLfloat t_plane[] = { 0.0, 1.0, 0.0, 0.0 };
		static GLfloat r_plane[] = { 0.0, 0.0, 1.0, 0.0 };
		static GLfloat q_plane[] = { 0.0, 0.0, 0.0, 1.0 };

		glTexGeni( GL_S, GL_TEXTURE_GEN_MODE, GL_EYE_LINEAR );
		glTexGeni( GL_T, GL_TEXTURE_GEN_MODE, GL_EYE_LINEAR );
		glTexGeni( GL_R, GL_TEXTURE_GEN_MODE, GL_EYE_LINEAR );
		glTexGeni( GL_Q, GL_TEXTURE_GEN_MODE, GL_EYE_LINEAR );

		glTexGenfv( GL_S, GL_EYE_PLANE, s_plane );
		glTexGenfv( GL_T, GL_EYE_PLANE, t_plane );
		glTexGenfv( GL_R, GL_EYE_PLANE, r_plane );
		glTexGenfv( GL_Q, GL_EYE_PLANE, q_plane );

		glEnable( GL_TEXTURE_GEN_S );
		glEnable( GL_TEXTURE_GEN_T );
		glEnable( GL_TEXTURE_GEN_R );
		glEnable( GL_TEXTURE_GEN_Q );

//		glMatrixMode( GL_TEXTURE );
//		glLoadIdentity();
//		glTranslatef( 0, 0, 0 );
//		glMatrixMode( GL_MODELVIEW );
	}


	//////////////////////////////
	// fog

	if (m_fogEnabled)
	{
		glHint( GL_FOG_HINT, GL_DONT_CARE );
		GLfloat fogColor[4] = { (GLfloat)m_fogRed, (GLfloat)m_fogGreen, (GLfloat)m_fogBlue, 1 };

		glFogi( GL_FOG_MODE, GL_EXP2 );
//		glFogi( GL_FOG_MODE, GL_LINEAR );
//		glFogf( GL_FOG_START, 0 );		// only used by linear fog
//		glFogf( GL_FOG_END, 1 );		// only used by linear fog

		glFogfv( GL_FOG_COLOR, fogColor );
		glFogf( GL_FOG_DENSITY, (GLfloat)m_fogDensity );
		if (USE_FOG_COORDS) glFogi( GL_FOG_COORDINATE_SOURCE_EXT, GL_FOG_COORDINATE_EXT );

		glEnable( GL_FOG );
	}
	else
	{
		glDisable( GL_FOG );
	}

	if (USE_OPENGL_LIGHTING)
	{
		glEnable( GL_LIGHTING );

		if (m_pointLights.size())
		{
			MPBVector pos = ((MPBPhysicsObject*)m_pointLights[0])->getPosition();
			GLfloat position[] = { (GLfloat)pos.x, (GLfloat)pos.y, (GLfloat)pos.z, 1 };	// positional light
			glLightfv( GL_LIGHT0, GL_POSITION, position );

			GLfloat ambient[]= { 1, 1, 1, 1 };
			GLfloat diffuse[]= { 1, 1, 1, 1 };
			GLfloat specular[]= { .5, .5, .5, 1 };
			glLightfv( GL_LIGHT0, GL_AMBIENT, ambient );
			glLightfv( GL_LIGHT0, GL_DIFFUSE, diffuse );
			glLightfv( GL_LIGHT0, GL_SPECULAR, specular );

			glEnable( GL_LIGHT0 );

			glLightf( GL_LIGHT0, GL_CONSTANT_ATTENUATION, 1.0 );		// default = 1
			glLightf( GL_LIGHT0, GL_LINEAR_ATTENUATION, .5 );			// default = 0
//			glLightf( GL_LIGHT0, GL_QUADRATIC_ATTENUATION, 1 );	// default = 0

//			glLightf( GL_LIGHT0, GL_SPOT_CUTOFF, 60 );	// default = 180 degrees
//			GLfloat direction[] = { 1, 0, 0 };
//			glLightfv( GL_LIGHT0, GL_SPOT_DIRECTION, direction );
		}
		else if (m_staticPointLights.size())
		{
			int glLight = GL_LIGHT0;
			MPB_LIST_LOOP( m_staticPointLights, po, MPBPhysicsObject* )
			{
				MPBVector light = po->getPosition();
				GLfloat position[] = { (GLfloat)light.x, (GLfloat)light.y, (GLfloat)light.z, 1 };	// directional light
				glLightfv( glLight, GL_POSITION, position );

				GLfloat ambient[]= { .2, .2, .2, 1 };
				GLfloat diffuse[]= { 1, 1, 1, 1 };
				GLfloat specular[]= { 1, 1, 1, 1 };
				glLightfv( glLight, GL_AMBIENT, ambient );
				glLightfv( glLight, GL_DIFFUSE, diffuse );
				glLightfv( glLight, GL_SPECULAR, specular );

//				glLightf( glLight, GL_CONSTANT_ATTENUATION, 3.0 );		// default = 1
//				glLightf( glLight, GL_LINEAR_ATTENUATION, .5 );			// default = 0
//				glLightf( glLight, GL_QUADRATIC_ATTENUATION, 1 );		// default = 0

				glEnable( glLight );
				glLight++;
			}
		}
		else
		{
			glDisable( GL_LIGHTING );
		}
	}
	else
	{
		glDisable( GL_LIGHTING );
	}

//	MPBOpenGL::errorCheck();

	if (is2D())
	{
		glDisable( GL_LIGHTING );
		glDisable( GL_STENCIL_TEST );
		glDisable( GL_DEPTH_TEST );
		glDisable( GL_CULL_FACE );

		resetCamera();
		MPBGameEntities::draw( false );
		clearTextures();

		glEnable( GL_BLEND );
		resetCamera();
		MPBPhysics::draw( false );
		glDisable( GL_BLEND );

/*		MPBOpenGL::bindTexture( "solidWhite" );
		glColor4ub( 255, 0, 0, 255 );
		glLineWidth( 10 );
		glBegin( GL_LINES );
		glVertex3d( 0, 0, 0 );
		glVertex3d( 1, 40, 0 );
		glEnd();
*/	}
	else
	{
		glDisable( GL_STENCIL_TEST );
		glEnable( GL_CULL_FACE );
		glCullFace( GL_BACK );
		glEnable( GL_BLEND );
		glBlendFunc( GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA );

		glColor4f( 1, 1, 1, 1 );
		clearTextures();

/*		if (ENV_TEXTURE) // not working
		{
			glViewport( 0, 0, 512, 512 );
			glMatrixMode( GL_PROJECTION );
			glLoadIdentity();
			gluPerspective( m_perspectiveAngle, 1, m_perspectiveNear, m_perspectiveFar );

			// draw mirror image of scene

			m_mirror = true;
			glCullFace( GL_FRONT );

			MPBGameEntities::draw( false );
			clearTextures();

			glColor4f( 1, 1, 1, 1 );
			MPBPhysics::draw( false );
			clearTextures();

			// copy mirror image of scene to texture

			int mapWidth = 512;
			int mapHeight = 512;
			bindTexture( "renderToTexture" );
			glCopyTexImage2D( GL_TEXTURE_2D, 0, GL_RGBA, 0, 0, mapWidth, mapHeight, 0 );

			// draw mirror surfaces

			m_mirror = false;
			setupProjectionTransformation();
			glCullFace( GL_BACK );
			glDisable( GL_STENCIL_TEST );
			glClear( GL_COLOR_BUFFER_BIT | GL_STENCIL_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );

			MPBGameEntities::draw( false );
			clearTextures();
		}
*/

		if (1)	// handle mirrors
		{
			MPB_LIST_LOOP( MPBGameEntities::mirrors, mirror, MPBShapeQuad* )
			{
				// draw mirror surface into SB

				glClear( GL_STENCIL_BUFFER_BIT );
				glEnable( GL_STENCIL_TEST );
				glStencilFunc( GL_ALWAYS, 1, ~0 );
				glStencilOp( GL_REPLACE, GL_REPLACE, GL_REPLACE );
				glColorMask( GL_FALSE, GL_FALSE, GL_FALSE, GL_FALSE );
				glDepthMask( GL_FALSE );
				glCullFace( GL_BACK );

				resetCamera();

				mirror->setDrawn( false );
				mirror->draw( false, false, true );

				// draw mirror image of scene

				glCullFace( GL_FRONT );
				glEnable( GL_STENCIL_TEST );
				glDepthMask( GL_TRUE );
				glColorMask( GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE );
				glStencilFunc( GL_EQUAL, 1, ~0 );
				glStencilOp( GL_KEEP, GL_KEEP, GL_KEEP );

				Plane* p = mirror->getPlane( 0 );
				MPBVector n = p->getNormal();
				MPBFLOAT d = p->getD();

				resetCamera();
				reflectAboutPlane( p );
				extractFrustum();

				glEnable( GL_CLIP_PLANE0 );  // Turn on the clipping plane
//				GLdouble planeEqn[4] = { 0.0, 1.0, 0.0, 0.0 };
				GLdouble planeEqn[4] = { n.x, n.y, n.z, d };
				glClipPlane( GL_CLIP_PLANE0, planeEqn );  // Define the clipping plane

//				glScalef( 1, -1, 1 );
//				glTranslated( 0, 2, 0 );

				MPBGameEntities::draw( false );
				clearTextures();

				resetCamera();
				reflectAboutPlane( p );
				extractFrustum();

				glColor4f( 1, 1, 1, 1 );
				MPBPhysics::draw( false );
				clearTextures();

				// draw mirror surface into DB and CB

				glDisable( GL_CLIP_PLANE0 );
				glDisable( GL_STENCIL_TEST );
				glCullFace( GL_BACK );

				resetCamera();
				extractFrustum();
				mirror->setDrawn( false );
				mirror->draw( true, false, true );
			}
		}

		// draw scene normally

		resetCamera();
		extractFrustum();
		MPBGameEntities::draw( false );
		clearTextures();

		glColor4f( 1, 1, 1, 1 );
		resetCamera();
		MPBPhysics::draw( false );
		resetCamera();
		MPBPhysics::drawParticleSystems( true );
		clearTextures();


		// shadow volumes
		if (!m_noShadows)
		{
			resetCamera();
			drawStencilShadows( OBJECT_SHADOW_VOLUMES, SHAPE_SHADOW_VOLUMES );
		}

		// planar shadows
		if (DRAW_PROJECTION_SHADOWS)
		{
			MPBPhysicsObject* light = getClosestStaticPointLight( m_cameraPos );
			if (light) 
			{
				resetCamera();
				MPBPhysics::drawProjectionShadows( light->getPosition() );
			}
			clearTextures();
		}

		clearTextures();
		resetCamera();
		MPBPhysics::drawParticleSystems( false );

//		if (m_navMesh) m_navMesh->draw();
	}

	customDraw();

	if (glFogCoordd) glFogCoordd( 0 );

	///////////////////////////////////////
	// show FPS

	if (!is2D())
	{
		static calls = 0;
		static MPBFLOAT framesPerSecond = 1000.0f / MPBFrameCounter::msPerFrame;//framesPerSecond;
		if (calls++ > 30)
		{
			calls = 0;
			framesPerSecond = 1000.0f / MPBFrameCounter::msPerFrame;//framesPerSecond
		}

		controlPanelBegin();
		controlPanelPosition( 0, 0 );

		char buf[1000];
		sprintf( buf, "%4.2f", framesPerSecond );
		printString( buf, 0 );

		controlPanelEnd();
	}

	if (!SINGLE_TEXTURE && !NO_TEXTURES && m_drawCrosshairs) drawCrosshairs();

	MPBGameEntities::drawControlPanels();

	SwapBuffers( m_hdc );
}

// draws the (precomputed) shadow volume into the stencil buffer.
// sets stencil buffer to non-zero wherever shadow falls.
// typical usage: 
//  1. draw scene in light where stencil buffer == 0
//  2. drawShadowStencil()
//  3. draw scene again in shadow or draw translucent gray quad where stencil buffer != 0
void MPBOpenGL::drawStencilShadows( bool objects, bool planes )
{
	///////////////////////////////////////
	// draw shadow volume in the stencil buffer

	glPushAttrib( GL_ALL_ATTRIB_BITS );

	glClear( GL_STENCIL_BUFFER_BIT );
	glDisable( GL_LIGHTING );
	glDisable( GL_TEXTURE_1D );
	glDisable( GL_TEXTURE_2D );
	glDisable( GL_TEXTURE_3D );

	glEnable( GL_STENCIL_TEST );
	glStencilFunc( GL_ALWAYS, 0, 0 );

	glEnable( GL_DEPTH_TEST );
	glDepthMask( GL_FALSE );	// don't let the shadow volume affect the depth buffer
	glDepthFunc( GL_LESS );

	glColorMask( GL_FALSE, GL_FALSE, GL_FALSE, GL_FALSE );	// do not actually draw the shadow volume

	if (0)	// draw shadow volume and leave
	{
		glDisable( GL_CULL_FACE );
		glColorMask( GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE );
		glColor4f( 1, 0, 0, .2f );
		glEnable( GL_BLEND );
		drawShadowVolumes( objects, planes );
		glPopAttrib();
		return;
	}

	if (0)	// draw shadow volume
	{
		glDisable( GL_CULL_FACE );
		glColorMask( GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE );
		glColor4f( 1, 0, 0, .2f );
		glEnable( GL_BLEND );
		glDisable( GL_DEPTH_TEST );
		drawShadowVolumes( objects, planes );
		glEnable( GL_DEPTH_TEST );
	}


	glEnable( GL_CULL_FACE );

	// frontface hidden? increment
	glCullFace( GL_FRONT );
	glStencilOp( GL_KEEP, GL_INCR, GL_KEEP );

	drawShadowVolumes( objects, planes );

	// backface hidden? decrement
	glCullFace( GL_BACK );
	glStencilOp( GL_KEEP, GL_DECR, GL_KEEP );

	drawShadowVolumes( objects, planes );

	///////////////////////////////////////

	MPB_LIST_LOOP( MPBGameEntities::cells, cell, MPBCompositeShapeCell* )
	{
		if (cell->m_light)
		{
//			cell->getAABB().draw();
		}
	}

	glPopAttrib();

	glPushAttrib( GL_ALL_ATTRIB_BITS );
	glColorMask( GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE );
	glEnable( GL_STENCIL_TEST );
	glStencilOp( GL_KEEP, GL_KEEP, GL_KEEP );
	glDisable( GL_TEXTURE_1D );
	glDisable( GL_TEXTURE_2D );
	glDisable( GL_TEXTURE_3D );


	if (USE_LIGHT_VOLUMES) glStencilFunc( GL_EQUAL, 0, ~0 );	// only draw where the stencil was not set (==0)
	else glStencilFunc( GL_NOTEQUAL, 0, ~0 );	// only draw where the stencil was set (!=0)

	if (GRAY_QUAD_SHADOW_VOLUMES)
	{
		///////////////////////////////////////
		// draw a translucent gray square over the screen
		// wherever stencil buffer is not 0

		controlPanelBegin();
		controlPanelPosition( .5, .5 );
		int width = getScreenWidth() * 1;
		int height = getScreenHeight() * 1;
		glColor4f( 0, 0, 0, .4f );
		glBegin( GL_QUADS );
		glVertex2d( -width/2,	-height/2 );
		glVertex2d(  width/2,	-height/2 );
		glVertex2d(  width/2,	 height/2 );
		glVertex2d( -width/2,	 height/2 );
		glEnd();
		controlPanelEnd();
	}
	else
	{
		// 2nd pass, lights out
		MPB_ASSERT( USE_OPENGL_LIGHTING );
		glClear( GL_DEPTH_BUFFER_BIT );

		if (m_staticPointLights.size())
		{
			int glLight = GL_LIGHT0;
			MPB_LIST_LOOP( m_staticPointLights, po, MPBPhysicsObject* )
			{
				glDisable( glLight );
				glLight++;
			}
		}

		resetCamera();
		MPBGameEntities::draw( false );
		clearTextures();

		glColor4f( 1, 1, 1, 1 );
		resetCamera();
		MPBPhysics::draw( false );
		resetCamera();
		MPBPhysics::drawParticleSystems( true );
		clearTextures();
	}

	glPopAttrib();
}

void MPBOpenGL::drawShadowVolumes( bool objects, bool planes )
{
	if (m_staticPointLights.size())
	{
		if (objects)
		{
			MPBVector lightPosition = ((MPBPhysicsObject*)m_staticPointLights[0])->getPosition();
			resetCamera();
			MPBPhysics::drawShadowVolumes( lightPosition );
		}

		if (planes)
		{
			if (SHADOW_VOLUME_CELLS)
			{
				MPBCompositeShapeCell* cell = NULL;

				if (m_player)
				{
					cell = MPBGameEntities::getCell( m_player->getPosition() );
				}
				else
				{
					cell = MPBGameEntities::getCurrentCell();
				}

				if (cell)
				{
					glCallList( cell->getShadowVolumeDisplayList() );
				}
			}
			else
			{
				glCallList( m_displayListShadowVolume );
			}
		}
	}
	else if (m_pointLights.size())
	{
		if (objects)
		{
			MPBVector lightPosition = ((MPBPhysicsObject*)m_pointLights[0])->getPosition();
			resetCamera();
			MPBPhysics::drawShadowVolumes( lightPosition );
		}

		if (planes)
		{
			MPB_LIST_LOOP( MPBGameEntities::shapes, shape, MPBShape* )
			{
				shape->drawShadowVolume();
			}
		}
	}
	else
	{
		/*
		every 6 vertices define a volume:



													3

														1
												 5



					2

						0
				 4
		*/

		for (int i=0; i<m_shadowVolumeVertexes.size(); )
		{
			MPBVector* v0 = (MPBVector*)m_shadowVolumeVertexes[i++];
			MPBVector* v1 = (MPBVector*)m_shadowVolumeVertexes[i++];
			MPBVector* v2 = (MPBVector*)m_shadowVolumeVertexes[i++];
			MPBVector* v3 = (MPBVector*)m_shadowVolumeVertexes[i++];
			MPBVector* v4 = (MPBVector*)m_shadowVolumeVertexes[i++];
			MPBVector* v5 = (MPBVector*)m_shadowVolumeVertexes[i++];

			glBegin( GL_POLYGON );
			v0->drawAsVertex();
			v2->drawAsVertex();
			v4->drawAsVertex();
			glEnd();

			glBegin( GL_POLYGON );
			v1->drawAsVertex();
			v5->drawAsVertex();
			v3->drawAsVertex();
			glEnd();

			glBegin( GL_QUADS );

			v0->drawAsVertex();
			v1->drawAsVertex();
			v3->drawAsVertex();
			v2->drawAsVertex();

			v2->drawAsVertex();
			v3->drawAsVertex();
			v5->drawAsVertex();
			v4->drawAsVertex();

			v4->drawAsVertex();
			v5->drawAsVertex();
			v1->drawAsVertex();
			v0->drawAsVertex();

			glEnd();
		}
	}
}

void MPBOpenGL::createShadowVolume()
{
//	MPBVector lightPosition;

	if (m_pointLights.size())
	{
		MPB_LIST_LOOP( MPBGameEntities::shapes, shape, MPBShape* )
		{
			shape->createShadowVolume( NULL, getClosestPointLight( shape->getAABB().getCenter() )->getPosition() );
		}
	}
	else if (m_staticPointLights.size())
	{
		MPB_LIST_LOOP( MPBGameEntities::shapes, shape, MPBShape* )
		{
			if (SHADOW_VOLUME_CELLS)
			{
				MPBCompositeShapeCell* cell = shape->getCell();
				if (cell && cell->m_light)
					shape->createShadowVolume( NULL, cell->m_light->getPosition() );
			}
			else
			{
				shape->createShadowVolume( NULL, getStaticPointLight( 0 )->getPosition() );
			}
		}

		MPBPhysics::createShadowVolumes( getStaticPointLight( 0 )->getPosition() );
	}
	else return;


/*
	// use feedback
	if (m_staticPointLight)
	{
		m_shadowVolumeVertexes.clear();

		glDisable( GL_STENCIL_TEST );
		glEnable( GL_CULL_FACE );
		glCullFace( GL_BACK );
		glDepthMask( GL_TRUE );

		glDisable( GL_TEXTURE_1D );
		glDisable( GL_TEXTURE_2D );
		glDisable( GL_TEXTURE_3D );

		MPBVector lightPos = m_staticPointLight->getPosition();

		MPB_LIST_LOOP( MPBGameEntities::shapeVector, MPBShape* )
		{
			MPBShape* shape = *itr;

			MPBVector shapePos = shape->getAABB().getCenter();

			glClear( GL_COLOR_BUFFER_BIT | GL_STENCIL_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );

			glViewport( 0, 0, m_screenWidth, m_screenHeight );
			glMatrixMode( GL_PROJECTION );
			glLoadIdentity();
			MPBFLOAT width = m_screenWidth / 20.0;
			MPBFLOAT height = m_screenHeight / 20.0;
			glOrtho( -width, width, -height, height, m_perspectiveNear, m_perspectiveFar );

			glMatrixMode( GL_MODELVIEW );
			glLoadIdentity();
			gluLookAt(	lightPos.x, lightPos.y, lightPos.z,		// camera pos
							shapePos.x, shapePos.y, shapePos.z,		// look at
							0, 1, 0		// up vector
						);

			GLint viewport[4];
			GLfloat mvmatrix[16];
			GLfloat projmatrix[16];

			glGetIntegerv( GL_VIEWPORT, viewport );
			glGetfloatv( GL_MODELVIEW_MATRIX, mvmatrix );
			glGetfloatv( GL_PROJECTION_MATRIX, projmatrix );

			const int FB_BUFFER_SIZE = 1024 * 8;
			GLfloat feedbackBuffer[FB_BUFFER_SIZE];
			glFeedbackBuffer( FB_BUFFER_SIZE, GL_3D, feedbackBuffer );
			glRenderMode( GL_FEEDBACK );

			bool oldFC = shape->getFrustumCull();
			shape->setFrustumCull( false );
			shape->setDrawn( false );
			shape->draw( false, true );
			shape->setFrustumCull( oldFC );

			int size = glRenderMode( GL_RENDER );
			MPB_ASSERT( size >= 0 );	// feedback buffer is too small!

			GLfloat wx, wy, wz;
			GLfloat token;
			MPBFLOAT x;
			MPBFLOAT y;
			MPBFLOAT z;
			int i = 0;
			while (i < size) 
			{
				token = feedbackBuffer[i++];
				
				if (token == GL_POLYGON_TOKEN) 
				{
					int n = feedbackBuffer[i++];
					int j = 0;
					while (j < n)
					{
						x = feedbackBuffer[i++];
						y = feedbackBuffer[i++];
						z = feedbackBuffer[i++];

						gluUnProject( x, y, z, mvmatrix, projmatrix, viewport, &wx, &wy, &wz );

						MPBVector v0( wx, wy, wz );
						MPBVector lightVector = lightPos - v0;
						v0 = v0 - lightVector * .05;
						MPBVector v1( v0 - lightVector * 1000 );
						m_shadowVolumeVertexes.push_back( v0 );
						m_shadowVolumeVertexes.push_back( v1 );
						j++;
					}
				}
			}
		}}
	}
	else
	{
		// first render from light's POV in feedback mode

		m_renderFromLight = true;

		setupProjectionTransformation();
		resetCamera();
		extractFrustum();

		GLint viewport[4];
		GLfloat mvmatrix[16];
		GLfloat projmatrix[16];

		glGetIntegerv( GL_VIEWPORT, viewport );
		glGetfloatv( GL_MODELVIEW_MATRIX, mvmatrix );
		glGetfloatv( GL_PROJECTION_MATRIX, projmatrix );

		glClear( GL_COLOR_BUFFER_BIT | GL_STENCIL_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );
		glDepthMask( GL_TRUE );

		glDisable( GL_STENCIL_TEST );
		glEnable( GL_CULL_FACE );
		glCullFace( GL_BACK );

		glDisable( GL_TEXTURE_1D );
		glDisable( GL_TEXTURE_2D );
		glDisable( GL_TEXTURE_3D );

		const int FB_BUFFER_SIZE = 1024 * 8;
		GLfloat feedbackBuffer[FB_BUFFER_SIZE];
		glFeedbackBuffer( FB_BUFFER_SIZE, GL_3D, feedbackBuffer );
		glRenderMode( GL_FEEDBACK );

	//	MPBPhysics::draw( true );
	//	MPBGameEntities::draw( true );

		MPB_LIST_LOOP( MPBGameEntities::shapeVector, MPBShape* )
		{
			(*itr)->setDrawn( false );
			(*itr)->draw( false, true );
		}}

		// switch back to render mode

		int size = glRenderMode( GL_RENDER );
		MPB_ASSERT( size >= 0 );	// feedback buffer is too small!
		m_renderFromLight = false;

		m_shadowVolumeVertexes.clear();
		MPBVector lightVector = m_vectorToSun;
		GLfloat wx, wy, wz;
		static bool firstTime = true;
		if (firstTime) writeLog( "\nstart feedback\n" );
		GLfloat token;
		MPBFLOAT x;
		MPBFLOAT y;
		MPBFLOAT z;
		int i = 0;
		while (i < size) 
		{
			token = feedbackBuffer[i++];
			
			if (token == GL_PASS_THROUGH_TOKEN) 
			{
				if (firstTime) writeLog( "GL_PASS_THROUGH_TOKEN\n" );
				i++;
			}
			else if (token == GL_POINT_TOKEN) 
			{
				if (firstTime) writeLog( "GL_POINT_TOKEN\n" );
				x = feedbackBuffer[i++];
				y = feedbackBuffer[i++];
				z = feedbackBuffer[i++];
				if (firstTime) sprintf( g_logBuf, "   %f, %f, %f\n", x, y, z );
				if (firstTime) writeLog();
			}
			else if (token == GL_LINE_TOKEN) 
			{
				if (firstTime) writeLog( "GL_LINE_TOKEN\n" );
				x = feedbackBuffer[i++];
				y = feedbackBuffer[i++];
				z = feedbackBuffer[i++];
				if (firstTime) sprintf( g_logBuf, "   %f, %f, %f\n", x, y, z );
				if (firstTime) writeLog();
				x = feedbackBuffer[i++];
				y = feedbackBuffer[i++];
				z = feedbackBuffer[i++];
				if (firstTime) sprintf( g_logBuf, "   %f, %f, %f\n", x, y, z );
				if (firstTime) writeLog();
			}
			else if (token == GL_LINE_RESET_TOKEN) 
			{
				if (firstTime) writeLog( "GL_LINE_RESET_TOKEN\n" );
				x = feedbackBuffer[i++];
				y = feedbackBuffer[i++];
				z = feedbackBuffer[i++];
				if (firstTime) sprintf( g_logBuf, "   %f, %f, %f\n", x, y, z );
				if (firstTime) writeLog();
				x = feedbackBuffer[i++];
				y = feedbackBuffer[i++];
				z = feedbackBuffer[i++];
				if (firstTime) sprintf( g_logBuf, "   %f, %f, %f\n", x, y, z );
				if (firstTime) writeLog();
			}
			else if (token == GL_POLYGON_TOKEN) 
			{
				if (firstTime) writeLog( "GL_POLYGON_TOKEN\n" );
				int n = feedbackBuffer[i++];
				if (firstTime) sprintf( g_logBuf, "   %d\n", n );
				if (firstTime) writeLog();
				int j = 0;
				while (j < n)
				{
					x = feedbackBuffer[i++];
					y = feedbackBuffer[i++];
					z = feedbackBuffer[i++];

					gluUnProject( x, y, z, mvmatrix, projmatrix, viewport, &wx, &wy, &wz );

					MPBVector v0( wx, wy, wz );
					v0 = v0 - lightVector * .05;
					MPBVector v1( v0 - lightVector * 1000 );
					m_shadowVolumeVertexes.push_back( v0 );
					m_shadowVolumeVertexes.push_back( v1 );

					if (firstTime) sprintf( g_logBuf, "   %f, %f, %f\n", wx, wy, wz );
					if (firstTime) writeLog();
					j++;
				}
			}
		}

		firstTime = false;
	}
*/
}

void MPBOpenGL::drawCrosshairs()
{
	controlPanelBegin();
	controlPanelPosition( .5, .5 );

	bindTexture( "crosshairs" );

	int width = m_screenWidth / 10;
	int height = m_screenHeight / 10;

	glNormal3f( 0, 0, 1 );
	glBegin( GL_QUADS );
	glTexCoord2d( 0, 0 ); glVertex2d( -width/2,	-height/2 );
	glTexCoord2d( 1, 0 ); glVertex2d(  width/2,	-height/2 );
	glTexCoord2d( 1, 1 ); glVertex2d(  width/2,	 height/2 );
	glTexCoord2d( 0, 1 ); glVertex2d( -width/2,	 height/2 );
	glEnd();

	controlPanelEnd();
}

// call before drawing a console-type graphic
void MPBOpenGL::controlPanelBegin()
{
	glPushAttrib( GL_LIGHTING_BIT | GL_DEPTH_BUFFER_BIT | GL_FOG_BIT );
	glDisable( GL_LIGHTING );
	glDisable( GL_DEPTH_TEST );
	glDisable( GL_FOG );
//	glEnable( GL_DEPTH_TEST );

	glEnable( GL_BLEND );
	glBlendFunc( GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA );

	// get a 1:1 pixel mapping, with (0,0,0) in the center of the viewport
	glMatrixMode( GL_PROJECTION );
	glPushMatrix();
	glLoadIdentity();

	glMatrixMode( GL_MODELVIEW );
	glPushMatrix();
	glLoadIdentity();

	glScalef( 2.0f / (float)m_screenWidth, 2.0f / (float)m_screenHeight, 1.0f );

	glColor4f( 1, 1, 1, 1 );
}

// call after drawing a console-type graphic
void MPBOpenGL::controlPanelEnd()
{
	glMatrixMode( GL_PROJECTION );
	glPopMatrix();
	glMatrixMode( GL_MODELVIEW );
	glPopMatrix();

	glDisable( GL_BLEND );
	glPopAttrib();
}

// set position of a console-type graphic
// x = 0.0 to 1.0 (% of screen width), 0 = left, 1 = right
// y = 0.0 to 1.0 (% of screen height), 0 = top, 1 = bottom
void MPBOpenGL::controlPanelPosition( MPBFLOAT x, MPBFLOAT y )
{
	glTranslated( m_screenWidth * x - m_screenWidth / 2, m_screenHeight * y - m_screenHeight / 2, 0 );
}

void MPBOpenGL::createFonts()
{
	MPBFLOAT cx;											// Holds Our X Character Coord
	MPBFLOAT cy;											// Holds Our Y Character Coord

	m_fontListBase = glGenLists( 256 );
	MPBOpenGL::errorCheck();
//	bindTexture( m_fontTexture );
	for (int i=0; i<256; i++)
	{
		cx = float(i%16)/16.0f;						// X Position Of Current Character
		cy = float(i/16)/16.0f;						// Y Position Of Current Character

		const int width = 16;
		const int height = 16;

		glNewList( m_fontListBase + i, GL_COMPILE );
			if (glFogCoordd) glFogCoordd( 0 );
			glBegin( GL_QUADS );							// Use A Quad For Each Character
				glTexCoord2d( cx, 1 - cy - 0.0625f );			// Texture Coord (Bottom Left)
				glVertex2d( 0, 0 );						// Vertex Coord (Bottom Left)
				glTexCoord2d( cx + 0.0625f, 1 - cy - 0.0625f );	// Texture Coord (Bottom Right)
				glVertex2d( width, 0 );						// Vertex Coord (Bottom Right)
				glTexCoord2d( cx + 0.0625f, 1 - cy );			// Texture Coord (Top Right)
				glVertex2d( width, height );						// Vertex Coord (Top Right)
				glTexCoord2d( cx, 1 - cy );					// Texture Coord (Top Left)
				glVertex2d( 0, height );						// Vertex Coord (Top Left)
			glEnd();									// Done Building Our Quad (Character)
			glTranslated( width, 0, 0 );						// Move To The Right Of The Character
		glEndList();
	}

	MPBOpenGL::errorCheck();
}

// set = 0 or 1
void MPBOpenGL::printString( const char* string, int set )
{
	bindTexture( m_fontTexture );
	glListBase( m_fontListBase - 32 + (128 * set) );
	glCallLists( strlen( string ), GL_BYTE, string );

//	MPBOpenGL::errorCheck();
}


//////////////////////////////////////////////////////
// texture functions

void MPBOpenGL::createTextureFromFile( GLenum texId, const char* dir, const char* name, BYTE alpha, bool mipmap )
{
	glActiveTexture( texId );
	MPBOpenGL::errorCheck();

//	if (MPBOpenGL::is2D()) mipmap = false;
	if (!strcmp( name, "health" )) mipmap = false;
	if (!strcmp( name, "healthSign" )) mipmap = false;
	if (!strcmp( name, "breath" )) mipmap = false;
	if (!strcmp( name, "breathSign" )) mipmap = false;
	if (!strcmp( name, "crosshairs" )) mipmap = false;
	if (!strcmp( name, "font2" )) mipmap = false;

	MPBTexture* t = new MPBTexture( name, alpha, mipmap, GL_REPEAT );
	int i = texId - GL_TEXTURE0;
	m_textures[i].push_back( t );

	char buf[1000];
	sprintf( buf, "%s/%s.tex", dir, name );
	t->loadFromFile( buf );
}

// if alphas is NULL, all alphas will be set to 255, or 0 if red, green and blue are TRANSPARENT_COLOR
void MPBOpenGL::createTextureFromMemory(	const char* name,
														BYTE* reds,
														BYTE* greens,
														BYTE* blues,
														BYTE* alphas,
														int width,
														int height,
														int depth,
														GLint wrapType,
														bool mipmap	)
{
	GLenum texId = GL_TEXTURE0;
	for (int i=0; i<MAX_TEXTURES; i++)
	{
		glActiveTexture( texId++ );
		MPBOpenGL::errorCheck();

		MPBTexture* t = new MPBTexture( name, 255, mipmap, wrapType );
		m_textures[i].push_back( t );
		t->loadFromMemory( reds, greens, blues, alphas, width, height, depth );
	}
}

MPBTexture* MPBOpenGL::findTexture( const char* fileName, GLenum texId )
{
	if (!strcmp( fileName, "" )) return NULL;

	MPB_LIST_LOOP( m_textures[texId - GL_TEXTURE0], t, MPBTexture* )
	{
		if (strcmp( t->getName(), fileName ) == 0)
		{
			return t;
		}
	}

	MPB_ASSERT( 0 );
	return NULL;
}

// don't call between glBegin() and glEnd()
void MPBOpenGL::bindParticleTexture()
{
	bindTexture( PARTICLE_TEXTURE, GL_TEXTURE0 );
}

// don't call between glBegin() and glEnd()
void MPBOpenGL::bindTexture( const char* fileName )
{
	bindTexture( fileName, GL_TEXTURE0 );
}

// texId = GL_TEXTUREn.
// call unbindTexture() with the same texId(s), below, after using this function if using multitextures.
void MPBOpenGL::bindTexture( const char* fileName, GLenum texId )
{
//	sprintf( g_logBuf, "binding texture %s, \n", fileName );
//	writeLog();

	if (!strcmp( fileName, "" )) return;
	if (NO_TEXTURES) return;

	if (!strcmp( fileName, "generatedShadowTexture" ))
	{
		glActiveTexture( texId );
		glEnable( GL_TEXTURE_2D );
		glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_R, GL_CLAMP );
		glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP );
		glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP );
		glBindTexture( GL_TEXTURE_2D, m_shadowMapTextureID );

		return;
	}

	if (	strcmp( fileName, "generatedLightmap" ) &&
			strcmp( fileName, "generatedShadowTexture" )	)
		if (SINGLE_TEXTURE) fileName = SINGLE_TEXTURE;

	if (!strcmp( m_lastTextureFile[texId - GL_TEXTURE0], fileName )) 
	{
		return;
	}
	strcpy( m_lastTextureFile[texId - GL_TEXTURE0], fileName );


//	writeLog( "binding\n" );
	MPBTexture* t = findTexture( fileName, texId );
	if (t)
	{
		glActiveTexture( texId );
		glEnable( t->getTexType() );
		t->bind();

	//	MPBOpenGL::errorCheck();
	}
}

void MPBOpenGL::clearTextures()
{
	for (int i=0; i<MAX_TEXTURES; i++)
	{
		GLenum texId = GL_TEXTURE0 + i;
		strcpy( m_lastTextureFile[texId - GL_TEXTURE0], "" );

/*		glActiveTexture( texId );

		glEnable( GL_TEXTURE_1D );
		glTexParameteri( GL_TEXTURE_1D, GL_TEXTURE_WRAP_R, GL_REPEAT );
		glTexParameteri( GL_TEXTURE_1D, GL_TEXTURE_WRAP_S, GL_REPEAT );
		glTexParameteri( GL_TEXTURE_1D, GL_TEXTURE_WRAP_T, GL_REPEAT );
		glDisable( GL_TEXTURE_1D );

		glEnable( GL_TEXTURE_2D );
		glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_R, GL_REPEAT );
		glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT );
		glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT );
		glDisable( GL_TEXTURE_2D );

		glEnable( GL_TEXTURE_3D );
		glTexParameteri( GL_TEXTURE_3D, GL_TEXTURE_WRAP_R, GL_REPEAT );
		glTexParameteri( GL_TEXTURE_3D, GL_TEXTURE_WRAP_S, GL_REPEAT );
		glTexParameteri( GL_TEXTURE_3D, GL_TEXTURE_WRAP_T, GL_REPEAT );
		glDisable( GL_TEXTURE_3D );
*/	}
}

void MPBOpenGL::unbindTexture( const char* fileName, GLenum texId )
{
	if (!strcmp( fileName, "" )) return;

	if (strcmp( fileName, "generatedLightmap" ))
		if (SINGLE_TEXTURE) fileName = SINGLE_TEXTURE;

	MPBTexture* t = findTexture( fileName, texId );
	if (t)
	{
		glActiveTexture( texId );
		glDisable( t->getTexType() );
		strcpy( m_lastTextureFile[texId - GL_TEXTURE0], "" );

	//	MPBOpenGL::errorCheck();
	}
}


//////////////////////////////////////////////////////
// frustum functions

void MPBOpenGL::extractFrustum()
{
	if (!ENABLE_FRUSTUM_CULLING) return;

	GLfloat proj[16];
	GLfloat modl[16];
	GLfloat clip[16];
	MPBFLOAT t;
	MPBFLOAT a, b, c, d;
	bool normalize = true;
	bool log = false;

	MPBVector normal;

	/* Get the current PROJECTION matrix from OpenGL */
	glGetFloatv( GL_PROJECTION_MATRIX, proj );

	/* Get the current MODELVIEW matrix from OpenGL */
	glGetFloatv( GL_MODELVIEW_MATRIX, modl );

	/* Combine the two matrices (multiply projection by modelview) */
	clip[ 0] = modl[ 0] * proj[ 0] + modl[ 1] * proj[ 4] + modl[ 2] * proj[ 8] + modl[ 3] * proj[12];
	clip[ 1] = modl[ 0] * proj[ 1] + modl[ 1] * proj[ 5] + modl[ 2] * proj[ 9] + modl[ 3] * proj[13];
	clip[ 2] = modl[ 0] * proj[ 2] + modl[ 1] * proj[ 6] + modl[ 2] * proj[10] + modl[ 3] * proj[14];
	clip[ 3] = modl[ 0] * proj[ 3] + modl[ 1] * proj[ 7] + modl[ 2] * proj[11] + modl[ 3] * proj[15];

	clip[ 4] = modl[ 4] * proj[ 0] + modl[ 5] * proj[ 4] + modl[ 6] * proj[ 8] + modl[ 7] * proj[12];
	clip[ 5] = modl[ 4] * proj[ 1] + modl[ 5] * proj[ 5] + modl[ 6] * proj[ 9] + modl[ 7] * proj[13];
	clip[ 6] = modl[ 4] * proj[ 2] + modl[ 5] * proj[ 6] + modl[ 6] * proj[10] + modl[ 7] * proj[14];
	clip[ 7] = modl[ 4] * proj[ 3] + modl[ 5] * proj[ 7] + modl[ 6] * proj[11] + modl[ 7] * proj[15];

	clip[ 8] = modl[ 8] * proj[ 0] + modl[ 9] * proj[ 4] + modl[10] * proj[ 8] + modl[11] * proj[12];
	clip[ 9] = modl[ 8] * proj[ 1] + modl[ 9] * proj[ 5] + modl[10] * proj[ 9] + modl[11] * proj[13];
	clip[10] = modl[ 8] * proj[ 2] + modl[ 9] * proj[ 6] + modl[10] * proj[10] + modl[11] * proj[14];
	clip[11] = modl[ 8] * proj[ 3] + modl[ 9] * proj[ 7] + modl[10] * proj[11] + modl[11] * proj[15];

	clip[12] = modl[12] * proj[ 0] + modl[13] * proj[ 4] + modl[14] * proj[ 8] + modl[15] * proj[12];
	clip[13] = modl[12] * proj[ 1] + modl[13] * proj[ 5] + modl[14] * proj[ 9] + modl[15] * proj[13];
	clip[14] = modl[12] * proj[ 2] + modl[13] * proj[ 6] + modl[14] * proj[10] + modl[15] * proj[14];
	clip[15] = modl[12] * proj[ 3] + modl[13] * proj[ 7] + modl[14] * proj[11] + modl[15] * proj[15];

	/* Extract the numbers for the RIGHT plane */
	a = clip[ 3] - clip[ 0];
	b = clip[ 7] - clip[ 4];
	c = clip[11] - clip[ 8];
	d = clip[15] - clip[12];

	if (normalize) { t = (float)sqrt( a * a + b * b + c * c ); if (t == 0) return; a /= t; b /= t; c /= t; d /= t; }

	normal.set( a, b, c );
	m_viewFrustumRight.setNormal( normal );
	m_viewFrustumRight.setD( d );

	if (log) { m_viewFrustumRight.toString( g_logBuf ); writeLog( g_logBuf ); writeLog( "\n" ); }

	/* Extract the numbers for the LEFT plane */
	a = clip[ 3] + clip[ 0];
	b = clip[ 7] + clip[ 4];
	c = clip[11] + clip[ 8];
	d = clip[15] + clip[12];

	if (normalize) { t = (float)sqrt( a * a + b * b + c * c ); if (t == 0) return; a /= t; b /= t; c /= t; d /= t; }

	normal.set( a, b, c );
	m_viewFrustumLeft.setNormal( normal );
	m_viewFrustumLeft.setD( d );

	if (log) { m_viewFrustumLeft.toString( g_logBuf ); writeLog( g_logBuf ); writeLog( "\n" ); }

	/* Extract the numbers for the BOTTOM plane */
	a = clip[ 3] + clip[ 1];
	b = clip[ 7] + clip[ 5];
	c = clip[11] + clip[ 9];
	d = clip[15] + clip[13];

	if (normalize) { t = (float)sqrt( a * a + b * b + c * c ); if (t == 0) return; a /= t; b /= t; c /= t; d /= t; }

	normal.set( a, b, c );
	m_viewFrustumBottom.setNormal( normal );
	m_viewFrustumBottom.setD( d );

	if (log) { m_viewFrustumBottom.toString( g_logBuf ); writeLog( g_logBuf ); writeLog( "\n" ); }

	/* Extract the numbers for the TOP plane */
	a = clip[ 3] - clip[ 1];
	b = clip[ 7] - clip[ 5];
	c = clip[11] - clip[ 9];
	d = clip[15] - clip[13];

	if (normalize) { t = (float)sqrt( a * a + b * b + c * c ); if (t == 0) return; a /= t; b /= t; c /= t; d /= t; }

	normal.set( a, b, c );
	m_viewFrustumTop.setNormal( normal );
	m_viewFrustumTop.setD( d );

	if (log) { m_viewFrustumTop.toString( g_logBuf ); writeLog( g_logBuf ); writeLog( "\n" ); }

	/* Extract the numbers for the FAR plane */
	a = clip[ 3] - clip[ 2];
	b = clip[ 7] - clip[ 6];
	c = clip[11] - clip[10];
	d = clip[15] - clip[14];

	if (normalize) { t = (float)sqrt( a * a + b * b + c * c ); if (t == 0) return; a /= t; b /= t; c /= t; d /= t; }

	normal.set( a, b, c );
	m_viewFrustumFar.setNormal( normal );
	m_viewFrustumFar.setD( d );

	if (log) { m_viewFrustumFar.toString( g_logBuf ); writeLog( g_logBuf ); writeLog( "\n" ); }

	/* Extract the numbers for the NEAR plane */
	a = clip[ 3] + clip[ 2];
	b = clip[ 7] + clip[ 6];
	c = clip[11] + clip[10];
	d = clip[15] + clip[14];

	if (normalize) { t = (float)sqrt( a * a + b * b + c * c ); if (t == 0) return; a /= t; b /= t; c /= t; d /= t; }

	normal.set( a, b, c );
	m_viewFrustumNear.setNormal( normal );
	m_viewFrustumNear.setD( d );

	if (log) { m_viewFrustumNear.toString( g_logBuf ); writeLog( g_logBuf ); writeLog( "\n" ); }
}

bool MPBOpenGL::pointInFrustum( MPBFLOAT x, MPBFLOAT y, MPBFLOAT z )
{
return true;
	MPBVector point( x, y, z );
	return pointInFrustum( point );
}

bool MPBOpenGL::pointInFrustum( const MPBVector& point )
{
	if (!ENABLE_FRUSTUM_CULLING) return true;

	// if point is inside any frustum plane, return false

	if (m_viewFrustumRight.isPointInside( point )) return false;
	if (m_viewFrustumLeft.isPointInside( point )) return false;
	if (m_viewFrustumTop.isPointInside( point )) return false;
	if (m_viewFrustumBottom.isPointInside( point )) return false;
	if (m_viewFrustumNear.isPointInside( point )) return false;
	if (m_viewFrustumFar.isPointInside( point )) return false;

	return true;
}

// returns false only if all points are inside any frustum plane
bool MPBOpenGL::pointsInFrustum( MPBVector* pointList, int points )
{
	if (!ENABLE_FRUSTUM_CULLING) return true;

	if (m_viewFrustumRight.arePointsInside( pointList, points )) return false;
	if (m_viewFrustumLeft.arePointsInside( pointList, points )) return false;
	if (m_viewFrustumBottom.arePointsInside( pointList, points )) return false;
	if (m_viewFrustumTop.arePointsInside( pointList, points )) return false;
	if (m_viewFrustumNear.arePointsInside( pointList, points )) return false;
	if (m_viewFrustumFar.arePointsInside( pointList, points )) return false;

	return true;
}

// ignores the view frustum's bottom and top planes
bool MPBOpenGL::pointsInOpenFrustum( MPBVector* pointList, int points )
{
	if (!ENABLE_FRUSTUM_CULLING) return true;

	if (m_viewFrustumRight.arePointsInside( pointList, points )) return false;
	if (m_viewFrustumLeft.arePointsInside( pointList, points )) return false;
	if (m_viewFrustumNear.arePointsInside( pointList, points )) return false;
	if (m_viewFrustumFar.arePointsInside( pointList, points )) return false;

	return true;
}

bool MPBOpenGL::sphereInFrustum( const MPBVector& center, MPBFLOAT radius )
{
	if (!ENABLE_FRUSTUM_CULLING) return true;

	// if sphere is inside any frustum plane, return false

	if (m_viewFrustumRight.isSphereInside( center, radius )) return false;
	if (m_viewFrustumLeft.isSphereInside( center, radius )) return false;
	if (m_viewFrustumTop.isSphereInside( center, radius )) return false;
	if (m_viewFrustumBottom.isSphereInside( center, radius )) return false;
	if (m_viewFrustumNear.isSphereInside( center, radius )) return false;
	if (m_viewFrustumFar.isSphereInside( center, radius )) return false;

	return MPBGameEntities::sphereInPortalFrustum( center, radius );
}

// returns false only if all corners are inside any frustum plane
bool MPBOpenGL::planeInFrustum( const Plane& plane )
{
	if (!ENABLE_FRUSTUM_CULLING) return true;

	MPBVector vertices[4];
	vertices[0] = plane.getLL();
	vertices[1] = plane.getLR();
	vertices[2] = plane.getUR();
	vertices[3] = plane.getUL();
	return pointsInFrustum( vertices, 4 );
}

// combines view frustum and portal frustum tests
bool MPBOpenGL::aabbInFrustum( const MPBAABB& aabb )
{
	if (!ENABLE_FRUSTUM_CULLING) return true;

	MPBVector vertices[8];
	vertices[0] = aabb.getNLL();
	vertices[1] = aabb.getNLR();
	vertices[2] = aabb.getNUR();
	vertices[3] = aabb.getNUL();
	vertices[4] = aabb.getFLL();
	vertices[5] = aabb.getFLR();
	vertices[6] = aabb.getFUR();
	vertices[7] = aabb.getFUL();
	return (
		pointsInFrustum( vertices, 8 ) && 
		aabbInPointLight( aabb ) &&
		MPBGameEntities::aabbInPortalFrustum( aabb )
	);
}

//////////////////////////////////////////////////////

bool MPBOpenGL::aabbInPointLight( const MPBAABB& aabb )
{
	bool rc = true;

/*	if (m_pointLight)
	{
		rc = false;
		if (aabb.intersectsSphere( m_pointLight->getPosition(), m_pointLight->getRadius() ))
			rc = true;
	}
*/
	return rc;
}

MPBPhysicsObject* MPBOpenGL::getPointLight( int i ) 
{ 
	if (m_pointLights.size() > i) return (MPBPhysicsObject*)m_pointLights[i]; 
	return NULL;
}

MPBPhysicsObject* MPBOpenGL::getStaticPointLight( int i ) 
{ 
	if (m_staticPointLights.size() > i) return (MPBPhysicsObject*)m_staticPointLights[i]; 
	return NULL;
}

MPBPhysicsObject* MPBOpenGL::getClosestPointLight( MPBVector point )
{
	MPBFLOAT maxDistance = 1000000;
	MPBPhysicsObject* closestLight = NULL;
	MPB_LIST_LOOP( m_pointLights, light, MPBPhysicsObject* )
	{
		MPBVector v = point - light->getPosition();
		MPBFLOAT distance = v.getLength();
		if (distance < maxDistance)
		{
			maxDistance = distance;
			closestLight = light;
		}
	}

	return closestLight;
}

MPBPhysicsObject* MPBOpenGL::getClosestStaticPointLight( MPBVector point )
{
	MPBFLOAT maxDistance = 1000000;
	MPBPhysicsObject* closestLight = NULL;
	MPB_LIST_LOOP( m_staticPointLights, light, MPBPhysicsObject* )
	{
		MPBVector v = point - light->getPosition();
		MPBFLOAT distance = v.getLength();
		if (distance < maxDistance)
		{
			maxDistance = distance;
			closestLight = light;
		}
	}

	return closestLight;
}

GLuint MPBOpenGL::createShadowDisplayList()
{
	m_renderFromLight = true;
	drawScene( NULL );

	MPB_ASSERT( DEPTH_BUFFER_BITS == 16 );
	int size = m_screenWidth * m_screenHeight;

	MPBFLOAT* depths = new MPBFLOAT[ size ];
	glReadPixels(	0,
						0,
						m_screenWidth,
						m_screenHeight,
						GL_DEPTH_COMPONENT,
						GL_FLOAT,
						depths	);

	GLuint displayList = glGenLists( 1 );
	glNewList( displayList, GL_COMPILE );

	GLint viewport[4];
	GLdouble mvmatrix[16], projmatrix[16];
	GLint realy;  /*  OpenGL y coordinate position  */
	GLdouble wx, wy, wz;  /*  returned world x, y, z coords  */
	glGetIntegerv (GL_VIEWPORT, viewport);
	glGetDoublev (GL_MODELVIEW_MATRIX, mvmatrix);
	glGetDoublev (GL_PROJECTION_MATRIX, projmatrix);

	for (int y=0; y<m_screenHeight - 1; y++)
	{
		glBegin( GL_TRIANGLE_STRIP );
		for (int x=0; x<m_screenWidth; x++)
		{
			MPBFLOAT depth;

			depth = depths[(y + 1) * m_screenWidth + x];
//			depth /= 0xFFFF;
			/*  note viewport[3] is height of window in pixels  */
			realy = y + 1;//viewport[3] - (GLint) y - 1;
			//printf ("Coordinates at cursor are (%4d, %4d)\n", x, realy);
			gluUnProject( x, realy, depth, mvmatrix, projmatrix, viewport, &wx, &wy, &wz );
			//printf ("World coords at z=0.0 are (%f, %f, %f)\n", wx, wy, wz);
//			gluUnProject( x, realy, 1.0, mvmatrix, projmatrix, viewport, &wx, &wy, &wz );
			//printf ("World coords at z=1.0 are (%f, %f, %f)\n", wx, wy, wz);
			glVertex3d( wx, wy, wz );

			depth = depths[(y) * m_screenWidth + x];
//			depth /= 0xFFFF;
			realy = y;//viewport[3] - (GLint) y - 1;
			gluUnProject( x, realy, depth, mvmatrix, projmatrix, viewport, &wx, &wy, &wz );
//			gluUnProject( x, realy, 1.0, mvmatrix, projmatrix, viewport, &wx, &wy, &wz );

			glVertex3d( wx, wy, wz );
		}
		glEnd();
	}

/*	for (int z=0; z<m_screenHeight - 1; z++)
	{
		glBegin( GL_TRIANGLE_STRIP );
		for (int x=0; x<m_screenWidth; x++)
		{
			unsigned short depth;
			MPBFLOAT xpos;
			MPBFLOAT ypos;
			MPBFLOAT zpos;

			depth = (z + 1) * m_screenWidth + x;
//			MPBVector lightToPixel = 
//			lightPos
			xpos = -(x - m_screenWidth/2.0);
			zpos = depths[depth];
			ypos = z + 1 - m_screenHeight/2.0;
			glVertex3d( xpos, ypos, zpos );

			depth = (z) * m_screenWidth + x;
			xpos = -(x - m_screenWidth/2.0);
			zpos = depths[depth];
			ypos = z - m_screenHeight/2.0;
			glVertex3d( xpos, ypos, zpos );
		}
		glEnd();
	}
*/
	glEndList();

	delete[] depths;

	m_renderFromLight = false;

//	takeDepthBufferShot( "shadowMap.raw" );

	return displayList;
}

// to read .raw file in PhotoShop: screen width x screen height, 3 channels, interleaved, 8 bits, no header
// then flip vertical
// see log file for screen size
void MPBOpenGL::takeScreenShot( const char* filename )
{
	int size = 3 * m_screenWidth * m_screenHeight;
	BYTE* pixels = new BYTE[ size ];
	glReadPixels(	0,
						0,
						m_screenWidth,
						m_screenHeight,
						GL_RGB,
						GL_UNSIGNED_BYTE,
						pixels	);

	writeFile( filename, pixels, size );
	delete[] pixels;
}

// to read .raw file in PhotoShop: screen width x screen height, 1 channel, 16 bits IBM PC, no header,
// then flip vertical
// see log file for screen size
void MPBOpenGL::takeDepthBufferShot( const char* filename )
{
	MPB_ASSERT( DEPTH_BUFFER_BITS == 16 );
	int size = m_screenWidth * m_screenHeight;
	unsigned short* pixels = new unsigned short[ size ];
	glReadPixels(	0,
						0,
						m_screenWidth,
						m_screenHeight,
						GL_DEPTH_COMPONENT,
						GL_UNSIGNED_SHORT,
						pixels	);

	FILE* fp = fopen( filename, "wb" );
	fwrite( pixels, sizeof (unsigned short), size, fp );
	fclose( fp );

	delete[] pixels;
}

// to read .raw file in PhotoShop: screen width x screen height, 1 channel, 8 bits, no header,
// then flip vertical
// see log file for screen size
void MPBOpenGL::takeStencilBufferShot( const char* filename )
{
	int size = m_screenWidth * m_screenHeight;
	BYTE* pixels = new BYTE[ size ];
	glReadPixels(	0,
						0,
						m_screenWidth,
						m_screenHeight,
						GL_STENCIL_INDEX,
						GL_UNSIGNED_BYTE,
						pixels	);

	writeFile( filename, pixels, size );
	delete[] pixels;
}


void MPBOpenGL::makeShadowMaps()
{
	glClearColor( 1, 1, 1, 1 );
	glClear( GL_COLOR_BUFFER_BIT | GL_STENCIL_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );
	setupProjectionTransformation();

	const bool MAKE_DEPTH_MAP = false;

	glDepthMask( GL_TRUE );
	glEnable( GL_DEPTH_TEST );
	if (MAKE_DEPTH_MAP) glColorMask( GL_FALSE, GL_FALSE, GL_FALSE, GL_FALSE );
	glColor4f( .4f, .4f, .4f, 1 );
	glDisable( GL_TEXTURE_1D );
	glDisable( GL_TEXTURE_2D );
	glDisable( GL_TEXTURE_3D );
	glDisable( GL_FOG );
	glDisable( GL_STENCIL_TEST );
	glEnable( GL_CULL_FACE );
	glCullFace( GL_BACK );


	int i = 0;
	MPB_LIST_LOOP( MPBPhysics::objects, o, MPBPhysicsObject* )
	{
		if (!o->getCastsShadow()) continue;

		//////////////////////////////////////
		// set up to render from light's POV

		MPBVector lookAt;// = o->getPosition();
		MPBPhysicsObject* light = getClosestStaticPointLight( o->getPosition() );
		MPBVector camera = light->getPosition();
		MPBVector up( 0, 1, 0 );

		MPBFLOAT frustumWidth = o->getRadius() * 2;
		MPBFLOAT frustumHeight = o->getRadius() * 2;

		int mapWidth = 512;
		int mapHeight = 512;

		glViewport( 0, 0, mapWidth, mapHeight );

		glMatrixMode( GL_PROJECTION );
		glLoadIdentity();

/*		glOrtho(	-frustumWidth/2,		// left
					 frustumWidth/2,		// right
					-frustumHeight/2,	// bottom
					 frustumHeight/2,		// top
					.1,					// near
					200	);				// far
*/
		gluPerspective(
			90,					// FOV angle
			(GLfloat)mapWidth / (GLfloat)mapHeight,	// aspect ratio
			m_perspectiveNear,					// near
			m_perspectiveFar	);					// far

		glMatrixMode( GL_MODELVIEW );
		glLoadIdentity();
		gluLookAt(	camera.x, camera.y, camera.z,
						lookAt.x, lookAt.y, lookAt.z,
						up.x, up.y, up.z	);

		o->draw( false, NULL, MPBVector(), true );

		//////////////////////////////////////

/*		GLint viewport[4];
		GLfloat mvmatrix[16], projmatrix[16];
		GLfloat wx, wy, wz;
		glGetIntegerv( GL_VIEWPORT, viewport );
		glGetfloatv( GL_MODELVIEW_MATRIX, mvmatrix );
		glGetfloatv( GL_PROJECTION_MATRIX, projmatrix );

		gluUnProject( x, y + 1, depth, mvmatrix, projmatrix, viewport, &wx, &wy, &wz );
		glVertex3d( wx, wy, wz );
*/

		//////////////////////////////////////
		// store the image in a texture

		char filename[1000];
		sprintf( filename, "lightmaps/lightmap_%d", i );

		if (MAKE_DEPTH_MAP)
		{
			int size = mapWidth * mapHeight;
			unsigned short* pixels = new unsigned short[ size ];
			glReadPixels(	0,
								0,
								mapWidth,
								mapHeight,
								GL_DEPTH_COMPONENT,
								GL_UNSIGNED_SHORT,
								pixels	);

			strcat( filename, ".raw" );
			FILE* fp = fopen( filename, "wb" );
			fwrite( pixels, sizeof (unsigned short), size, fp );
			fclose( fp );

			delete[] pixels;
		}
		else
		{
			glActiveTexture( GL_TEXTURE1 );
			glEnable( GL_TEXTURE_2D );
			glBindTexture( GL_TEXTURE_2D, m_shadowMapTextureID );
			glCopyTexImage2D( GL_TEXTURE_2D, 0, GL_RGBA, 0, 0, mapWidth, mapHeight, 0 );
//			glDisable( GL_TEXTURE_2D );
		}

		i++;
	}
}

void MPBOpenGL::reflectAboutPlane( Plane* p )
{
	MPBVector T = p->getCenter();
	MPBVector n = p->getNormal();

	MPBQuaternion q;
	q.fromVector( n );
	MPBFLOAT N[16];
	q.toMatrix( N );

	MPBVector n_inv = n * -1;
	MPBQuaternion q_inv;
	q.fromVector( n_inv );
	MPBFLOAT N_inv[16];
	q.toMatrix( N_inv );

	glTranslated( T.x, T.y, T.z );		// T
	glMultMatrixd( N );						// N
	glScalef( 1, 1, -1 );					// M
	glMultMatrixd( N_inv );					// N'
	glTranslated( -T.x, -T.y, -T.z );	// T'
}


