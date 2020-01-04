
#include "MPBGlobal.h"
#include "MPBOpenGL.h"
#include "MPBShape.h"
#include "MPBTexturedPlane.h"
#include "MPBPhysics.h"
#include "MPBPerformanceMonitor.h"
#include "MPBLine.h"
#include "MPBAnimation.h"
#include "MPBFrameCounter.h"
#include "MPBGameEntities.h"
#include "MPBGeometry.h"
#include "MPBQuaternion.h"
#include "MPBMatrix.h"
#include "mmgr.h"

//
// essentially a list of TexturedPlanes
//


/*
// The following are material definitions for the body.
// Ambiance, Diffuse, Specular, Shininess

MPBFLOAT Brass[] = {
    0.329412, 0.223529, 0.027451, 1.000000,
    0.780392, 0.568627, 0.113725, 1.000000,
    0.992157, 0.941176, 0.807843, 1.000000,
    27.897400
};

MPBFLOAT Bronze[] = {
    0.212500, 0.127500, 0.054000, 1.000000,
    0.714000, 0.428400, 0.181440, 1.000000,
    0.393548, 0.271906, 0.166721, 1.000000,
    25.600000
};

MPBFLOAT Polished_Bronze[] = {
    0.250000, 0.148000, 0.064750, 1.000000,
    0.400000, 0.236800, 0.103600, 1.000000,
    0.774597, 0.458561, 0.200621, 1.000000,
    76.800003
};

MPBFLOAT Chrome[] = {
    0.250000, 0.250000, 0.250000, 1.000000,
    0.400000, 0.400000, 0.400000, 1.000000,
    0.774597, 0.774597, 0.774597, 1.000000,
    76.800003
};

MPBFLOAT Copper[] = {
    0.191250, 0.073500, 0.022500, 1.000000,
    0.703800, 0.270480, 0.082800, 1.000000,
    0.256777, 0.137622, 0.086014, 1.000000,
    12.800000
};

MPBFLOAT Polished_Copper[] = {
    0.229500, 0.088250, 0.027500, 1.000000,
    0.550800, 0.211800, 0.066000, 1.000000,
    0.580594, 0.223257, 0.069570, 1.000000,
    51.200001
};

MPBFLOAT Gold[] = {
    0.247250, 0.199500, 0.074500, 1.000000,
    0.751640, 0.606480, 0.226480, 1.000000,
    0.628281, 0.555802, 0.366065, 1.000000,
    51.200001
};

MPBFLOAT Polished_Gold[] = {
    0.247250, 0.224500, 0.064500, 1.000000,
    0.346150, 0.314300, 0.090300, 1.000000,
    0.797357, 0.723991, 0.208006, 1.000000,
    83.199997
};

MPBFLOAT Pewter[] = {
    0.105882, 0.058824, 0.113725, 1.000000,
    0.427451, 0.470588, 0.541176, 1.000000,
    0.333333, 0.333333, 0.521569, 1.000000,
    9.846150
};

MPBFLOAT Silver[] = {
    0.192250, 0.192250, 0.192250, 1.000000,
    0.507540, 0.507540, 0.507540, 1.000000,
    0.508273, 0.508273, 0.508273, 1.000000,
    51.200001
};

MPBFLOAT Polished_Silver[] = {
    0.231250, 0.231250, 0.231250, 1.000000,
    0.277500, 0.277500, 0.277500, 1.000000,
    0.773911, 0.773911, 0.773911, 1.000000,
    89.599998
};

MPBFLOAT Emerald[] = {
    0.021500, 0.174500, 0.021500, 0.550000,
    0.075680, 0.614240, 0.075680, 0.550000,
    0.633000, 0.727811, 0.633000, 0.550000,
    76.800003
};

MPBFLOAT Jade[] = {
    0.135000, 0.222500, 0.157500, 0.950000,
    0.540000, 0.890000, 0.630000, 0.950000,
    0.316228, 0.316228, 0.316228, 0.950000,
    12.800000
};

MPBFLOAT Obsidian[] = {
    0.053750, 0.050000, 0.066250, 0.820000,
    0.182750, 0.170000, 0.225250, 0.820000,
    0.332741, 0.328634, 0.346435, 0.820000,
    38.400002
};

MPBFLOAT Pearl[] = {
    0.250000, 0.207250, 0.207250, 0.922000,
    1.000000, 0.829000, 0.829000, 0.922000,
    0.296648, 0.296648, 0.296648, 0.922000,
    11.264000
};

MPBFLOAT Ruby[] = {
    0.174500, 0.011750, 0.011750, 0.550000,
    0.614240, 0.041360, 0.041360, 0.550000,
    0.727811, 0.626959, 0.626959, 0.550000,
    76.800003
};

MPBFLOAT Turquoise[] = {
    0.100000, 0.187250, 0.174500, 0.800000,
    0.396000, 0.741510, 0.691020, 0.800000,
    0.297254, 0.308290, 0.306678, 0.800000,
    12.800000
};

MPBFLOAT Black_Plastic[] = {
    0.000000, 0.000000, 0.000000, 1.000000,
    0.010000, 0.010000, 0.010000, 1.000000,
    0.500000, 0.500000, 0.500000, 1.000000,
    32.000000
};

MPBFLOAT Black_Rubber[] = {
    0.020000, 0.020000, 0.020000, 1.000000,
    0.010000, 0.010000, 0.010000, 1.000000,
    0.400000, 0.400000, 0.400000, 1.000000,
    10.000000
};

static const GLfloat ambientColorDefault[4] = { Chrome[0], Chrome[1], Chrome[2], Chrome[3] };
static const GLfloat diffuseColorDefault[4] = { Chrome[4], Chrome[5], Chrome[6], Chrome[7] };
static const GLfloat specularColorDefault[4] = { Chrome[8], Chrome[9], Chrome[10], Chrome[11] };
static const GLfloat shininessDefault[1] = { Chrome[12] };
static const GLfloat emissionColorDefault[4] = { 0, 0, 0, 1 };
*/


