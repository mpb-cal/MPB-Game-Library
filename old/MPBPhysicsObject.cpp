
#include "MPBPhysics.h"
#include "MPBPhysicsParticle.h"
#include "MPBGlobal.h"
#include "MPBFrameCounter.h"
#include "MPBCollision.h"
#include "MPBGameEntities.h"
#include "MPBMessageWindow.h"
#include "MPBPlane.h"
#include "MPBOpenGL.h"
#include "MPBSoundManager.h"
#include "MPBShadow.h"
#include "MPBParticleSystem.h"
#include "MPBShape.h"
#include "MPBHardSpring.h"
#include "MPBHeightMap.h"
#include "MPBRigidBody.h"
#include "MPBProfile.h"
#include "MPBNavigationMesh.h"
#include "MPBShapeExtrusion.h"
#include "MPBShapeTube.h"
#include <ode/ode.h>
#include "mmgr.h"


const MPBFLOAT DRAW_SCALE = 1.0;
const bool USE_ODE = true;


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


/*
static const GLfloat ambientColorDefault[4] = { 1, 1, 1, 1 };
static const GLfloat diffuseColorDefault[4] = { 1, 1, 1, 1 };
static const GLfloat specularColorDefault[4] = { 1, 1, 1, 1 };
static const GLfloat emissionColorDefault[4] = { 1, 1, 1, 1 };
static const GLfloat shininessDefault[1] = { 1 };
*/

static const GLfloat ambientColorDefault[4] = { Chrome[0], Chrome[1], Chrome[2], Chrome[3] };
static const GLfloat diffuseColorDefault[4] = { Chrome[4], Chrome[5], Chrome[6], Chrome[7] };
static const GLfloat specularColorDefault[4] = { Chrome[8], Chrome[9], Chrome[10], Chrome[11] };
static const GLfloat shininessDefault[1] = { Chrome[12] };

static const GLfloat emissionColorDefault[4] = { 0, 0, 0, 1 };

MPBPhysicsObject::MPBPhysicsObject( PhysicsObjectType t ) :
	MPB3DObject(),
	m_poType( t ),
	m_alive( true ),
	m_collisionTest( true ),
	m_planeCollisionTest( true ),
	m_collidable( true ),
	m_elasticity( 0.01f ),
	m_draw( true ),
	m_freeRotate( false ),
	m_constraints( 0 ),
	m_owner( NULL ),
	m_ownerRB( NULL ),
	m_ownerPP( NULL ),
	m_lateralFriction( false ),
	m_isParticleSystem( false ),
	m_castsShadow( true ),
	m_damage( 0 ),
	m_airDensity( AIR_DENSITY ),
	m_immovable( false ),
	m_rollingResistance( -13.0f ),
	m_geomID( 0 ),
	m_shadowVolumeShape( NULL )
{
	// ODE stuff

	if (m_poType == POT_SPHERE)
		m_geomID = dCreateSphere( 0, 1 );
	else if (m_poType == POT_BOX)
		m_geomID = dCreateBox( 0, 1, 1, 1 );

	dGeomSetBody( m_geomID, m_bodyID );



	setRadius( 1 );
	updateOrientationVector();
	if (MPBOpenGL::is2D())
		strcpy( m_texture, "circle" );
	else
		strcpy( m_texture, "metal3" );
	m_gravity = MPBPhysics::gravity;
}

MPBPhysicsObject::~MPBPhysicsObject()
{
	MPB_LIST_DELETE( m_shapes, MPBShape* );
}

void MPBPhysicsObject::setRadius( MPBFLOAT s ) 
{
	if (m_poType == POT_SPHERE)
		dGeomSphereSetRadius( m_geomID, (dReal)s );
	else if (m_poType == POT_BOX)
		dGeomBoxSetLengths( m_geomID, (dReal)s * 2.0, (dReal)s * 2.0, (dReal)s * 2.0 );


	setRadiusVector( MPBVector( s, s, s ) );

	m_aabb.setCenterAndExtents( MPBVector(), MPBVector( s, s, s ) );

	setupAABBPlanes();
}

void MPBPhysicsObject::setRadiusVector( MPBVector v ) 
{
	if (m_poType == POT_AABB)
		m_radiusVector = v;
	else
		m_radiusVector = MPBVector( v.y, v.y, v.y );

	m_radius = m_radiusVector.y;
	setMass( m_radius * 100 );

	m_aabb.setCenterAndExtents( MPBVector(), m_radiusVector );

	setupAABBPlanes();
}

void MPBPhysicsObject::setMass( MPBFLOAT s ) 
{ 
	MPB3DObject::setMass( s );
	m_inertia = 2.0f/5.0f * m_mass * (m_radius * m_radius);

	dMass mass;
	dMassSetSphereTotal( &mass, (dReal)s, (dReal)m_radius );
	dBodySetMass( m_bodyID, &mass );
}

void MPBPhysicsObject::setPosition( MPBVector pos ) 
{
	MPB3DObject::setPosition( pos );

	m_aabb.setCenter( pos );
}

void MPBPhysicsObject::setOwner( MPBPhysicsObject* owner, MPBVector ownerOffset ) 
{ 
	m_owner = owner; 
	m_ownerOffset = ownerOffset;
	setCollidable( false );
	setCollisionTest( false );
	setPlaneCollisionTest( false );
}

