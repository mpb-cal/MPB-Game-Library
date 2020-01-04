
#include "MPBGameEntities.h"
#include "MPBGlobal.h"
#include "MPBOpenGL.h"
#include "MPBTexturedPlane.h"
#include "MPBShape.h"
#include "MPBShapeBox.h"
#include "MPBShapeQuad.h"
#include "MPBShapeFrustum.h"
#include "MPBCompositeShape.h"
#include "MPBPartitionTreeNode.h"
#include "MPBCountdown.h"
#include "MPBFrameCounter.h"
#include "MPBPhysics.h"
#include "MPBHeightMap.h"
#include "MPBAABB.h"
#include "MPBGameEntities.h"
#include "MPBControlPanel.h"
#include "MPBCollision.h"
#include "MPBMessageWindow.h"
#include "MPBPhysicsObject.h"
#include "MPBTriStrip.h"
#include "mmgr.h"


dWorldID										MPBGameEntities::worldID;
dJointGroupID								MPBGameEntities::contactJointGroup;
MPBList										MPBGameEntities::planes;
MPBList										MPBGameEntities::shapes;
MPBList										MPBGameEntities::triStrips;
MPBList										MPBGameEntities::mirrors;
MPBList										MPBGameEntities::compositeShapes;
MPBList										MPBGameEntities::countdowns;

MPBList										MPBGameEntities::m_controlPanels;
MPBPartitionTreeNode*					MPBGameEntities::m_shapeOctree = NULL;
MPBHeightMap*								MPBGameEntities::m_heightMap = NULL;
MPBShapeBox*								MPBGameEntities::m_skyBox = NULL;


#define SKYBOX_SIZE (MPBOpenGL::getPerspectiveFar())


void MPBGameEntities::init()
{
	worldID = dWorldCreate();
	dWorldSetGravity( worldID, 0, -9.81, 0 );
//	dWorldSetGravity( worldID, 0, 0, 0 );
//	dWorldSetCFM( worldID, .0001 );	// default = .00001
	contactJointGroup = dJointGroupCreate( 0 );
}

// call MPBPhysics::cleanUp() and MPBOpenGL::cleanUp() first
void MPBGameEntities::cleanUp()
{
	MPB_LIST_DELETE( shapes, MPBShape* )
	MPB_LIST_DELETE( triStrips, MPBTriStrip* )
	MPB_LIST_DELETE( compositeShapes, MPBCompositeShape* );

	dJointGroupDestroy( contactJointGroup );
	dWorldDestroy( worldID );
	dCloseODE();

	m_controlPanels.clear();
	planes.clear();
	countdowns.clear();

	NULLIFY( m_shapeOctree );
	NULLIFY( m_heightMap );
}

void MPBGameEntities::addPlane( MPBTexturedPlane* plane )
{
	if (MPBOpenGL::getPointLight( 0 ))
	{
		plane->addTexture( "generatedLightmap" );
	}

	planes.push_back( plane );
}

void MPBGameEntities::addShape( MPBShape* s )
{
	shapes.push_back( s );

	MPB_LIST_LOOP( s->getPlanes(), p, MPBTexturedPlane* )
	{
		addPlane( p );
	}
}

void MPBGameEntities::addTriStrip( MPBTriStrip* s )
{
	triStrips.push_back( s );
}

void MPBGameEntities::addMirror( MPBShapeQuad* s )
{
	mirrors.push_back( s );

	MPB_LIST_LOOP( s->getPlanes(), p, MPBTexturedPlane* )
	{
		addPlane( p );
	}
}

void MPBGameEntities::addCompositeShape( MPBCompositeShape* s )
{
	compositeShapes.push_back( s );

	MPBList l;
	s->getShapes( l );

	MPB_LIST_LOOP( l, shape, MPBShape* )
	{
		addShape( shape );
	}
}