// defaults
/*
static const GLfloat ambientColorDefault[4] = { .5, .5, .5, 1 };
//static const GLfloat ambientColorDefault[4] = { 1, 1, 1, 1 };	// when using shadows
static const GLfloat diffuseColorDefault[4] = { .5, .5, .5, 1 };
//static const GLfloat diffuseColorDefault[4] = { 1, 1, 1, 1 };	// when using shadows
static const GLfloat specularColorDefault[4] = { 0, 0, 0, 1 };
static const GLfloat emissionColorDefault[4] = { 0, 0, 0, 1 };
static const GLfloat shininessDefault[1] = { 0 };
*/
static const GLfloat ambientColorDefault[4] = { 1, 1, 1, 1 };
static const GLfloat diffuseColorDefault[4] = { 1, 1, 1, 1 };
static const GLfloat specularColorDefault[4] = { 0, 0, 0, 1 };
static const GLfloat emissionColorDefault[4] = { 0, 0, 0, 1 };
static const GLfloat shininessDefault[1] = { 128 };


MPBShape::MPBShape() :
	m_collidable( true ),
	m_frustumCull( true ),
	m_backfaceCull( true ),
	m_draw( true ),
	m_compiled( false ),
	m_compile( true ),
//	m_billboard( false ),
	m_drawn( false ),
	m_castsShadow( true ),
	m_disableFog( false ),
	m_isAlpha( false ),
	m_isLit( true ),
	m_receivesShadow( true ),
	m_cell( NULL ),
	m_geomID( 0 ),
	m_elasticity( 1 )
{
	resetColor();
	resetDrawMatrix();
	resetAnimationMatrix();
}

MPBShape::~MPBShape()
{
	MPB_LIST_DELETE( m_planes, MPBTexturedPlane* );
	MPB_LIST_DELETE( m_lines, MPBLine* );
	MPB_LIST_DELETE( m_children, MPBShape* );
	MPB_LIST_DELETE( m_animations, MPBAnimation* );
}

void MPBShape::updateFrame()
{
/*	if (m_billboard)	// mpb! only works for squares?
	{
		MPBOpenGL::resetCamera();
		GLfloat cameraMatrix[16];
		glGetFloatv( GL_MODELVIEW_MATRIX, cameraMatrix );
		MPBVector cameraXAxis( cameraMatrix[0], cameraMatrix[4], cameraMatrix[8] );
		MPBVector cameraYAxis( cameraMatrix[1], cameraMatrix[5], cameraMatrix[9] );
		m_billboardOffset1 = (cameraXAxis * -1 + cameraYAxis * -1);
		m_billboardOffset2 = (cameraXAxis *  1 + cameraYAxis * -1);
		m_billboardOffset3 = (cameraXAxis *  1 + cameraYAxis *  1);
		m_billboardOffset4 = (cameraXAxis * -1 + cameraYAxis *  1);
	}
*/
	resetAnimationMatrix();

	MPB_LIST_LOOP( m_animations, a, MPBAnimation* )
	{
		if (a->isRunning())
		{
			a->updateFrame( this );
		}
	}

	resetDrawMatrix();

	MPB_LIST_LOOP( m_children, c, MPBShape* )
	{
		c->updateFrame();
	}
}