void MPBPhysicsObject::setOwner( MPBPhysicsParticle* owner, MPBVector ownerOffset ) 
{ 
	m_ownerPP = owner; 
	m_ownerOffset = ownerOffset;
	setCollidable( false );
	setCollisionTest( false );
	setPlaneCollisionTest( false );
}

void MPBPhysicsObject::setOwner( MPBRigidBody* owner, MPBVector ownerOffset ) 
{ 
	m_ownerRB = owner; 
	m_ownerOffset = ownerOffset;
	setCollidable( false );
	setCollisionTest( false );
	setPlaneCollisionTest( false );
}

void MPBPhysicsObject::addDamage( MPBFLOAT d )
{
	m_damage += d;
}

void MPBPhysicsObject::setRotationMatrix( MPBMatrix& m ) 
{ 
	setOrientationQuaternion( m.getQuaternion() );
}

// call when m_orientationQuaternion has changed
void MPBPhysicsObject::updateOrientationVector()
{
	m_orientationVector.set( 0, 0, -1 );
	m_orientationQuaternion.rotateVector( m_orientationVector );
}

// in x,y,z radians
void MPBPhysicsObject::rotate( const MPBVector& rotation )
{
	MPB_ASSERT( m_freeRotate );

	if (rotation.x || rotation.y || rotation.z)
	{
		m_eulerRotation = m_eulerRotation + rotation;
		m_orientationQuaternion.fromEuler( m_eulerRotation.x, m_eulerRotation.y, m_eulerRotation.z );
		m_orientationQuaternion.normalize();

		updateOrientationVector();
	}
}

// in radians
void MPBPhysicsObject::rotateToX( MPBFLOAT radians )
{
	MPB_ASSERT( m_freeRotate );

	m_eulerRotation.x = radians;
	m_orientationQuaternion.fromEuler( m_eulerRotation.x, m_eulerRotation.y, m_eulerRotation.z );
	m_orientationQuaternion.normalize();

	updateOrientationVector();
}

// in radians
void MPBPhysicsObject::rotateToY( MPBFLOAT radians )
{
	MPB_ASSERT( m_freeRotate );

	m_eulerRotation.y = radians;
	m_orientationQuaternion.fromEuler( m_eulerRotation.x, m_eulerRotation.y, m_eulerRotation.z );
	m_orientationQuaternion.normalize();

	updateOrientationVector();
}

// in radians
void MPBPhysicsObject::rotateToZ( MPBFLOAT radians )
{
	MPB_ASSERT( m_freeRotate );

	m_eulerRotation.z = radians;
	m_orientationQuaternion.fromEuler( m_eulerRotation.x, m_eulerRotation.y, m_eulerRotation.z );
	m_orientationQuaternion.normalize();

	updateOrientationVector();
}

MPBVector MPBPhysicsObject::getAxisVector( DirectionType direction ) const
{
	// take a unit vector pointing down the z-axis
	// and rotate it appropriately.
	MPBVector axisVector( 0, 0, -1 );

	switch (direction)
	{
		case DIR_FORWARDS:	break;
		case DIR_BACKWARDS:	axisVector.rotateY( 180 ); break;
		case DIR_LEFT:			axisVector.rotateY( 90); break;
		case DIR_RIGHT:		axisVector.rotateY( -90 ); break;
		case DIR_UP:			axisVector.rotateX( 90 ); break;
		case DIR_DOWN:			axisVector.rotateX( -90 ); break;
		MPB_ASSERT( 0 );
	}

	m_orientationQuaternion.rotateVector( axisVector );

	return axisVector;
}

void MPBPhysicsObject::move( const MPBVector& moveVector, bool testCollisions )
{
	if (testCollisions)
	{
		MPBVector prevVelocity = m_velocity;
		m_velocity = moveVector;
		collisionTest();
		m_velocity = prevVelocity;
	}
	else
	{
		m_position = m_position + moveVector * MPBFrameCounter::secondsElapsed;
	}
}
/*
void MPBPhysicsObject::draw2D( HDC& hdc )
{
	SelectObject( hdc, GetStockObject( BLACK_PEN ) );
	SelectObject( hdc, CreateSolidBrush( m_colorRef ) );

	Ellipse(	hdc,
				m_position.x * DRAW_SCALE - m_radius * DRAW_SCALE,
				m_position.y * DRAW_SCALE - m_radius * DRAW_SCALE,
				m_position.x * DRAW_SCALE + m_radius * DRAW_SCALE,
				m_position.y * DRAW_SCALE + m_radius * DRAW_SCALE	);

	MPBVector v = m_orientationVector;
	v = v * m_radius * DRAW_SCALE;
	v.draw2D( &hdc, m_position.x * DRAW_SCALE, m_position.y * DRAW_SCALE );
}
*/

