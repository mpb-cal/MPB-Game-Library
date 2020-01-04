
#include "MPBLibrary.h"
#include "MPBPlane.h"
#include "MPBShadow.h"
//#include "Robot.h"
#include "MPBOpenGL.h"
#include "mmgr.h"


static int projectionShadowCallDepth = 0;

void MPBShadow::projectionShadowBegin()
{
	projectionShadowCallDepth++;

	glPushAttrib( GL_ALL_ATTRIB_BITS );

	glDisable( GL_LIGHTING );
	glDisable( GL_TEXTURE_1D );
	glDisable( GL_TEXTURE_2D );
	glDisable( GL_TEXTURE_3D );
	glDisable( GL_FOG );

	glEnable( GL_POLYGON_OFFSET_FILL );
	glPolygonOffset( -2.0, -2.0 );

	glEnable( GL_STENCIL_TEST );
	glClear( GL_STENCIL_BUFFER_BIT );

	glEnable( GL_CULL_FACE );

	glEnable( GL_BLEND );
	glBlendFunc( GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA );

	glEnable( GL_DEPTH_TEST );
	glDepthMask( GL_TRUE );
	glDepthFunc( GL_LEQUAL );

	glColorMask( GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE );
	glEnable( GL_COLOR_MATERIAL );
	glColorMaterial( GL_FRONT, GL_AMBIENT_AND_DIFFUSE );
	glColor4f( 0, 0, 0, .2f );
}

void MPBShadow::projectionShadowPrepReceiver( Plane* receiver, int index )
{
	MPB_ASSERT( projectionShadowCallDepth == 1 );	// must call projectionShadowBegin() first!

	// draw the plane into the stencil buffer
	glStencilFunc( GL_NEVER, index, ~0 );
	glStencilOp( GL_REPLACE, GL_REPLACE, GL_REPLACE );
	glColorMask( GL_FALSE, GL_FALSE, GL_FALSE, GL_FALSE );

	glBegin( GL_QUADS );
	glVertex3d( receiver->getLL().x, receiver->getLL().y, receiver->getLL().z );
	glVertex3d( receiver->getLR().x, receiver->getLR().y, receiver->getLR().z );
	glVertex3d( receiver->getUR().x, receiver->getUR().y, receiver->getUR().z );
	glVertex3d( receiver->getUL().x, receiver->getUL().y, receiver->getUL().z );
	glEnd();

	// draw the shadow wherever the plane left its number in the stencil buffer
	glStencilFunc( GL_EQUAL, index, ~0 );
	glStencilOp( GL_KEEP, GL_KEEP, GL_KEEP );	// the shadow doesn't change the stencil buffer?
	MPBOpenGL::resetCamera();
	glColorMask( GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE );
}

void MPBShadow::projectionShadowEnd()
{
	MPB_ASSERT( projectionShadowCallDepth == 1 );	// must call projectionShadowBegin() first!
	projectionShadowCallDepth--;

	glDisable( GL_POLYGON_OFFSET_FILL );
	glPopAttrib();
}


// returns a matrix for drawing an object squashed onto a plane.
// shadowMatrix is 4 x 4:
//		0	4	8	12
//		1	5	9	13
//		2	6	10	14
//		3	7	11	15
void MPBShadow::projectionShadowMatrix(	MPBFLOAT* shadowMatrix, 
														const MPBVector& lightPosition, 
														const Plane* receivingPlane	)
{
	MPBFLOAT nx = receivingPlane->getNormal().x;
	MPBFLOAT ny = receivingPlane->getNormal().y;
	MPBFLOAT nz = receivingPlane->getNormal().z;
	MPBFLOAT d = receivingPlane->getD();

	MPBFLOAT dp = receivingPlane->getNormal().dotProduct( lightPosition ) + d;

	// row 1
	shadowMatrix[0]  = dp -lightPosition.x * nx;
	shadowMatrix[4]  =    -lightPosition.x * ny;
	shadowMatrix[8]  =    -lightPosition.x * nz;
	shadowMatrix[12] =    -lightPosition.x * d;

	// row 2
	shadowMatrix[1]  =    -lightPosition.y * nx;
	shadowMatrix[5]  = dp -lightPosition.y * ny;
	shadowMatrix[9]  =    -lightPosition.y * nz;
	shadowMatrix[13] =    -lightPosition.y * d;

	// row 3
	shadowMatrix[2]  =    -lightPosition.z * nx;
	shadowMatrix[6]  =    -lightPosition.z * ny;
	shadowMatrix[10] = dp -lightPosition.z * nz;
	shadowMatrix[14] =    -lightPosition.z * d;

	// row 4
	shadowMatrix[3]  =    -nx;
	shadowMatrix[7]  =    -ny;
	shadowMatrix[11] =    -nz;
	shadowMatrix[15] = dp -d;
}