// a child shape's position is absolute but transformed by draw matrix and animation matrix
void MPBShape::addChild( MPBShape* s )
{
//	s->setCastsShadow( false );
	s->setFrustumCull( false );
	s->setCollidable( false );
	m_children.push_back( s );
}

// called by child classes
void MPBShape::addPlane( MPBTexturedPlane* plane )
{
	plane->setCollidable( m_collidable );
	m_planes.push_back( plane );
	findAABB();
}

void MPBShape::addTexture( const char* name )
{
	MPB_LIST_LOOP( m_planes, p, MPBTexturedPlane* )
	{
		p->addTexture( name );
	}
}

void MPBShape::setDrawn( bool set ) 
{ 
	m_drawn = set; 

	MPB_LIST_LOOP( m_children, c, MPBShape* )
	{
		c->setDrawn( set );
	}
}

void MPBShape::addLine( MPBLine* l )
{
	m_lines.push_back( l );
	findAABB();
}

void MPBShape::addAnimation( MPBAnimation* l )
{
	m_animations.push_back( l );
}

void MPBShape::scaleTimeAnimations( MPBFLOAT factor )
{
	MPB_LIST_LOOP( m_animations, a, MPBAnimation* )
	{
		a->scaleTime( factor );
	}

	MPB_LIST_LOOP( m_children, c, MPBShape* )
	{
		c->scaleTimeAnimations( factor );
	}
}

void MPBShape::startAnimations()
{
	MPB_LIST_LOOP( m_animations, a, MPBAnimation* )
	{
		a->start();
	}

	MPB_LIST_LOOP( m_children, c, MPBShape* )
	{
		c->startAnimations();
	}
}

void MPBShape::pauseAnimations()
{
	MPB_LIST_LOOP( m_animations, a, MPBAnimation* )
	{
		a->pause();
	}

	MPB_LIST_LOOP( m_children, c, MPBShape* )
	{
		c->pauseAnimations();
	}
}

void MPBShape::pauseAnimationsWhenDone()
{
	MPB_LIST_LOOP( m_animations, a, MPBAnimation* )
	{
		a->pauseWhenDone();
	}

	MPB_LIST_LOOP( m_children, c, MPBShape* )
	{
		c->pauseAnimationsWhenDone();
	}
}

void MPBShape::unpauseAnimations()
{
	MPB_LIST_LOOP( m_animations, a, MPBAnimation* )
	{
		a->unpause();
	}

	MPB_LIST_LOOP( m_children, c, MPBShape* )
	{
		c->unpauseAnimations();
	}
}

void MPBShape::stopAnimations()
{
	MPB_LIST_LOOP( m_animations, a, MPBAnimation* )
	{
		a->stop();
	}

	MPB_LIST_LOOP( m_children, c, MPBShape* )
	{
		c->stopAnimations();
	}
}

void MPBShape::setLoopAnimations( bool b )
{
	MPB_LIST_LOOP( m_animations, a, MPBAnimation* )
	{
		a->setLoop( b );
	}

	MPB_LIST_LOOP( m_children, c, MPBShape* )
	{
		c->setLoopAnimations( b );
	}
}

void MPBShape::setCollidable( bool set ) 
{ 
	m_collidable = set; 

	MPB_LIST_LOOP( m_planes, p, MPBTexturedPlane* )
	{
		p->setCollidable( set );
	}
}

void MPBShape::findAABB()
{
	// find new bounding box for frustum culling

	if (m_planes.size())
	{
		MPBVector LL = ((MPBTexturedPlane*)m_planes.get( 0 ))->getLL();
		MPBVector LR;
		MPBVector UR;
		MPBVector UL;

		MPBVector NLL( LL.x, LL.y, LL.z );
		MPBVector FUR( LL.x, LL.y, LL.z );

		MPB_LIST_LOOP( m_planes, p, MPBTexturedPlane* )
		{
			LL = p->getLL();
			LR = p->getLR();
			UR = p->getUR();
			UL = p->getUL();

			NLL.x = min( 
				NLL.x, 
				min(
					min( LL.x, LR.x ),
					min( UR.x, UL.x )
				)
			);

			NLL.y = min( 
				NLL.y, 
				min(
					min( LL.y, LR.y ),
					min( UR.y, UL.y )
				)
			);

			NLL.z = max( 
				NLL.z, 
				max(
					max( LL.z, LR.z ),
					max( UR.z, UL.z )
				)
			);

			FUR.x = max( 
				FUR.x, 
				max(
					max( LL.x, LR.x ),
					max( UR.x, UL.x )
				)
			);

			FUR.y = max( 
				FUR.y, 
				max(
					max( LL.y, LR.y ),
					max( UR.y, UL.y )
				)
			);

			FUR.z = min( 
				FUR.z, 
				min(
					min( LL.z, LR.z ),
					min( UR.z, UL.z )
				)
			);
		}

		if (NLL.x == FUR.x) FUR.x += EPSILON;
		if (NLL.y == FUR.y) FUR.y += EPSILON;
		if (NLL.z == FUR.z) FUR.z -= EPSILON;

		m_aabb.setCorners( NLL, FUR );
	}
}

