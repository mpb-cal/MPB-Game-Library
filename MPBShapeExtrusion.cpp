
#include "MPBGlobal.h"
#include "MPBShapeExtrusion.h"
#include "MPBTexturedPlane.h"
#include "MPBOpenGL.h"
#include "mmgr.h"


MPBShapeExtrusion::MPBShapeExtrusion(	Plane* plane, 
													MPBVector extrusion,
													const char* texture, 
													MPBFLOAT texSizeX, 
													MPBFLOAT texSizeY	):
	MPBShape()
{
//	m_texCoordX = texCoordX;
//	m_texCoordY = texCoordY;

	strcpy( m_texture, texture );

	MPBTexturedPlane* tp;

	// front
	tp = new MPBTexturedPlane(	plane->getLL(), 
										plane->getLR(), 
										plane->getUR(), 
										plane->getUL()	);
	tp->addTexture( texture );
	tp->setTexSize( 0, texSizeX, texSizeY );
	addPlane( tp );

	// back
	tp = new MPBTexturedPlane(	plane->getLR() + extrusion, 
										plane->getLL() + extrusion, 
										plane->getUL() + extrusion, 
										plane->getUR() + extrusion	);
	tp->addTexture( texture );
	tp->setTexSize( 0, texSizeX, texSizeY );
	addPlane( tp );

	// left
	tp = new MPBTexturedPlane(	plane->getLL() + extrusion, 
										plane->getLL(),
										plane->getUL(),
										plane->getUL() + extrusion	);
	tp->addTexture( texture );
	tp->setTexSize( 0, texSizeX, texSizeY );
	addPlane( tp );

	// right
	tp = new MPBTexturedPlane(	plane->getLR(),
										plane->getLR() + extrusion,
										plane->getUR() + extrusion,
										plane->getUR()	);
	tp->addTexture( texture );
	tp->setTexSize( 0, texSizeX, texSizeY );
	addPlane( tp );

	// top
	tp = new MPBTexturedPlane(	plane->getUL(),
										plane->getUR(),
										plane->getUR() + extrusion, 
										plane->getUL() + extrusion	);
	tp->addTexture( texture );
	tp->setTexSize( 0, texSizeX, texSizeY );
	addPlane( tp );

	// bottom
	tp = new MPBTexturedPlane(	plane->getLL() + extrusion, 
										plane->getLR() + extrusion, 
										plane->getLR(),
										plane->getLL()	);
	tp->addTexture( texture );
	tp->setTexSize( 0, texSizeX, texSizeY );
	addPlane( tp );
}

MPBShapeExtrusion::~MPBShapeExtrusion()
{
}