void MPBPhysicsObject::draw( 
	bool shadow, 
	Plane* shadowReceiver, 
	MPBVector vectorToSun,
	bool forShadowVolume ) 
{
	if (!m_draw && !shadow && !forShadowVolume) return;

	if (!shadow && !forShadowVolume && !MPBOpenGL::sphereInFrustum( m_position, m_radius )) return;

	glPushMatrix();

	if (shadow)
	{
		if (!m_castsShadow) 
		{
			glPopMatrix();
			return;
		}

		MPBFLOAT shadowMatrix[16];
		MPBShadow::projectionShadowMatrix(	shadowMatrix, 
														vectorToSun, 
														shadowReceiver );

		glMultMatrixd( shadowMatrix );
	}
	else if (forShadowVolume)
	{
	}
	else
	{
		MPBOpenGL::bindTexture( m_texture );
	}

	// nav node debugging
	if (0)//(!shadow && !forShadowVolume)
	{
		MPBNavigationNode* node = MPBOpenGL::getNavMesh()->getNode( m_position.x, m_position.z );
		if (node) node->draw( m_position.y );
	}

	if (m_poType == POT_BODY)
	{
		glColor4ub( 255, 0, 0, 255 );
		glLineWidth( 5 );

		glBegin( GL_LINE_STRIP );

		for (int i=0; i<m_vertices.size(); i++)
		{
			m_vertices[i].drawAsVertex();
		}

		glEnd();
	}

	// translation & rotation
	if (m_poType != POT_AABB || m_shapes.size())
	{
		glTranslated( m_position.x, m_position.y, m_position.z );

		MPBFLOAT matrix[16];

		if (USE_ODE)
		{
			const dReal *bq = dBodyGetQuaternion( m_bodyID );
			MPBQuaternion q( bq[0], bq[1], bq[2], bq[3] );
			q.toMatrix( matrix );
		}
		else
		{
			m_orientationQuaternion.toMatrix( matrix );
		}

		glMultMatrixd( matrix );
	}

	// color
	if (!shadow && !forShadowVolume) 
	{
		glColor4ub( m_red, m_green, m_blue, 255 );

		// set material properties for OpenGL lighting

		glMaterialfv( GL_FRONT, GL_AMBIENT, ambientColorDefault );
		glMaterialfv( GL_FRONT, GL_DIFFUSE, diffuseColorDefault );
		glMaterialfv( GL_FRONT, GL_SPECULAR, specularColorDefault );
		glMaterialfv( GL_FRONT, GL_EMISSION, emissionColorDefault );
		glMaterialfv( GL_FRONT, GL_SHININESS, shininessDefault );
	}

	// render

	if (m_shapes.size())
	{
		if (shadow)
		{
			MPB_LIST_LOOP( m_shapes, shape, MPBShape* )
			{
				shape->setDrawn( false );
				shape->draw( false, false, false );
			}
		}
		else
		{
			MPB_LIST_LOOP( m_shapes, shape, MPBShape* )
			{
				shape->setDrawn( false );
				shape->draw( !forShadowVolume, forShadowVolume, false );
			}
		}
	}
	else
	{
		if (m_poType == POT_AABB)
		{
			m_aabb.draw();
		}
		else if (m_poType == POT_SPHERE)
		{
			glScaled( m_radiusVector.x, m_radiusVector.y, m_radiusVector.z );
			gluSphere( MPBOpenGL::quadricObj, 1, 20, 20 );
		}
		else if (m_poType == POT_BOX)
		{
			m_aabb.setCenter( MPBVector() );
			m_aabb.draw();
		}
	}

	glPopMatrix();
}

void MPBPhysicsObject::createShadowVolume( MPBVector& lightPos )
{
	if (!m_castsShadow) return;

	const MPBFLOAT edgeLength = 100;

	if (0)
	{
		if (m_shapes.size())
		{
			glPushMatrix();

//			glTranslated( m_position.x, m_position.y, m_position.z );
			MPBFLOAT matrix[16];
			m_orientationQuaternion.toMatrix( matrix );
//			glMultMatrixd( matrix );

			MPB_LIST_LOOP( m_shapes, shape, MPBShape* )
			{
				shape->createShadowVolume( NULL, lightPos );
			}

			glPopMatrix();
		}
	}
	else if (0)
	{
		/*
		for each plane in object
			continue if the plane does not face the light
			for each edge (quads have 4)
				continue unless edge has a neighboring plane which faces the light
				extend edge along light vector as shadow volume face
		*/
	}
	else if (m_poType == POT_SPHERE)
	{
		delete m_shadowVolumeShape;

		m_shadowVolumeShape = new MPBShapeTube( m_radius * 1.1, m_radius * 1.1, edgeLength, 20, true, "" );
		m_shadowVolumeShape->setFrustumCull( false );
	}
}

struct Edge
{
	MPBVector p1;
	MPBVector p2;
};