// heights will have y-values of 0 - 255 units, multiplied by scaleY
// each point will be separated by 1 unit, multiplied by scaleXZ,
// so a raw file of 128 x 128 pixels will be 128 meters wide if scaleXZ = 1
void MPBGameEntities::createHeightMap(	MPBFLOAT scaleXZ, 
													MPBFLOAT scaleY, 
													MPBFLOAT waterLevel,
													char* heightMapFile, 
													bool isBMP,		// else RAW
													char* lightMapFile, 
													char* textureFile )
{
	m_heightMap = new MPBHeightMap();
	m_heightMap->init( heightMapFile, isBMP, textureFile, lightMapFile, scaleXZ, scaleY, waterLevel );
}

MPBFLOAT MPBGameEntities::getWorldWidth()
{
	if (m_heightMap) return m_heightMap->getWidth() * m_heightMap->getScaleXZ();
	return 0;
}

void MPBGameEntities::createSkyBox()
{
	MPBFLOAT texCoord1 = 0;
	MPBFLOAT texCoord2 = 1;

	MPBVector texCoordFront[4] = { 
		MPBVector( texCoord1, texCoord1, 0 ),
		MPBVector( -texCoord2, texCoord1, 0 ),
		MPBVector( -texCoord2, texCoord2, 0 ),
		MPBVector( texCoord1, texCoord2, 0 )
	};

	MPBVector texCoordBack[4] = { 
		MPBVector( texCoord1, texCoord1, 0 ),
		MPBVector( -texCoord2, texCoord1, 0 ),
		MPBVector( -texCoord2, texCoord2, 0 ),
		MPBVector( texCoord1, texCoord2, 0 )
	};

	MPBVector texCoordTop[4] = { 
		MPBVector( texCoord1, texCoord1, 0 ),
		MPBVector( texCoord2, texCoord1, 0 ),
		MPBVector( texCoord2, -texCoord2, 0 ),
		MPBVector( texCoord1, -texCoord2, 0 )
	};

	MPBVector texCoordBottom[4] = { 
		MPBVector( texCoord1, texCoord1, 0 ),
		MPBVector( -texCoord2, texCoord1, 0 ),
		MPBVector( -texCoord2, texCoord2, 0 ),
		MPBVector( texCoord1, texCoord2, 0 )
	};

	MPBVector texCoordLeft[4] = { 
		MPBVector( texCoord1, texCoord1, 0 ),
		MPBVector( -texCoord2, texCoord1, 0 ),
		MPBVector( -texCoord2, texCoord2, 0 ),
		MPBVector( texCoord1, texCoord2, 0 )
	};

	MPBVector texCoordRight[4] = { 
		MPBVector( texCoord1, texCoord1, 0 ),
		MPBVector( -texCoord2, texCoord1, 0 ),
		MPBVector( -texCoord2, texCoord2, 0 ),
		MPBVector( texCoord1, texCoord2, 0 )
	};

	m_skyBox = new MPBShapeBox(
		SKYBOX_SIZE, 
		SKYBOX_SIZE,
		SKYBOX_SIZE,
		"skybox1-3.tex",		// front
		"skybox1-1.tex",		// back
		"skybox1-5.tex",		// top
		"skybox1-6.tex",		// bottom
		"skybox1-4.tex",		// left
		"skybox1-2.tex",		// right
		texCoordFront,
		texCoordBack,
		texCoordTop,
		texCoordBottom,
		texCoordLeft,
		texCoordRight,
		false
	);
/*
	MPBFLOAT scale = 1.001f;
	MPBList planes = m_skyBox->getPlanes();
	((MPBTexturedPlane*)planes.get( 0 ))->scale( scale, scale, 1 );
	((MPBTexturedPlane*)planes.get( 1 ))->scale( scale, scale, 1 );
	((MPBTexturedPlane*)planes.get( 2 ))->scale( scale, 1, scale );
	((MPBTexturedPlane*)planes.get( 3 ))->scale( scale, 1, scale );
	((MPBTexturedPlane*)planes.get( 4 ))->scale( 1, scale, scale );
	((MPBTexturedPlane*)planes.get( 5 ))->scale( 1, scale, scale );
*/
	m_skyBox->setFrustumCull( false );
	m_skyBox->setBackfaceCull( false );
	m_skyBox->setCollidable( false );
	m_skyBox->setDisableFog( true );
	m_skyBox->setCastsShadow( false );
	m_skyBox->setReceivesShadow( false );
	m_skyBox->setLit( false );

	addShape( m_skyBox );
}