bool SortShapeFunc( MPBShape* a, MPBShape* b ) 
{
	MPBVector aCenter = a->getAABB().getCenter();
	MPBVector bCenter = b->getAABB().getCenter();
	MPBVector cameraCenter = MPBOpenGL::getCameraPos();

	MPBVector aToCamera = aCenter - cameraCenter;
	MPBFLOAT aToCameraDist = pow( aToCamera.x, 2 ) + pow( aToCamera.y, 2 ) + pow( aToCamera.z, 2 );

	MPBVector bToCamera = bCenter - cameraCenter;
	MPBFLOAT bToCameraDist = pow( bToCamera.x, 2 ) + pow( bToCamera.y, 2 ) + pow( bToCamera.z, 2 );

	return aToCameraDist > bToCameraDist;
}

void MPBShape::resetColor()
{
	int i;
	for (i=0; i<4; i++) m_ambientColor[i] = ambientColorDefault[i];
	for (i=0; i<4; i++) m_diffuseColor[i] = diffuseColorDefault[i];
	for (i=0; i<4; i++) m_specularColor[i] = specularColorDefault[i];
	for (i=0; i<4; i++) m_emissionColor[i] = emissionColorDefault[i];
	m_shininess[0] = shininessDefault[0];
}


/////////////////////////////////////////////////
// material properties
/////////////////////////////////////////////////

void MPBShape::setColor( GLfloat r, GLfloat g, GLfloat b, GLfloat a )
{
	MPB_LIST_LOOP( m_planes, p, MPBTexturedPlane* )
	{
		p->setColor( r, g, b, a );
	}
}

void MPBShape::setAmbientColor( GLfloat r, GLfloat g, GLfloat b, GLfloat a )
{
	m_ambientColor[0] = r;
	m_ambientColor[1] = g;
	m_ambientColor[2] = b;
	m_ambientColor[3] = a;
}

void MPBShape::setDiffuseColor( GLfloat r, GLfloat g, GLfloat b, GLfloat a )
{
	m_diffuseColor[0] = r;
	m_diffuseColor[1] = g;
	m_diffuseColor[2] = b;
	m_diffuseColor[3] = a;
}

void MPBShape::setSpecularColor( GLfloat r, GLfloat g, GLfloat b, GLfloat a )
{
	m_specularColor[0] = r;
	m_specularColor[1] = g;
	m_specularColor[2] = b;
	m_specularColor[3] = a;
}

void MPBShape::setEmissionColor( GLfloat r, GLfloat g, GLfloat b, GLfloat a )
{
	m_emissionColor[0] = r;
	m_emissionColor[1] = g;
	m_emissionColor[2] = b;
	m_emissionColor[3] = a;
}

void MPBShape::setShininess( GLfloat s )
{
	m_shininess[0] = s;
}

/////////////////////////////////////////////////
// draw transforms (reset each frame)
/////////////////////////////////////////////////

void MPBShape::setDrawMatrix( MPBMatrix& m )
{
	m.toOpenGLArray( m_drawMatrix );
}

void MPBShape::multDrawMatrix( MPBMatrix& m )
{
	MPBMatrix result;
	result.fromOpenGLArray( m_drawMatrix );
	result = result * m;
	result.toOpenGLArray( m_drawMatrix );
}

void MPBShape::resetDrawMatrix()
{
	// load identity matrix
	m_drawMatrix[0] =  1;
	m_drawMatrix[1] =  0;
	m_drawMatrix[2] =  0;
	m_drawMatrix[3] =  0;
	m_drawMatrix[4] =  0;
	m_drawMatrix[5] =  1;
	m_drawMatrix[6] =  0;
	m_drawMatrix[7] =  0;
	m_drawMatrix[8] =  0;
	m_drawMatrix[9] =  0;
	m_drawMatrix[10] = 1;
	m_drawMatrix[11] = 0;
	m_drawMatrix[12] = 0;
	m_drawMatrix[13] = 0;
	m_drawMatrix[14] = 0;
	m_drawMatrix[15] = 1;
}