void MPBPhysicsObject::drawShadowVolume( const MPBVector& lightPos )
{
	if (!m_castsShadow) return;

	const MPBFLOAT edgeLength = 100;

	if (0)
	{
		if (m_shapes.size())
		{
//			glPushMatrix();

//			glTranslated( m_position.x, m_position.y, m_position.z );
//			MPBFLOAT matrix[16];
//			m_orientationQuaternion.toMatrix( matrix );
//			glMultMatrixd( matrix );

			MPB_LIST_LOOP( m_shapes, shape, MPBShape* )
			{
				shape->createShadowVolume( NULL, lightPos );
				shape->drawShadowVolume();
			}

//			glPopMatrix();
		}
	}
	else if (m_poType == POT_AABB)
	{
		MPBVector vectorToLight = lightPos - m_aabb.getCenter();
		vectorToLight.normalize();

		MPBVector extrusion = vectorToLight * -1;
		extrusion.setLength( edgeLength );

		glPushMatrix();
		glTranslated( getPosition().x, getPosition().y, getPosition().z );

		for (int i=0; i<6; i++)
		{
			if (vectorToLight.dotProduct( m_aabbPlanes[i].getNormal() ) > 0)		// faces light
			{
				MPBShapeExtrusion se( &m_aabbPlanes[i], extrusion, "", 0, 0 );
				se.setFrustumCull( false );
				se.getPlane( 0 )->setPolygonOffset( 2.0, 2.0 );
				se.draw( false, false, false );
			}
		}

		glPopMatrix();


		// 12 edges:
		// 0		front left
		// 1		front bottom
		// 2		front right
		// 3		front top
		// 4		lower left
		// 5		lower right
		// 6		upper right
		// 7		upper left
		// 8		back left
		// 9		back bottom
		// 10		back right
		// 11		back top

		Edge edges[12];

	}
	else if (m_poType == POT_SPHERE)
	{
		glPushMatrix();
		glTranslated( m_position.x, m_position.y, m_position.z );

//		glScaled( m_radiusVector.x * 3, m_radiusVector.y * 3, m_radiusVector.z * 3 );
//		gluSphere( MPBOpenGL::quadricObj, 1, 10, 10 );

//		MPBShapeExtrusion se( &m_aabbPlanes[i], extrusion, "", 0, 0 );

		if (m_shadowVolumeShape)
		{
			MPBVector lightToMe = m_position - lightPos;
			MPBQuaternion q;
			q.fromVector( lightToMe.normalized() );
			MPBMatrix m;
			q.toMatrix( m );

			m_shadowVolumeShape->setDrawMatrix( m );
	//		m_shadowVolumeShape->rotateToVector( lightToMe );

			m_shadowVolumeShape->setDrawn( false );
			m_shadowVolumeShape->draw( false, false, false );
		}

		glPopMatrix();
	}
	else MPB_ASSERT( 0 );
}

void MPBPhysicsObject::setupAABBPlanes()
{
	// set up planes for shadow volume

	MPBAABB aabb = m_aabb;
	MPBVector normal;
	aabb.setCenter( MPBVector() );

	m_aabbPlanes[0].setCorners( aabb.getNLL(), aabb.getNLR(), aabb.getNUR(), aabb.getNUL() );	// front
	m_aabbPlanes[1].setCorners( aabb.getFLR(), aabb.getFLL(), aabb.getFUL(), aabb.getFUR() );	// back
	m_aabbPlanes[2].setCorners( aabb.getNUL(), aabb.getNUR(), aabb.getFUR(), aabb.getFUL() );	// top
	m_aabbPlanes[3].setCorners( aabb.getFLL(), aabb.getFLR(), aabb.getNLR(), aabb.getNLL() );	// bottom
	m_aabbPlanes[4].setCorners( aabb.getFLL(), aabb.getNLL(), aabb.getNUL(), aabb.getFUL() );	// left
	m_aabbPlanes[5].setCorners( aabb.getNLR(), aabb.getFLR(), aabb.getFUR(), aabb.getNUR() );	// right
}

// apply a torque
// pointOfContact is in units, relative to center of mass
// magnitude of force should be inertia * (radians per second squared)
void MPBPhysicsObject::applyTorque( const MPBVector& force, const MPBVector& pointOfContact )
{
	if (USE_ODE) { MPB_ASSERT( 0 ); }//dBodyAddTorque( m_bodyID, force.x, force.y, force.z );
	else
		m_angularAcceleration = 
			m_angularAcceleration + (pointOfContact.crossProduct( force * MPBFrameCounter::secondsElapsed ) / m_inertia);
}

void MPBPhysicsObject::applyTorqueImpulse( const MPBVector& force, const MPBVector& pointOfContact )
{
	if (USE_ODE)
	{
		MPB_ASSERT( 0 );
		dVector3 f;
		dWorldImpulseToForce( MPBGameEntities::worldID, MPBFrameCounter::secondsElapsed, force.x, force.y, force.z, f );
		dBodyAddTorque( m_bodyID, f[0], f[1], f[2] );
	}
	else
		m_angularAcceleration = 
			m_angularAcceleration + (pointOfContact.crossProduct( force ) / m_inertia);
}

void MPBPhysicsObject::turnToFaceNowX( MPBVector target )
{
	MPBVector desiredOrientation( target - getPosition() );
	desiredOrientation.normalize();
	rotateToX( desiredOrientation.getXRotation() );
}

void MPBPhysicsObject::turnToFaceNowY( MPBVector target )
{
	MPBVector desiredOrientation( target - getPosition() );
	desiredOrientation.normalize();
	rotateToY( desiredOrientation.getYRotation() );
}

bool MPBPhysicsObject::odeCollide( 
	dGeomID obstacleGeomID, 
	dBodyID obstacleBodyID, 
	MPBFLOAT obstacleElasticity,
	MPBPhysicsObject* obstaclePO,		// can be NULL
	MPBShape* obstacleShape				// can be NULL
)
{
	dContactGeom contactGeoms[1];

	int c = dCollide( m_geomID, obstacleGeomID, 0x01, contactGeoms, sizeof( dContactGeom ) );

	if (c)
	{
		m_collided = true;

		dContact contact;
		memset( &contact, 0, sizeof( dContact ) );
		contact.surface.mode = dContactBounce;
		contact.surface.mu = 1;//dInfinity;
		contact.surface.bounce = m_elasticity * obstacleElasticity;
		for (int i=0; i<4; i++) contact.geom.pos[i] = contactGeoms[0].pos[i];
		for (i=0; i<4; i++) contact.geom.normal[i] = contactGeoms[0].normal[i];
		contact.geom.depth = contactGeoms[0].depth;
		contact.geom.g1 = contactGeoms[0].g1;
		contact.geom.g2 = contactGeoms[0].g2;

		dJointID jID = dJointCreateContact( MPBGameEntities::worldID, MPBGameEntities::contactJointGroup, &contact );
		dJointAttach( jID, getBodyID(), obstacleBodyID );
		dBodyID b = dJointGetBody( jID, 0 );

		Collision c( 0, MPBVector(), MPBVector() );
		c.m_targetObject = obstaclePO;
		c.m_targetPlane = NULL;
		c.m_targetShape = obstacleShape;
		handleCollision1( &c );

		return true;
	}

	return false;
}