void MPBGameEntities::startGameLoop()
{
	// compile shapes

	MPB_LIST_LOOP( shapes, shape, MPBShape* )
	{
//		shape->compile();
	}

	// create shape octree

	MPBVector NLL;
	MPBVector FUR;

	{MPB_LIST_LOOP( shapes, shape, MPBShape* )
	{
		MPBAABB aabb = shape->getAABB();
		NLL.x = min( NLL.x, aabb.getNLL().x );
		NLL.y = min( NLL.y, aabb.getNLL().y );
		NLL.z = max( NLL.z, aabb.getNLL().z );
		FUR.x = max( FUR.x, aabb.getFUR().x );
		FUR.y = max( FUR.y, aabb.getFUR().y );
		FUR.z = min( FUR.z, aabb.getFUR().z );
	}}

	MPB_LIST_LOOP( triStrips, ts, MPBTriStrip* )
	{
		ts->compile();
	}

	MPBAABB worldBounds( NLL, FUR );

//	m_shapeOctree = new MPBPartitionTreeNode( QUADTREE, 16, 20 );
//	m_shapeOctree->buildTree( shapes, worldBounds );
}

void MPBGameEntities::updateFrame()
{
	dWorldStep( worldID, MPBFrameCounter::secondsElapsed );
	dJointGroupEmpty( contactJointGroup );

	MPB_LIST_LOOP( shapes, shape, MPBShape* )
	{
		shape->updateFrame();
	}

	// countdown timers
	MPB_LIST_LOOP( countdowns, countdown, MPBCountdown* )
	{
		countdown->update( (int)(MPBFrameCounter::secondsElapsed * 1000.0) );
	}

	if (m_skyBox) m_skyBox->translateDraw(
		MPBOpenGL::getCameraPos().x, 
		MPBOpenGL::getCameraPos().y, 
		MPBOpenGL::getCameraPos().z 
	);
}


void MPBGameEntities::draw( bool shadowersOnly )
{
	//////////////////////////////////////////////////////////////
	// draw height map

	if (m_heightMap) m_heightMap->draw();

	//////////////////////////////////////////////////////////////
	// draw shapes

	MPB_LIST_LOOP( shapes, shape, MPBShape* )
	{
		shape->setDrawn( false );
	}

	if (m_shapeOctree)
	{
//		m_shapeOctree->draw();
		m_shapeOctree->startDraw();
		m_shapeOctree->drawShapes( false, shadowersOnly );
	}
	else
	{
		MPB_LIST_LOOP( shapes, shape, MPBShape* )
		{
			shape->draw( !shadowersOnly, shadowersOnly, true );
		}
	}

	//////////////////////////////////////////////////////////////
	// draw tri strips

	MPB_LIST_LOOP( triStrips, ts, MPBTriStrip* )
	{
		ts->draw();
	}


	//////////////////////////////////////////////////////////////
	// draw transparent shapes.

	glPushAttrib( GL_ALL_ATTRIB_BITS );
	glEnable( GL_BLEND );
	glBlendFunc( GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA );

	// sort by distance to camera so they are farthest to nearest
//	sort( m_alphaShapeVector.begin(), m_alphaShapeVector.end(), SortShapeFunc );

	{MPB_LIST_LOOP( shapes, shape, MPBShape* )
	{
		if (shape->getAlpha())
		{
			shape->setDrawn( false );
			shape->draw( !shadowersOnly, shadowersOnly, true );
		}
	}}

	glPopAttrib();
}