void MPBShape::translateDraw( MPBFLOAT x, MPBFLOAT y, MPBFLOAT z )
{
	MPBFLOAT matrix[16] = {
		1, 0, 0, 0,
		0, 1, 0, 0,
		0, 0, 1, 0,
		x, y, z, 1
	};

	MPBFLOAT result[16];

	matrixMultiply( matrix, m_drawMatrix, result );
	matrixCopy( result, m_drawMatrix );
}

void MPBShape::rotateXDraw( MPBFLOAT degrees )
{
	MPBFLOAT sine = sin( degrees * PI_OVER_180 );
	MPBFLOAT cosine = cos( degrees * PI_OVER_180 );

	MPBFLOAT matrix[16] = {
		1, 0, 0, 0,
		0, cosine, sine, 0,
		0, -sine, cosine, 0,
		0, 0, 0, 1
	};

	MPBFLOAT result[16];

	matrixMultiply( matrix, m_drawMatrix, result );
	matrixCopy( result, m_drawMatrix );
}

void MPBShape::rotateYDraw( MPBFLOAT degrees )
{
	MPBFLOAT sine = sin( degrees * PI_OVER_180 );
	MPBFLOAT cosine = cos( degrees * PI_OVER_180 );

	MPBFLOAT matrix[16] = {
		cosine, 0, -sine,  0,
		0,      1, 0,      0,
		sine,   0, cosine, 0,
		0,      0, 0,      1
	};

	MPBFLOAT result[16];

	matrixMultiply( matrix, m_drawMatrix, result );
	matrixCopy( result, m_drawMatrix );
}

void MPBShape::rotateZDraw( MPBFLOAT degrees )
{
	MPBFLOAT sine = sin( degrees * PI_OVER_180 );
	MPBFLOAT cosine = cos( degrees * PI_OVER_180 );

	MPBFLOAT matrix[16] = {
		cosine, -sine,  0, 0,
		sine,   cosine, 0, 0,
		0,      0,      1, 0,
		0,      0,      0, 1
	};

	MPBFLOAT result[16];

	matrixMultiply( matrix, m_drawMatrix, result );
	matrixCopy( result, m_drawMatrix );
}

void MPBShape::scaleDraw( MPBFLOAT x, MPBFLOAT y, MPBFLOAT z )
{
	MPBFLOAT matrix[16] = {
		x, 0, 0, 0,
		0, y, 0, 0,
		0, 0, z, 0,
		0, 0, 0, 1
	};
	MPBFLOAT result[16];

	matrixMultiply( matrix, m_drawMatrix, result );
	matrixCopy( result, m_drawMatrix );
}

/////////////////////////////////////////////////
// animation transforms (reset each frame)
/////////////////////////////////////////////////

void MPBShape::resetAnimationMatrix()
{
	// load identity matrix
	m_animationMatrix[0] =  1;
	m_animationMatrix[1] =  0;
	m_animationMatrix[2] =  0;
	m_animationMatrix[3] =  0;
	m_animationMatrix[4] =  0;
	m_animationMatrix[5] =  1;
	m_animationMatrix[6] =  0;
	m_animationMatrix[7] =  0;
	m_animationMatrix[8] =  0;
	m_animationMatrix[9] =  0;
	m_animationMatrix[10] = 1;
	m_animationMatrix[11] = 0;
	m_animationMatrix[12] = 0;
	m_animationMatrix[13] = 0;
	m_animationMatrix[14] = 0;
	m_animationMatrix[15] = 1;
}

void MPBShape::translateAnimation( MPBFLOAT x, MPBFLOAT y, MPBFLOAT z )
{
	MPBFLOAT matrix[16] = {
		1, 0, 0, 0,
		0, 1, 0, 0,
		0, 0, 1, 0,
		x, y, z, 1
	};

	MPBFLOAT result[16];

	matrixMultiply( matrix, m_animationMatrix, result );
	matrixCopy( result, m_animationMatrix );
}

void MPBShape::rotateXAnimation( MPBFLOAT degrees )
{
	MPBFLOAT sine = sin( degrees * PI_OVER_180 );
	MPBFLOAT cosine = cos( degrees * PI_OVER_180 );

	MPBFLOAT matrix[16] = {
		1, 0, 0, 0,
		0, cosine, sine, 0,
		0, -sine, cosine, 0,
		0, 0, 0, 1
	};

	MPBFLOAT result[16];

	matrixMultiply( matrix, m_animationMatrix, result );
	matrixCopy( result, m_animationMatrix );
}

