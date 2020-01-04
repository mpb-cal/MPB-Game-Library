#ifndef __MPBGAMEENTITIES_H__
#define __MPBGAMEENTITIES_H__

#include "MPBLibrary.h"
#include "MPBCollision.h"
#include "MPBList.h"
#include "ode/ode.h"

//class MPBDirectDrawSprite;
class Plane;
class MPBTexturedPlane;
class MPBPhysicsObject;
//class MPBSpring;
//class MPBSpringTriangle;
class MPBShape;
class MPBShapeQuad;
class MPBCompositeShape;
class MPBShapeBox;
class MPBPartitionTreeNode;
class MPBCountdown;
class MPBHeightMap;
class MPBAABB;
class MPBControlPanel;
class MPBCompositeShapeCell;
//class MPBFlock;
//class MPBUsableObject;
//class MPBHumanPlayer;
//class MPBNPC;

struct t3DModel;
struct ClientUpdate;
struct ClientInput;


class MPBGameEntities
{
	public:

		static void init();
		static void startGameLoop();
		static void cleanUp();
		static void addPlane( MPBTexturedPlane* plane );
		static void addSpring( MPBSpring* spring );
		static void addSpringTriangle( MPBSpringTriangle* st );
		static void addShape( MPBShape* s );
		static void addMirror( MPBShapeQuad* s );
		static void addCompositeShape( MPBCompositeShape* s );
//		static void add3DSModel( char* filename );
		static void addNPC( MPBNPC* n );
		static void addFlock( MPBFlock* f );
		static void addUsableObject( MPBUsableObject* o );
		static MPBUsableObject* getUsableObject( MPBVector position );
		static void useObject1( MPBHumanPlayer* user );
		static void addControlPanel( MPBControlPanel* s );
		static void removeControlPanel( MPBControlPanel* s );
		static void removeUsableObject( MPBUsableObject* o );
		static void createHeightMap( MPBFLOAT scaleXZ, MPBFLOAT scaleY, char* textureFile );
		static void createSkyBox();
		static void updateFrame();
		static void draw( bool shadowersOnly );
		static void drawControlPanels();
		static void getCollisionPlanes( MPBList& planeList, MPBAABB& moveBox );
		static MPBFLOAT findHighestPoint( MPBVector LL, MPBVector LR, MPBVector UR, MPBVector UL );
		static MPBFLOAT findHighestPoint( MPBFLOAT x, MPBFLOAT z, MPBFLOAT width, MPBFLOAT depth );
		static MPBFLOAT getWorldWidth();
		static void* addDPPlayer( DPNID dpnid );
		static void removeDPPlayer( DPNID dpnid );
		static void inputDPPlayer( DPNID dpnid, ClientInput* clientInput );
		static void savePOVFile( char* filename );
		static void addCell( MPBCompositeShapeCell* cell );
		static bool aabbInPortalFrustum( const MPBAABB& aabb );
		static bool sphereInPortalFrustum( const MPBVector& center, MPBFLOAT radius );
		static MPBHeightMap* getHeightMap() { return m_heightMap; }
		static MPBShapeBox* getSkyBox() { return m_skyBox; }
		static MPBCompositeShapeCell* getCurrentCell() { return m_currentCell; }
		static MPBCompositeShapeCell* getCell( MPBVector position );

		static dWorldID									worldID;
		static dJointGroupID								contactJointGroup;
		static MPBList										planes;
//		static MPBList										springs;
//		static MPBList										springTriangles;
		static MPBList										shapes;
		static MPBList										mirrors;
		static MPBList										compositeShapes;
		static MPBList										countdowns;
//		static MPBList										flocks;
//		static MPBList										usableObjects;
//		static MPBList										npcs;
		static MPBList										cells;


	private:

		static MPBCompositeShapeCell*					m_currentCell;
		static MPBShapeBox*								m_skyBox;
		static MPBPartitionTreeNode*					m_shapeOctree;
		static MPBHeightMap*								m_heightMap;
//		static MPBList										m_3DSModels;
		static MPBList										m_controlPanels;

};




#endif