void MPBGameEntities::addControlPanel( MPBControlPanel* s )
{
	m_controlPanels.push_back( s );
}

void MPBGameEntities::removeControlPanel( MPBControlPanel* s )
{
	m_controlPanels.remove( s );
}

void MPBGameEntities::drawControlPanels()
{
	MPB_LIST_LOOP( m_controlPanels, cp, MPBControlPanel* )
	{
		MPBOpenGL::controlPanelBegin();
		MPBOpenGL::controlPanelPosition( cp->x, cp->y );
		MPBOpenGL::bindTexture( cp->texture );
		glBegin( GL_QUADS );
		glTexCoord2d( cp->tcx0, cp->tcy0 ); glVertex2d( -cp->width/2,	-cp->height/2 );
		glTexCoord2d( cp->tcx1, cp->tcy0 ); glVertex2d(  cp->width/2,	-cp->height/2 );
		glTexCoord2d( cp->tcx1, cp->tcy1 ); glVertex2d(  cp->width/2,	 cp->height/2 );
		glTexCoord2d( cp->tcx0, cp->tcy1 ); glVertex2d( -cp->width/2,	 cp->height/2 );
		glEnd();
		MPBOpenGL::controlPanelEnd();
	}
}

void* MPBGameEntities::addDPPlayer( DPNID dpnid )
{
	return (void*)789;
}

void MPBGameEntities::removeDPPlayer( DPNID dpnid )
{
}

void MPBGameEntities::inputDPPlayer( DPNID dpnid, ClientInput* clientInput )
{
}