void MPBShape::rotateYAnimation( MPBFLOAT degrees )
{
	MPBFLOAT sine = sin( degrees * PI_OVER_180 );
	MPBFLOAT cosine = cos( degrees * PI_OVER_180 );

	MPBFLOAT matrix[16] = {
		cosine, 0, -sine,  0,
		0,      1, 0,      0,
		sine,   0, cosine, 0,
		0,      0, 0,      1
	};

	MPBFLOAT result[16];

//	translateAnimation( 0, 0, .286 );
	matrixMultiply( matrix, m_animationMatrix, result );
	matrixCopy( result, m_animationMatrix );
//	translateAnimation( 0, 0, -.286 );
}

void MPBShape::rotateZAnimation( MPBFLOAT degrees )
{
	MPBFLOAT sine = sin( degrees * PI_OVER_180 );
	MPBFLOAT cosine = cos( degrees * PI_OVER_180 );

	MPBFLOAT matrix[16] = {
		cosine, -sine,  0, 0,
		sine,   cosine, 0, 0,
		0,      0,      1, 0,
		0,      0,      0, 1
	};

	MPBFLOAT result[16];

	matrixMultiply( matrix, m_animationMatrix, result );
	matrixCopy( result, m_animationMatrix );
}

void MPBShape::scaleAnimation( MPBFLOAT x, MPBFLOAT y, MPBFLOAT z )
{
	MPBFLOAT matrix[16] = {
		x, 0, 0, 0,
		0, y, 0, 0,
		0, 0, z, 0,
		0, 0, 0, 1
	};
	MPBFLOAT result[16];

	matrixMultiply( matrix, m_animationMatrix, result );
	matrixCopy( result, m_animationMatrix );
}

/////////////////////////////////////////////////
// texture animation transforms
/////////////////////////////////////////////////

void MPBShape::setTexCoords(	int texture,
										const MPBVector& LL,
										const MPBVector& LR,
										const MPBVector& UR,
										const MPBVector& UL	)
{
	MPB_LIST_LOOP( m_planes, p, MPBTexturedPlane* )
	{
		p->setTexCoords( texture, LL, LR, UR, UL );
	}
}

void MPBShape::setTexSize( int texture, MPBFLOAT x, MPBFLOAT y )
{
	MPB_LIST_LOOP( m_planes, plane, MPBTexturedPlane* )
	{
		plane->setTexCoords( 
			texture, 
			MPBVector( 0, 0, 0 ),
			MPBVector( plane->getWidth() / x, 0, 0 ),
			MPBVector( plane->getWidth() / x, plane->getHeight() / y, 0 ),
			MPBVector( 0, plane->getHeight() / y, 0 )
		);
	}
}


/////////////////////////////////////////////////
// drawing functions
/////////////////////////////////////////////////

bool MPBShape::isInViewFrustum()
{
	return MPBOpenGL::aabbInFrustum( m_aabb );
//	return true;
}

void MPBShape::drawList( int lod, bool useTexture, bool useColor )
{
	if (lod >= 1)
	{
//		return;
	}

	MPB_LIST_LOOP( m_planes, plane, MPBTexturedPlane* )
	{
		plane->draw( useTexture, useColor );
	}

	MPB_LIST_LOOP( m_lines, l, MPBLine* )
	{
		l->draw();
	}
}

void MPBShape::compile()
{
	if (!m_compile) return;
/*
	m_displayList = glGenLists( 1 );
	glNewList( m_displayList, GL_COMPILE );
	drawList( 0 );
	glEndList();
*/
	for (int lod=0; lod<LODS; lod++)
	{
		m_displayList[lod] = glGenLists( 1 );
		glNewList( m_displayList[lod], GL_COMPILE );
		drawList( lod, true, true );
		glEndList();
	}

	m_compiled = true;

	MPB_LIST_LOOP( m_children, c, MPBShape* )
	{
		c->compile();
	}
}

