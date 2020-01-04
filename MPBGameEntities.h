#ifndef __MPBGAMEENTITIES_H__
#define __MPBGAMEENTITIES_H__

#include "MPBLibrary.h"
#include "MPBCollision.h"
#include "MPBList.h"
#include "ode/ode.h"

class Plane;
class MPBTexturedPlane;
class MPBPhysicsObject;
class MPBShape;
class MPBTriStrip;
class MPBShapeQuad;
class MPBCompositeShape;
class MPBShapeBox;
class MPBPartitionTreeNode;
class MPBCountdown;
class MPBHeightMap;
class MPBAABB;
class MPBControlPanel;
class MPBCompositeShapeCell;

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
		static void addShape( MPBShape* s );
		static void addTriStrip( MPBTriStrip* s );
		static void addMirror( MPBShapeQuad* s );
		static void addCompositeShape( MPBCompositeShape* s );
		static void addControlPanel( MPBControlPanel* s );
		static void removeControlPanel( MPBControlPanel* s );
		static void createHeightMap(	MPBFLOAT scaleXZ, 
												MPBFLOAT scaleY, 
												MPBFLOAT waterLevel,
												char* heightMapFile, 
												bool isBMP,		// else RAW
												char* lightMapFile, 
												char* textureFile );
		static void createSkyBox();
		static void updateFrame();
		static void draw( bool shadowersOnly );
		static void drawControlPanels();
		static MPBFLOAT findHighestPoint( MPBVector LL, MPBVector LR, MPBVector UR, MPBVector UL );
		static MPBFLOAT findHighestPoint( MPBFLOAT x, MPBFLOAT z, MPBFLOAT width, MPBFLOAT depth );
		static MPBFLOAT getWorldWidth();
		static void* addDPPlayer( DPNID dpnid );
		static void removeDPPlayer( DPNID dpnid );
		static void inputDPPlayer( DPNID dpnid, ClientInput* clientInput );
		static void savePOVFile( char* filename );
		static MPBHeightMap* getHeightMap() { return m_heightMap; }
		static MPBShapeBox* getSkyBox() { return m_skyBox; }

		static dWorldID									worldID;
		static dJointGroupID								contactJointGroup;
		static MPBList										planes;
		static MPBList										shapes;
		static MPBList										triStrips;
		static MPBList										mirrors;
		static MPBList										compositeShapes;
		static MPBList										countdowns;


	private:

		static MPBShapeBox*								m_skyBox;
		static MPBPartitionTreeNode*					m_shapeOctree;
		static MPBHeightMap*								m_heightMap;
		static MPBList										m_controlPanels;

};




#endif

