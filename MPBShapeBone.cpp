
#include "MPBGlobal.h"
#include "MPBOpenGL.h"
#include "MPBShapeBone.h"
#include "MPBShapeTube.h"
#include "MPBLine.h"
#include "MPBPhysics.h"
#include "mmgr.h"



MPBShapeBone::MPBShapeBone( MPBFLOAT length ):
	MPBShape()
{
//	MPBLine* line = new MPBLine( MPBVector( 0, 0, 0 ), MPBVector( 0, 0, -length ) );
//	addLine( line );
}

MPBShapeBone::MPBShapeBone( MPBShape* parent, MPBFLOAT length, const char* texture ):
	MPBShape()
{
//	MPBLine* line = new MPBLine( MPBVector( 0, 0, 0 ), MPBVector( 0, 0, -length ) );
//	addLine( line );
	parent->addChild( this );

	MPBFLOAT radius = length * .2;
	addChild( new MPBShapeTube( radius, radius, length, 6, texture, false ) );
}

MPBShapeBone::MPBShapeBone( MPBShape* parent, MPBFLOAT length, MPBFLOAT radius, const char* texture ):
	MPBShape()
{
//	MPBLine* line = new MPBLine( MPBVector( 0, 0, 0 ), MPBVector( 0, 0, -length ) );
//	addLine( line );
	parent->addChild( this );

	addChild( new MPBShapeTube( radius, radius, length, 6, texture, false ) );
}

MPBShapeBone::~MPBShapeBone()
{
}