void MPBShape::draw( bool useTexture, bool shadowersOnly, bool useColor )
{
	if (m_drawn) return;
	m_drawn = true;

	if (!m_draw && !shadowersOnly) return;
	if (shadowersOnly && !m_castsShadow) return;

	if (m_frustumCull && !isInViewFrustum()) return;

	glPushAttrib( GL_FOG_BIT );
	if (m_disableFog) glDisable( GL_FOG );

	GLboolean lightingEnabled = true;
	if (!m_isLit)
	{
		glGetBooleanv( GL_LIGHTING, &lightingEnabled );
		glDisable( GL_LIGHTING );
	}

	glPushMatrix();

	if (m_backfaceCull) glEnable( GL_CULL_FACE );
	else glDisable( GL_CULL_FACE );

	// set material properties for OpenGL lighting

	glMaterialfv( GL_FRONT, GL_AMBIENT, m_ambientColor );
	glMaterialfv( GL_FRONT, GL_DIFFUSE, m_diffuseColor );
	glMaterialfv( GL_FRONT, GL_SPECULAR, m_specularColor );
	glMaterialfv( GL_FRONT, GL_EMISSION, m_emissionColor );
	glMaterialfv( GL_FRONT, GL_SHININESS, m_shininess );

	int lod = 0;

	if (0)//(g_optionsManager->getOptionState( ID_OPT_ENABLE_LOD ))
	{
		int distanceToPlayer = 
			(MPBOpenGL::getCameraPos() - m_aabb.getCenter()).getLength();

		const int detailThreshold = 64;

		lod = LODS - 1;
		for (int i=0; i<LODS; i++)
		{
			if (distanceToPlayer < detailThreshold * pow( 2, i )) 
			{
				lod = i;
				break;
			}
		}
	}


	glMultMatrixf( m_animationMatrix );
	glMultMatrixf( m_drawMatrix );

	if (m_compiled)
		glCallList( m_displayList[lod] );
/*	else if (m_billboard)
	{
		MPBOpenGL::bindTexture( "metal3" );

		glTexCoord2d( 0, 0 ); 
		glVertex3d(	m_aabb.getCenter().x + m_billboardOffset1.x, 
						m_aabb.getCenter().y + m_billboardOffset1.y, 
						m_aabb.getCenter().z + m_billboardOffset1.z	);

		glTexCoord2d( 1, 0 ); 
		glVertex3d(	m_aabb.getCenter().x + m_billboardOffset2.x, 
						m_aabb.getCenter().y + m_billboardOffset2.y, 
						m_aabb.getCenter().z + m_billboardOffset2.z	);

		glTexCoord2d( 1, 1 ); 
		glVertex3d(	m_aabb.getCenter().x + m_billboardOffset3.x, 
						m_aabb.getCenter().y + m_billboardOffset3.y, 
						m_aabb.getCenter().z + m_billboardOffset3.z	);

		glTexCoord2d( 0, 1 ); 
		glVertex3d(	m_aabb.getCenter().x + m_billboardOffset4.x, 
						m_aabb.getCenter().y + m_billboardOffset4.y, 
						m_aabb.getCenter().z + m_billboardOffset4.z	);
	}
*/	else
		drawList( lod, useTexture, useColor );

	MPBPerformanceMonitor::m_shapesDrawn++;

	MPB_LIST_LOOP( m_children, c, MPBShape* )
	{
		c->draw( useTexture, shadowersOnly, useColor );
	}

	if (!m_isLit && lightingEnabled)
	{
		glEnable( GL_LIGHTING );
	}

	glPopMatrix();
	glPopAttrib();
}

void MPBShape::drawShadowVolume()
{
	if (!m_castsShadow) return;
	if (!m_draw) return;

	glPushMatrix();

	glMultMatrixf( m_animationMatrix );
	glMultMatrixf( m_drawMatrix );

	if (USE_LIGHT_VOLUMES)
	{
		glBegin( GL_TRIANGLES );
		MPB_LIST_LOOP( m_planes, p, MPBTexturedPlane* )
		{
			p->drawShadowVolume();
		}
		glEnd();
	}
	else
	{
		glBegin( GL_QUADS );
		MPB_LIST_LOOP( m_planes, p, MPBTexturedPlane* )
		{
			p->drawShadowVolume();
		}
		glEnd();
	}

	MPB_LIST_LOOP( m_children, c, MPBShape* )
	{
		c->drawShadowVolume();
	}

	glPopMatrix();
}

// lightPos should be absolute
// mpb! should only draw shape's silhouette
void MPBShape::createShadowVolume( const MPBAABB* cellAABB, const MPBVector& lightPos )
{
	if (!m_castsShadow) return;
	if (!m_draw) return;

	MPB_LIST_LOOP( m_planes, p, MPBTexturedPlane* )
	{
		p->createShadowVolume( cellAABB, lightPos );
	}

	MPB_LIST_LOOP( m_children, c, MPBShape* )
	{
		c->createShadowVolume( cellAABB, lightPos );
	}
}


/////////////////////////////////////////////////
// don't call these after the planes and lines have been added
// to the level.
/////////////////////////////////////////////////