void MPBPhysicsObject::updateFrame()
{
	MPB_LIST_LOOP( m_shapes, shape, MPBShape* )
	{
		shape->updateFrame();
	}

	if (USE_ODE)
	{
		const dReal* vel = dBodyGetLinearVel( m_bodyID );
		m_velocity.set( vel[0], vel[1], vel[2] );

		applyForce( m_velocity * m_rollingResistance );

		vel = dBodyGetAngularVel( m_bodyID );
		m_angularVelocity.set( vel[0], vel[1], vel[2] );

		MPBVector angularRR = m_angularVelocity * -.00002;
		dBodyAddTorque( m_bodyID, angularRR.x, angularRR.y, angularRR.z );

		if (m_planeCollisionTest)
		{
			MPB_LIST_LOOP( MPBGameEntities::shapes, shape, MPBShape* )
			{
				if (!shape->isCollidable()) continue;
				if (!shape->getGeomID()) continue;

				odeCollide( shape->getGeomID(), 0, shape->getElasticity(), NULL, shape );
			}
		}

		if (m_collisionTest)
		{
			MPB_LIST_LOOP( MPBPhysics::objects, obj, MPBPhysicsObject* )
			{
				if (obj == this) continue;
				if (!obj->m_collidable) continue;

				odeCollide( obj->m_geomID, obj->getBodyID(), obj->m_elasticity, obj, NULL );
			}
		}
	}
	else
	{
		////////////////////////////////////
		// update linear velocity

		// apply gravity

		MPBVector gravity( 0, -m_gravity, 0 );
	//	applyForce( gravity * m_mass );
		m_acceleration = m_acceleration + gravity;

		// apply lift

		m_acceleration = m_acceleration + m_lift;

		// apply drag
	//	const MPBFLOAT DRAG_COEFFICIENT = 0.3;
		const MPBFLOAT DRAG_COEFFICIENT = .05f;
	//	MPBFLOAT frontalArea =	(m_radius * 1) * (m_radius * 1);
		MPBFLOAT frontalArea =	1;
		MPBFLOAT drag = -.5f * DRAG_COEFFICIENT * frontalArea * m_airDensity;

		MPBVector dragForce(	drag * m_velocity.x,
									drag * m_velocity.y,
									drag * m_velocity.z	);

		// square v at fast speeds?
	//	MPBVector dragForce(	drag * m_velocity.x * fabs( m_velocity.x ),
	//								drag * m_velocity.y * fabs( m_velocity.y ),
	//								drag * m_velocity.z * fabs( m_velocity.z )	);

		// apply collision friction
		MPBVector rollingResistanceForce;
		if (isGrounded())
		{
			rollingResistanceForce.x = m_rollingResistance * m_velocity.x;
			rollingResistanceForce.y = m_rollingResistance * m_velocity.y;
			rollingResistanceForce.z = m_rollingResistance * m_velocity.z;
		}

		// apply lateral friction
		if (m_lateralFriction && isGrounded())
		{
			applyForce( getLateralVelocity() * -getMass() * 100 );
		}

		m_acceleration = m_acceleration + dragForce + rollingResistanceForce;

		////////////////////////////
		// Euler integration

		m_velocity = m_velocity + m_acceleration * MPBFrameCounter::secondsElapsed;

		// apply velocity constraints
		if (m_constraints & CONSTRAIN_MOVEMENT_X) m_velocity.x = 0;
		if (m_constraints & CONSTRAIN_MOVEMENT_Y) m_velocity.y = 0;
		if (m_constraints & CONSTRAIN_MOVEMENT_Z) m_velocity.z = 0;

		// apply absolute maximum velocity
		const int MAX_VELOCITY = 1000;
		if (m_velocity.x > MAX_VELOCITY) m_velocity.x = MAX_VELOCITY;
		if (m_velocity.y > MAX_VELOCITY) m_velocity.y = MAX_VELOCITY;
		if (m_velocity.z > MAX_VELOCITY) m_velocity.z = MAX_VELOCITY;

		////////////////////////////////////
		// update angular velocity

	/*	const MPBFLOAT ANGULAR_DRAG = -10;//-.01;
		
		MPBVector angularDrag(	ANGULAR_DRAG * m_angularVelocity.x * fabs( m_angularVelocity.x ),
										ANGULAR_DRAG * m_angularVelocity.y * fabs( m_angularVelocity.y ),
										ANGULAR_DRAG * m_angularVelocity.z * fabs( m_angularVelocity.z )	);

		angularDrag = angularDrag * MPBFrameCounter::secondsElapsed;
		MPBVector angularVelocityAfterDrag = m_angularVelocity + angularDrag;
	*/
	/*
		// don't allow drag to change the object's direction
		if (fabs( angularDrag.x ) > fabs( m_angularVelocity.x )) angularDrag.x = 0;
		if (fabs( angularDrag.y ) > fabs( m_angularVelocity.y )) angularDrag.y = 0;
		if (fabs( angularDrag.z ) > fabs( m_angularVelocity.z )) angularDrag.z = 0;
	*/

	//	m_angularVelocity = m_angularVelocity + angularDrag;

		m_angularVelocity = m_angularVelocity + m_angularAcceleration * MPBFrameCounter::secondsElapsed;
	/*
		MPBVector angularVelocityThisFrame = m_angularVelocity * MPBFrameCounter::secondsElapsed;

		// apply rotation constraints
		if (m_constraints & CONSTRAIN_ROTATION_X) angularVelocityThisFrame.x = 0;
		if (m_constraints & CONSTRAIN_ROTATION_Y) angularVelocityThisFrame.y = 0;
		if (m_constraints & CONSTRAIN_ROTATION_Z) angularVelocityThisFrame.z = 0;

		// apply absolute maximum angular velocity
		const int MAX_ANGULAR_VELOCITY = 10;
		if (m_angularVelocity.x > MAX_ANGULAR_VELOCITY) m_angularVelocity.x = MAX_ANGULAR_VELOCITY;
		if (m_angularVelocity.y > MAX_ANGULAR_VELOCITY) m_angularVelocity.y = MAX_ANGULAR_VELOCITY;
		if (m_angularVelocity.z > MAX_ANGULAR_VELOCITY) m_angularVelocity.z = MAX_ANGULAR_VELOCITY;

		////////////////////////////////////
		// update rotation

		if (!m_freeRotate)
		{
			m_orientationQuaternion = m_orientationQuaternion + (m_orientationQuaternion * angularVelocityThisFrame * .5);
			m_orientationQuaternion.normalize();
			updateOrientationVector();
		}
	*/

		m_acceleration.set( 0, 0, 0 );
		m_angularAcceleration.set( 0, 0, 0 );

		if (m_immovable)
		{
		}
		else if (m_owner) 
		{
			m_position = m_ownerOffset;
			m_owner->m_orientationQuaternion.rotateVector( m_position );
			m_position = m_position + m_owner->getPosition();
		}
		else if (m_ownerPP)
		{
			m_position = m_ownerOffset;
			m_position = m_position * m_ownerPP->getRotationMatrix();
			m_position = m_position + m_ownerPP->getPosition();
		}
		else if (m_ownerRB)
		{
			m_position = m_ownerOffset;
			m_position = m_position * m_ownerRB->getRotationMatrix();
			m_position = m_position * m_ownerRB->getTranslationMatrix();
		}
		else
		{
			m_collisionTestCalls = 0;
			collisionTest();
		}
	}

	m_aabb.setCenter( getPosition() );

	MPB3DObject::updateFrame();
}

