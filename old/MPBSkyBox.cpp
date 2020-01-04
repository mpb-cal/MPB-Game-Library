
#include "MPBGlobal.h"
#include "MPBSkyBox.h"


MPBSkyBox::MPBSkyBox() :
	MPBShape()
{
	setFrustumCull( false );
	setBackfaceCull( false );
	setDisableFog( true );
	setDisableLighting( true );

	float rotZ = 90;
	float radius = 1000;
	float scaleY = .4f;

///////////

	StillbotQuad* child;

	int maxWidth = 2 * sqrt( 3 ) / 3 * GE::getPerspectiveFar();
	int extent = maxWidth / 2;
	int extentStretched = extent + METER * 10;
	MPBVector LL;
	MPBVector LR;
	MPBVector UR;
	MPBVector UL;

	// top
	LL.set( -extentStretched,  extent, -extentStretched );
	LR.set(  extentStretched,  extent, -extentStretched );
	UR.set(  extentStretched,  extent,  extentStretched );
	UL.set( -extentStretched,  extent,  extentStretched );
	child = new StillbotQuad( LL, LR, UR, UL );
//		child->setTexture( "skybox1-5 copy.bmp" );
	child->setTexture( "skybox0005.bmp" );
	addChild( child );

	// bottom
	LL.set( -extentStretched, -extent,  extentStretched );
	LR.set(  extentStretched, -extent,  extentStretched );
	UR.set(  extentStretched, -extent, -extentStretched );
	UL.set( -extentStretched, -extent, -extentStretched );
	child = new StillbotQuad( LL, LR, UR, UL );
//		child->setTexture( "skybox1-6 copy.bmp" );
	addChild( child );

	// left
	LL.set( -extent, -extentStretched,  extentStretched );
	LR.set( -extent, -extentStretched, -extentStretched );
	UR.set( -extent,  extentStretched, -extentStretched );
	UL.set( -extent,  extentStretched,  extentStretched );
	child = new StillbotQuad( LL, LR, UR, UL );
//		child->setTexture( "skybox1-4 copy.bmp" );
	child->setTexture( "skybox0004.bmp" );
	addChild( child );

	// right
	LL.set(  extent, -extentStretched, -extentStretched );
	LR.set(  extent, -extentStretched,  extentStretched );
	UR.set(  extent,  extentStretched,  extentStretched );
	UL.set(  extent,  extentStretched, -extentStretched );
	child = new StillbotQuad( LL, LR, UR, UL );
//		child->setTexture( "skybox1-2 copy.bmp" );
	child->setTexture( "skybox0002.bmp" );
	addChild( child );

	// front
	LL.set(  extentStretched, -extentStretched,  extent );
	LR.set( -extentStretched, -extentStretched,  extent );
	UR.set( -extentStretched,  extentStretched,  extent );
	UL.set(  extentStretched,  extentStretched,  extent );
	child = new StillbotQuad( LL, LR, UR, UL );
//		child->setTexture( "skybox1-3 copy.bmp" );
	child->setTexture( "skybox0003.bmp" );
	addChild( child );

	// back
	LL.set( -extentStretched, -extentStretched, -extent );
	LR.set(  extentStretched, -extentStretched, -extent );
	UR.set(  extentStretched,  extentStretched, -extent );
	UL.set( -extentStretched,  extentStretched, -extent );
	child = new StillbotQuad( LL, LR, UR, UL );
//		child->setTexture( "skybox1-1 copy.bmp" );
	child->setTexture( "skybox0001.bmp" );
	addChild( child );
}

void MPBSkyBox::updateFrame()
{
//	setPosition( GE::getPlayerRobot()->getPosition() );
}