void MPBGameEntities::savePOVFile( char* filename )
{
	MPBVector lightPosition;
	char buf[1000];

	//////////////////////////////////////
	// test file

	sprintf( g_logBuf, "%s.pov", filename );
	FILE* fp = fopen( g_logBuf, "w" );
	MPB_ASSERT( fp );

	fputs( "#include \"colors.inc\"\n", fp );
	fputs( "#include \"textures.inc\"\n", fp );
	fputs( "\n", fp );

	fputs( "//global_settings { radiosity {} }\n\n", fp );

	fputs( "camera {\n", fp );
	fputs( " location <5, 0, -9>\n", fp );
	fputs( " look_at  <-1, 0, -8>\n", fp );
	fputs( " right<-1.33,0,0>\n", fp );
	fputs( "}\n", fp );
	fputs( "\n", fp );

	int i = 0;
	MPBPhysicsObject* light;
	while (light = MPBOpenGL::getStaticPointLight( i ))
	{
		lightPosition = MPBOpenGL::getStaticPointLight( i )->getPosition();

		fputs( "light_source {\n", fp );
		lightPosition.toString( buf ); 
		fprintf( fp, " <%s> color White\n", buf ); 
		fprintf( fp, " fade_distance %f\n", MPBOpenGL::getStaticPointLight( i )->getRadius() ); 
		fputs( " fade_power 1\n", fp );

		fputs( " adaptive 1\n", fp );
		fputs( " jitter\n", fp );
		fputs( "}\n", fp );

		i++;
	}

	MPB_LIST_LOOP( shapes, shape, MPBShape* )
	{
		if (shape == m_skyBox) continue;
		shape->savePOVFile( fp );
	}

	fclose( fp );

	//////////////////////////////////////
	// lightmaps

	int j = 0;
	MPB_LIST_LOOP( planes, visiblePlane, Plane* )
	{
		sprintf( g_logBuf, "%s_%d.pov", filename, j );
		FILE* fp = fopen( g_logBuf, "w" );
		MPB_ASSERT( fp );

		fputs( "#include \"colors.inc\"\n", fp );
		fputs( "#include \"textures.inc\"\n", fp );
		fputs( "\n", fp );

		fputs( "//global_settings { radiosity {} }\n\n", fp );

//		fputs( "camera {\n", fp );
//		fputs( " location <1, 2, 16>\n", fp );
//		fputs( " look_at  <0, 1, 0>\n", fp );
//		fputs( " right<-1.33,0,0>\n", fp );
//		fputs( "}\n", fp );
//		fputs( "\n", fp );

		fputs( "camera {\n", fp );
		fputs( " orthographic\n", fp );

		(visiblePlane->getCenter() + visiblePlane->getNormal()).toString( buf ); 
		fprintf( fp, " location <%s>\n", buf ); 

		visiblePlane->getCenter().toString( buf ); 
		fprintf( fp, " look_at <%s>\n", buf ); 

		(visiblePlane->getUL() - visiblePlane->getLL()).toString( buf ); 
		fprintf( fp, " sky <%s>\n", buf ); 

		fprintf( fp, " right<-1,0,0> * %f\n", (visiblePlane->getLR() - visiblePlane->getLL()).getLength() ); 
		fprintf( fp, " up<0,1,0> * %f\n", (visiblePlane->getUL() - visiblePlane->getLL()).getLength() ); 
		fputs( "}\n", fp );
		fputs( "\n", fp );

		int i = 0;
		MPBPhysicsObject* light;
		while (light = MPBOpenGL::getStaticPointLight( i ))
		{
			lightPosition = MPBOpenGL::getStaticPointLight( i )->getPosition();

			fputs( "light_source {\n", fp );
			lightPosition.toString( buf ); 
			fprintf( fp, " <%s> color White\n", buf ); 
			fprintf( fp, " fade_distance %f\n", MPBOpenGL::getStaticPointLight( i )->getRadius() ); 
			fputs( " fade_power 1\n", fp );

		//	if (MPBOpenGL::getPointLight( 0 ))
		//		fputs( " area_light <1, 0, 0>, <0, 0, 1>, 5, 5\n", fp );
		//	else
		//		fputs( " area_light <4000, 0, 0>, <0, 0, 4000>, 5, 5\n", fp );

			fputs( " adaptive 1\n", fp );
			fputs( " jitter\n", fp );
//			fputs( " looks_like {\n", fp );
//			fputs( "  sphere {\n", fp );
//			fputs( "   <0, 0, 0>, .2\n", fp );
//			fputs( "   texture {\n", fp );
//			fputs( "    pigment { color White }\n", fp );
//			fputs( "    finish {ambient 1 diffuse 1}\n", fp );
//			fputs( "   }\n", fp );
//			fputs( "  }\n", fp );
//			fputs( " }\n", fp );
			fputs( "}\n", fp );

			i++;
		}

//		MPB_LIST_LOOP( shapes, MPBShape* )
//		{
//			MPBShape* shape = *itr;
//			if (shape == m_skyBox) continue;
//			shape->savePOVFile( fp );
//		}

		char buf[1000];
		char buf2[1000];

//		MPB_LIST_LOOP( planes, plane, Plane* )
		for (int p=0; p<planes.size(); p++)
		{
			Plane* plane = (Plane*)planes[p];

			fputs( "polygon {\n", fp );
			fputs( " 4,\n", fp );

			plane->getLL().toString( buf ); sprintf( buf2, " <%s>,\n", buf ); fputs( buf2, fp );
			plane->getLR().toString( buf ); sprintf( buf2, " <%s>,\n", buf ); fputs( buf2, fp );
			plane->getUR().toString( buf ); sprintf( buf2, " <%s>,\n", buf ); fputs( buf2, fp );
			plane->getUL().toString( buf ); sprintf( buf2, " <%s>\n", buf ); fputs( buf2, fp );

			fputs( " texture { \n", fp );
	//		fputs( "  pigment { DMFWood4 scale 4 }\n", fp );
			fputs( "  pigment { color White }\n", fp );
			fputs( " }\n", fp );
			if (visiblePlane != plane)
				fputs( " no_image\n", fp );
			fputs( "}\n", fp );
			fputs( "\n", fp );
		}

		fclose( fp );
		j++;
	}
}