// new position gets set here.
// set m_velocity and m_angularVelocity before calling.
void MPBPhysicsObject::collisionTest()
{
	if (++m_collisionTestCalls >= 10) return;

	// find local velocity
	m_localVelocity = m_velocity;
	MPBQuaternion tmp = ~m_orientationQuaternion;
	tmp.rotateVector( m_localVelocity );

	// done if not actually moving
	if (floatEQ( m_velocity.getLength(), 0 )) 
		return;


	////////////////////////////////////
	// adjust velocities for frame rate

	MPBVector velocityVector = m_velocity * MPBFrameCounter::secondsElapsed;
	MPBFLOAT travelDistance = velocityVector.getLength();
	MPBVector velocityVectorNormalized = velocityVector;
	velocityVectorNormalized.normalize();
	m_prevPosition = m_position;
	MPBVector startPosition = m_position;
	MPBVector destPosition = m_position + velocityVector;

	////////////////////////////////////
	// get planes to collide with

	MPBList planes;

	MPBAABB moveBox;
	if (m_planeCollisionTest || m_collisionTest)
	{
		// make a box that bounds the start and destination positions
		MPBVector extents = m_radiusVector;//( m_radius, m_radius, m_radius );

		MPBAABB startBox;
		startBox.setCenterAndExtents( startPosition, extents );

		MPBAABB destBox;
		destBox.setCenterAndExtents( destPosition, extents );

		moveBox.bound( startBox, destBox );

		MPBGameEntities::getCollisionPlanes( planes, moveBox );
	}

	////////////////////////////////////
	// test for collisions

	MPBList collisionList;
	Collision* collision;
	m_collided = false;

	if (m_poType == POT_AABB)
	{
		if (m_collisionTest)
		{
			MPB_LIST_LOOP( MPBPhysics::objects, obj, MPBPhysicsObject* )
			{
				if (obj == this) continue;
				if (!obj->m_collidable) continue;

				if (collisionAABBAABB(	destPosition,
												moveBox,
												obj->m_aabb,
												velocityVectorNormalized,
												travelDistance,
												collision	))
				{
					collisionList.push_back( collision );
					collision->m_targetObject = obj;
					m_collided = true;
				}
			}
		}

		if (m_planeCollisionTest)
		{
			MPB_LIST_LOOP( planes, plane, Plane* )
			{
				if (!plane->isCollidable()) continue;

				if (collisionAABBPlane(	travelDistance,
												velocityVector,
												velocityVectorNormalized,
												destPosition,
												m_aabb,
												plane,
												collision	))
				{
					collisionList.push_back( collision );
					m_collided = true;
					break;
				}
			}
		}
	}
	else if (m_poType == POT_SPHERE)
	{
		if (m_collisionTest)
		{
			MPB_LIST_LOOP( MPBPhysics::objects, obj, MPBPhysicsObject* )
			{
				if (obj == this) continue;
				if (!obj->m_collidable) continue;

				if (collisionSphereSphere(	startPosition,
													destPosition,
													m_radius,
													obj->m_position,
													obj->m_radius,
													velocityVectorNormalized,
													travelDistance,
													collision	))
				{
					collisionList.push_back( collision );
					collision->m_targetObject = obj;
					m_collided = true;
				}
			}
		}

		if (m_planeCollisionTest)
		{
			MPB_LIST_LOOP( planes, plane, Plane* )
			{
				if (!plane->isCollidable()) continue;

				if (collisionSpherePlane(	plane, 
													startPosition, 
													destPosition, 
													velocityVectorNormalized,
													velocityVector,
													travelDistance,
													m_radius,
													collision	))
				{
					collisionList.push_back( collision );
					collision->m_targetPlane = plane;
					m_collided = true;
				}
			}
		}
	}
	else if (m_poType == POT_BODY)
	{
/*		if (m_collisionTest)
		{
			MPB_LIST_LOOP( MPBPhysics::objects, obj, MPBPhysicsObject* )
			{
				if (obj == this) continue;
				if (!obj->m_collidable) continue;

				if (collisionSphereSphere(	startPosition,
													destPosition,
													m_radius,
													obj->m_position,
													obj->m_radius,
													velocityVectorNormalized,
													travelDistance,
													collision	))
				{
					collisionList.push_back( collision );
					collision->m_targetObject = obj;
					m_collided = true;
				}
			}
		}
*/
		if (m_planeCollisionTest)
		{
			MPB_LIST_LOOP( planes, plane, Plane* )
			{
				if (!plane->isCollidable()) continue;

				for (int i=0; i<m_vertices.size(); i++)
				{
					MPBFLOAT distanceToPlane;
					MPBVector planeIntersectionPoint;

					if (collisionPointPlane(	m_vertices[i],
														plane,
														velocityVector,
														travelDistance,
														distanceToPlane,
														planeIntersectionPoint	))
					{
						collision = new Collision(	distanceToPlane, planeIntersectionPoint, MPBVector() );
						collisionList.push_back( collision );
						collision->m_targetPlane = plane;
						m_collided = true;
					}
				}
			}
		}
	}

	////////////////////////////////////
	// react to collisions

	if (m_collided)
	{
		////////////////////////////////////////////
		// find closest collision

		collision = (Collision*)collisionList[0];
		MPB_LIST_LOOP( collisionList, c, Collision* )
		{
			if (collision != c && collisionLessThan( c, collision ))
				collision = c;
		}

		if (m_poType == POT_BODY)
		{
			MPBList collisionList2;

			MPB_LIST_LOOP( collisionList, c, Collision* )
			{
				if (collisionEquals( c, collision ))
					collisionList2.push_back( c );
			}

			MPBVector newDestination;
			MPB_LIST_LOOP( collisionList2, c2, Collision* )
			{
				newDestination = newDestination + c2->m_newDestination;
			}

			newDestination = newDestination / collisionList2.size();

			Plane* tp = collision->m_targetPlane;
			collision = new Collision( collision->m_distance, newDestination, MPBVector() );
			collision->m_targetPlane = tp;

			MPB_LIST_DELETE( collisionList2, Collision* );
		}

		// tell child classes
		handleCollision1( collision );

		// move as far as we can
//		m_position = collision->m_newDestination;

		// see what we collided with
		MPBPhysicsObject* targetObject = collision->m_targetObject;
		Plane* targetPlane = collision->m_targetPlane;
		MPBVector collisionPoint = collision->m_newDestination;// + velocityVector * m_radius;

		MPBVector newVelocity1;
		MPBVector newVelocity2;
		MPBVector newAngularVelocity1;
		MPBVector newAngularVelocity2;
		MPBVector oldVelocity = m_velocity;

		if (targetObject)
		{
			if (m_poType == POT_AABB)
			{
				m_velocity.reset();
			}
			else
			{
				bool ignoreAngular = true;
//				if (m_poType == POT_BODY) ignoreAngular = false;

				collisionBounce(	m_elasticity,
										collisionPoint,
										m_velocity,
										targetObject->m_velocity,
										m_localVelocity,
										targetObject->m_localVelocity,
										m_angularVelocity,
										targetObject->m_angularVelocity,
										m_position,
										targetObject->m_position,
										m_orientationQuaternion,
										targetObject->m_orientationQuaternion,
										m_mass, 
										targetObject->m_mass, 
										m_inertia, 
										targetObject->m_inertia, 
										newVelocity1,
										newVelocity2,
										newAngularVelocity1,
										newAngularVelocity2,
										ignoreAngular	);
				
				m_velocity = newVelocity1;
//				m_angularVelocity = newAngularVelocity1;

				targetObject->m_velocity = newVelocity2;
//				targetObject->m_angularVelocity = newAngularVelocity2;
			}

			if (m_objectCollisionSound)
			{
				MPBFLOAT speed = m_velocity.getLength();
				if (speed > .1)
				{
//					MPBSoundManager::playSound( m_objectCollisionSound, m_position, oldVelocity, speed * 1000 - 3000, false );
				}
			}
		}
		else if (targetPlane)
		{
			if (0)//(m_poType == POT_BODY)
			{
				m_velocity.reset();
			}
/*			else if (0)//(m_poType == POT_AABB)
			{
				// slide
				m_position = collision->m_newDestination;
				m_velocity = collision->m_slideVector / MPBFrameCounter::secondsElapsed;
				collisionTest();
				m_collided = true;
			}
*/			else if (1)
			{
				bool ignoreAngular = true;
				if (m_poType == POT_BODY) ignoreAngular = false;

				collisionBounce(	m_elasticity,
										collisionPoint,
										m_velocity,
										MPBVector( 0, 0, 0 ),
										m_localVelocity,
										MPBVector( 0, 0, 0 ),
										m_angularVelocity,
										MPBVector( 0, 0, 0 ),
										m_position,
										m_position - targetPlane->getNormal(), 
										m_orientationQuaternion,
										MPBQuaternion( 0, 0, 0, 0 ),
										m_mass, 
										0, 
										m_inertia, 
										0, 
										newVelocity1,
										newVelocity2,
										newAngularVelocity1,
										newAngularVelocity2,
										ignoreAngular	);

				m_velocity = newVelocity1;
				if (!ignoreAngular) m_angularVelocity = newAngularVelocity1;

				collisionTest();
				m_collided = true;
			}
/*			else if (0)	// slide
			{
				m_velocity = collision->m_slideVector / MPBFrameCounter::secondsElapsed;
				collisionTest();
				m_velocity.reset();
			}
			else	// stop moving
			{
				m_velocity.reset();
			}
*/
			if (m_planeCollisionSound)
			{
				MPBSoundManager::collisionSound( targetPlane, oldVelocity, 5, m_position, m_planeCollisionSound );
			}
		}

		// tell child classes
		handleCollision2( collision );
	}
	else
	{
		// no collision; move to desired position
		m_position = destPosition;
		m_velocity = (m_position - m_prevPosition) / MPBFrameCounter::secondsElapsed;


		if (m_poType == POT_BODY)
		{
			MPBVector angularVelocityThisFrame = m_angularVelocity * MPBFrameCounter::secondsElapsed;

			// apply rotation constraints
			if (m_constraints & CONSTRAIN_ROTATION_X) angularVelocityThisFrame.x = 0;
			if (m_constraints & CONSTRAIN_ROTATION_Y) angularVelocityThisFrame.y = 0;
			if (m_constraints & CONSTRAIN_ROTATION_Z) angularVelocityThisFrame.z = 0;

			// apply absolute maximum angular velocity
			const int MAX_ANGULAR_VELOCITY = 10;
			if (m_angularVelocity.x > MAX_ANGULAR_VELOCITY) m_angularVelocity.x = MAX_ANGULAR_VELOCITY;
			if (m_angularVelocity.y > MAX_ANGULAR_VELOCITY) m_angularVelocity.y = MAX_ANGULAR_VELOCITY;
			if (m_angularVelocity.z > MAX_ANGULAR_VELOCITY) m_angularVelocity.z = MAX_ANGULAR_VELOCITY;

			////////////////////////////////////
			// update rotation

			if (!m_freeRotate)
			{
				m_orientationQuaternion = m_orientationQuaternion + (m_orientationQuaternion * angularVelocityThisFrame * .5);
				m_orientationQuaternion.normalize();
				updateOrientationVector();
			}

			m_vertices.clear();

	/*		MPB_LIST_LOOP( m_shapes, shape, MPBShape* )
			{
				MPBVectorList vertices = shape->getVertices();

				MPB_LIST_LOOP( vertices, v, MPBVector* )
				{
					m_vertices.push_back( v );
				}
			}
	*/
			m_vertices.push_back( MPBVector( -m_radius / 2, -m_radius / 2, m_radius / 2 ) );
			m_vertices.push_back( MPBVector( -m_radius / 2,  m_radius / 2, m_radius / 2 ) );
			m_vertices.push_back( MPBVector(  m_radius / 2,  m_radius / 2, m_radius / 2 ) );
			m_vertices.push_back( MPBVector(  m_radius / 2, -m_radius / 2, m_radius / 2 ) );

			m_vertices.push_back( MPBVector( -m_radius / 2, -m_radius / 2, -m_radius / 2 ) );
			m_vertices.push_back( MPBVector( -m_radius / 2,  m_radius / 2, -m_radius / 2 ) );
			m_vertices.push_back( MPBVector(  m_radius / 2,  m_radius / 2, -m_radius / 2 ) );
			m_vertices.push_back( MPBVector(  m_radius / 2, -m_radius / 2, -m_radius / 2 ) );

			for (int i=0; i<m_vertices.size(); i++)
			{
				m_orientationQuaternion.rotateVector( m_vertices[i] );
				m_vertices[i].translate( m_position.x, m_position.y, m_position.z );
			}
		}
	}

	MPB_LIST_DELETE( collisionList, Collision* );
}

bool MPBPhysicsObject::pathIsClear(
	MPBVector to,
	MPBFLOAT radius,
	bool testObjects,
	MPB3DObject* target	// object to ignore collisions with, can be null
)
{
	if (0)//(m_poType == POT_AABB)
	{
		return pathIsClearAABB( getPosition(), to, radius, testObjects, this, target );
	}
	else
	{
		Collision* collision = NULL;
		return pathIsClearSphere( getPosition(), to, radius, testObjects, this, target, collision );
	}
}






