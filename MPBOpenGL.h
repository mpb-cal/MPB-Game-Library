#ifndef __MPBOPENGL_H__
#define __MPBOPENGL_H__

#include "MPBLibrary.h"
#include "MPBTexturedPlane.h"
#include "MPBList.h"
#include "MPBVectorList.h"

class MPBTexture;
class MPBPhysicsObject;
class MPBShape;


const bool USE_LIGHT_VOLUMES = false;


class MPBOpenGL
{
public:

	static void errorCheck();
	static void getGLSettings();
	static HWND createGLWindow(	WNDPROC wndProc,
											HINSTANCE hInstance,
											char* wndName,
											bool fullScreen,
											bool _2D,
											int width,
											int height,
											char* textureDir,
											HMENU hMenu	);
	static void changeWindowSize( int width, int height );
	static void switchFullScreen();
	static void startGameLoop();
	static void cleanUp();
	static void drawScene( void(*customDraw)() );
	static void drawSceneTest();
	static void controlPanelBegin();
	static void controlPanelEnd();
	static void controlPanelPosition( MPBFLOAT x, MPBFLOAT y );
	static void drawCrosshairs();
	static void resetCamera();
	static bool is2D() { return m_is2D; }
	static void bindParticleTexture();
	static void bindTexture( const char* fileName );
	static void bindTexture( const char* fileName, GLenum texId );
	static void unbindTexture( const char* fileName, GLenum texId );
	static void clearTextures();
	static void printString( const char* string, int set );
	static void setCameraPos( MPBVector v ) { m_cameraPos = v; }
	static void setLookAt( MPBVector l ) { m_lookAt = l; }
	static void addPointLight( MPBPhysicsObject* l ) { m_pointLights.push_back( l ); }
	static void addStaticPointLight( MPBPhysicsObject* l ) { m_staticPointLights.push_back( l ); }
	static MPBPhysicsObject* getPointLight( int i );
	static MPBPhysicsObject* getStaticPointLight( int i );
	static MPBPhysicsObject* getClosestPointLight( MPBVector point );
	static MPBPhysicsObject* getClosestStaticPointLight( MPBVector point );
	static MPBVector getCameraPos() { return m_cameraPos; }
	static MPBVector getLookAt() { return m_lookAt; }
	static void setupProjectionTransformation();
	static void setPerspectiveAngle( GLfloat a ) { m_perspectiveAngle = a; }
	static GLfloat getPerspectiveAngle() { return m_perspectiveAngle; }
	static void createTextureFromFile( GLenum texId, const char* dir, const char* name, BYTE alpha, bool mipmap );
	static void createTextureFromMemory(	const char* name,
														BYTE* reds,
														BYTE* greens,
														BYTE* blues,
														BYTE* alphas,
														int width,
														int height,
														int depth,
														GLint wrapType,
														bool mipmap	);
	static int getScreenWidth() { return m_screenWidth; }
	static int getScreenHeight() { return m_screenHeight; }
	static bool pointInFrustum( MPBFLOAT x, MPBFLOAT y, MPBFLOAT z );
	static bool pointInFrustum( const MPBVector& point );
	static bool pointsInFrustum( MPBVector* pointList, int points );
	static bool pointsInOpenFrustum( MPBVector* pointList, int points );
	static bool aabbInFrustum( const MPBAABB& aabb );
	static bool aabbInPointLight( const MPBAABB& aabb );
	static bool sphereInFrustum( const MPBVector& center, MPBFLOAT radius );
	static bool planeInFrustum( const Plane& plane );
	static void takeScreenShot( const char* filename );
	static void takeDepthBufferShot( const char* filename );
	static void takeStencilBufferShot( const char* filename );
	static GLfloat getPerspectiveFar() { return m_perspectiveFar; }
	static GLuint createShadowDisplayList();
	static void enableFog( MPBFLOAT red, MPBFLOAT green, MPBFLOAT blue, MPBFLOAT density );
	static void disableFog();
	static void disableShadows() { m_noShadows = true; }
	static void enableShadows() { m_noShadows = false; }
	static void drawShadowVolumes( bool objects, bool planes );
	static void createShadowVolume();
	static void drawStencilShadows( bool objects, bool planes );
	static void setFogCoord( MPBVector point );
	static void disableCrosshairs() { m_drawCrosshairs = false; }
	static void enableCrosshairs() { m_drawCrosshairs = true; }
	static void makeShadowMaps();
	static void setPlayer( MPBPhysicsObject* p ) { m_player = p; }
	static MPBPhysicsObject* getPlayer() { return m_player; }
	static void reflectAboutPlane( Plane* p );
	static void drawPoint( MPBVector p );


	static GLUquadricObj*					quadricObj;
	static GLUtesselator*					tesselator;
	static PFNGLACTIVETEXTUREARBPROC		glActiveTexture;
	static PFNGLMULTITEXCOORD3FARBPROC	glMultiTexCoord3f;
	static PFNGLTEXIMAGE3DPROC				glTexImage3D;
	static PFNGLFOGCOORDDEXTPROC			glFogCoordd;
	static MPBVector							clearColor;
	static MPBVector							planeSortPosition;

private:

	static HDC									m_hdc;
	static int									m_screenWidth;
	static int									m_screenHeight;
	static GLfloat 							m_perspectiveAngle;
	static GLfloat 							m_perspectiveFar;
	static GLfloat 							m_perspectiveNear;
	static bool									m_is2D;
	static GLuint								m_fontListBase;
	static char									m_fontTexture[MAX_PATH];
	static char									m_textureDir[MAX_PATH];
	static MPBList								m_textures[MAX_TEXTURES];
	static MPBVector							m_lookAt;
	static MPBVector							m_cameraPos;
	static MPBList								m_pointLights;
	static MPBList								m_staticPointLights;
	static Plane								m_viewFrustumRight;
	static Plane								m_viewFrustumLeft;
	static Plane								m_viewFrustumTop;
	static Plane								m_viewFrustumBottom;
	static Plane								m_viewFrustumNear;
	static Plane								m_viewFrustumFar;
	static GLuint								m_displayListShadowShape;
	static bool									m_renderFromLight;
	static MPBFLOAT 							m_lightModelViewMatrix[16];
	static MPBFLOAT 							m_lightProjectionMatrix[16];
	static MPBFLOAT 							m_fogRed;
	static MPBFLOAT 							m_fogGreen;
	static MPBFLOAT 							m_fogBlue;
	static MPBFLOAT 							m_fogDensity;
	static bool									m_fogEnabled;
	static bool									m_noShadows;
	static bool									m_drawCrosshairs;
	static MPBList								m_shadowVolumeVertexes;
	static GLuint								m_displayListShadowVolume;
	static MPBPhysicsObject*				m_player;
	static MPBVectorList						m_drawPoints;

	static void createFonts();
	static MPBTexture* findTexture( const char* fileName, GLenum texId );
	static void extractFrustum();
};


#endif