void MPBShape::translate( MPBFLOAT x, MPBFLOAT y, MPBFLOAT z )
{
	if (m_geomID)
	{
		dGeomSetPosition( m_geomID, x, y, z );
	}

	MPB_LIST_LOOP( m_planes, p, MPBTexturedPlane* )
	{
		p->translate( x, y, z );
	}

//	MPB_LIST_LOOP_CONST( m_lines, MPBLine* )
//	{
//		(*itr)->translate( x, y, z );
//	}

	findAABB();
}

void MPBShape::setRotation( MPBFLOAT degreesX, MPBFLOAT degreesY, MPBFLOAT degreesZ )
{
	if (m_geomID)
	{
		MPBQuaternion q;
		q.fromEuler( degreesToRadians( degreesX ), degreesToRadians( degreesY ), degreesToRadians( degreesZ ) );

		dQuaternion dq;
		dq[0] = q.n;
		dq[1] = q.v.x;
		dq[2] = q.v.y;
		dq[3] = q.v.z;
		dGeomSetQuaternion( m_geomID, dq );
	}

	MPB_LIST_LOOP( m_planes, p, MPBTexturedPlane* )
	{
		p->rotateX( degreesX );
		p->rotateY( degreesY );
		p->rotateZ( degreesZ );
	}
}

void MPBShape::rotateX( MPBFLOAT degrees )
{
	if (m_geomID) MPB_ASSERT( 0 );

	MPB_LIST_LOOP( m_planes, p, MPBTexturedPlane* )
	{
		p->rotateX( degrees );
	}

	findAABB();
}

void MPBShape::rotateY( MPBFLOAT degrees )
{
	if (m_geomID) MPB_ASSERT( 0 );

	MPB_LIST_LOOP( m_planes, p, MPBTexturedPlane* )
	{
		p->rotateY( degrees );
	}

	findAABB();
}

void MPBShape::rotateZ( MPBFLOAT degrees )
{
	if (m_geomID) MPB_ASSERT( 0 );

	MPB_LIST_LOOP( m_planes, p, MPBTexturedPlane* )
	{
		p->rotateZ( degrees );
	}

	findAABB();
}

void MPBShape::scale( MPBFLOAT x, MPBFLOAT y, MPBFLOAT z )
{
	if (m_geomID) MPB_ASSERT( 0 );

	MPB_LIST_LOOP( m_planes, p, MPBTexturedPlane* )
	{
		p->scale( x, y, z );
	}

	findAABB();
}

void MPBShape::rotateToVector( MPBVector v )
{
	MPBQuaternion q;
	q.fromVector( v.normalized() );
	MPBMatrix m;
	q.toMatrix( m );

	MPB_LIST_LOOP( m_planes, p, MPBTexturedPlane* )
	{
		MPBMatrix id;
		p->rotateMatrix( m );
	}

	findAABB();
}

/////////////////////////////////////////////////
// 
/////////////////////////////////////////////////

Plane* MPBShape::getPlane( int i )
{
	return (Plane*)m_planes.get( i );
}

MPBList& MPBShape::getPlanes()
{
	return m_planes;
}

void MPBShape::getPlanes( MPBList& planes )
{
	MPB_LIST_LOOP( m_planes, p, MPBTexturedPlane* )
	{
		MPB_ASSERT( p );
		planes.push_back( p );
	}
}

// penetration point is returned in point parameter
// mpb! shapes must be concave for this to work
bool MPBShape::getPenetrationPoint( MPBVector start, MPBVector& point )
{
	MPBFLOAT minDistance = 1000000;
	Plane* penetrationPlane = NULL;

	MPB_LIST_LOOP( m_planes, plane, MPBTexturedPlane* )
	{
		MPBFLOAT distance = plane->distanceToPoint( point );
		if (distance > 0) return false;	// point is oustide plane

		if (-distance < minDistance) 
		{
			minDistance = -distance;
			penetrationPlane = plane;
		}
	}

	MPB_ASSERT( penetrationPlane );

	MPBVector ip;
//	penetrationPlane->lineIntersect(	point, 
//												point + penetrationPlane->getNormal(), 
//												ip	);

	if (!penetrationPlane->lineInsideCorners(	point, 
															point + penetrationPlane->getNormal(), 
															ip	))
		return false;

	point = ip;
	return true;
}

void MPBShape::savePOVFile( FILE* fp )
{
	char buf[1000];
	char buf2[1000];

	MPB_LIST_LOOP( m_planes, plane, MPBTexturedPlane* )
	{
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
		fputs( "}\n", fp );
		fputs( "\n", fp );
	}
}

