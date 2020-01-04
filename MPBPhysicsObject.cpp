
#include "MPBPhysics.h"
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
#include "MPBHeightMap.h"
#include "MPBShapeExtrusion.h"
#include "MPBPerformanceMonitor.h"
#include "MPBTriStrip.h"
#include <ode/ode.h>
#include "mmgr.h"


static const GLfloat ambientColorDefault[4] = { 1, 1, 1, 1 };
static const GLfloat diffuseColorDefault[4] = { 1, 1, 1, 1 };
static const GLfloat specularColorDefault[4] = { 1, 1, 1, 1 };
static const GLfloat shininessDefault[1] = { 128 };
static const GLfloat emissionColorDefault[4] = { 0, 0, 0, 0 };



MPBPhysicsObject::MPBPhysicsObject( PhysicsObjectType t ) :
	m_poType( t ),
	m_alive( true ),
	m_collisionTest( true ),
	m_planeCollisionTest( true ),
	m_collidable( true ),
	m_elasticity( 1 ),
	m_draw( true ),
	m_owner( NULL ),
	m_ownerRB( NULL ),
	m_ownerPP( NULL ),
	m_isParticleSystem( false ),
	m_castsShadow( true ),
	m_damage( 0 ),
//	m_rollingResistance( -13.0f ),
	m_geomID( 0 ),
	m_shadowVolumeShape( NULL ),
	m_red( 255 ),
	m_blue( 255 ),
	m_green( 255 ),
	m_collided( false ),
	m_planeCollisionSound( NULL ),
	m_objectCollisionSound( NULL ),
	m_networkId( 0 ),
	m_sound( 0 ),
	m_streamingSound( NULL ),
	m_cylinderLength( 1 ),
	m_cylinderShape( NULL ),
	m_cylinderShapeBall1( NULL ),
	m_cylinderShapeBall2( NULL ),
	m_friction( 0 ),
	m_friction2( 0 ),
	m_slip1( 1 ),
	m_useContactFriction2( false ),
	m_useContactFDir( false ),
	m_useContactSlip1( false ),
	m_mass( 1000 ),
	m_bodyID( 0 ),
	m_drag( 0 ),
	m_angularDrag( 0 ),
	m_countdownTTL( NULL )
{
//	m_red = 255.0 * RANDOM_FRACTION;
//	m_green = 255.0 * RANDOM_FRACTION;
//	m_blue = 255.0 * RANDOM_FRACTION;

	// ODE stuff

	m_bodyID = dBodyCreate( MPBGameEntities::worldID );

	if (m_poType == POT_SPHERE)
	{
		m_geomID = dCreateSphere( 0, 1 );
	}
	else if (m_poType == POT_BOX)
	{
		m_geomID = dCreateBox( 0, 1, 1, 1 );
	}
	else if (m_poType == POT_CYLINDER)
	{
		m_geomID = dCreateCCylinder( 0, 1, 1 );
	}

	dGeomSetBody( m_geomID, m_bodyID );

	setRadius( 1 );

	if (MPBOpenGL::is2D())
		strcpy( m_texture, "circle.tex" );
	else
		strcpy( m_texture, "metal3.tex" );
}

MPBPhysicsObject::~MPBPhysicsObject()
{
	dBodyDestroy( m_bodyID );

	MPB_LIST_DELETE( m_shapes, MPBShape* );

	stopSound();
	killStreamingSound();

	MPB_LIST_DELETE( m_shapes, MPBShape* );
	NULLIFY( m_countdownTTL );
}

void MPBPhysicsObject::addDamage( MPBFLOAT d )
{
	m_damage += d;
}


void MPBPhysicsObject::updateFrame()
{
	if (m_sound)
	{
		float alPos[3];
		alPos[0] = (float)getPosition().x / MPBSoundManager::soundPositionDivisor;
		alPos[1] = (float)getPosition().y / MPBSoundManager::soundPositionDivisor;
		alPos[2] = (float)getPosition().z / MPBSoundManager::soundPositionDivisor;
		alSourcefv( m_sound, AL_POSITION, alPos );
		MPB_ASSERT( alGetError() == AL_NO_ERROR );

		float alVel[3];
		alVel[0] = (float)getVelocity().x / MPBSoundManager::soundPositionDivisor;
		alVel[1] = (float)getVelocity().y / MPBSoundManager::soundPositionDivisor;
		alVel[2] = (float)getVelocity().z / MPBSoundManager::soundPositionDivisor;
		alSourcefv( m_sound, AL_VELOCITY, alVel );
		MPB_ASSERT( alGetError() == AL_NO_ERROR );
	}

	if (m_streamingSound)
	{
		m_streamingSound->updatePosition( getPosition(), getVelocity() );
		m_streamingSound->start();
	}

	MPB_LIST_LOOP( m_shapes, shape, MPBShape* )
	{
		shape->updateFrame();
	}

	// drag

	if (m_drag)
	{
		MPBVector drag = getVelocity() * getMass();
		drag.x = drag.x * fabs( drag.x );
		drag.y = drag.y * fabs( drag.y );
		drag.z = drag.z * fabs( drag.z );
		drag = drag * m_drag;
		applyForce( drag );
	}

//	applyForce( getVelocity() * m_rollingResistance );

	if (m_angularDrag)
	{
		MPBVector angularDrag = getAngularVelocity() * getMass();
		angularDrag.x = angularDrag.x * fabs( angularDrag.x );
		angularDrag.y = angularDrag.y * fabs( angularDrag.y );
		angularDrag.z = angularDrag.z * fabs( angularDrag.z );
		angularDrag = angularDrag * m_angularDrag;
		dBodyAddTorque( m_bodyID, angularDrag.x, angularDrag.y, angularDrag.z );
	}

	m_collided = false;

	if (MPBGameEntities::getHeightMap())
	{
		heightMapCollide();
	}

	if (m_planeCollisionTest)
	{
		MPB_LIST_LOOP( MPBGameEntities::shapes, shape, MPBShape* )
		{
			if (!shape->isCollidable()) continue;
			if (!shape->getGeomID()) continue;

			odeCollide( shape->getGeomID(), 0, shape->getElasticity(), NULL, shape );
			MPBPerformanceMonitor::m_spherePlaneCollisionTests++;
		}

		MPB_LIST_LOOP( MPBGameEntities::triStrips, ts, MPBTriStrip* )
		{
			if (!ts->getGeomID()) continue;

			odeCollide( ts->getGeomID(), 0, 1, NULL, NULL );
			MPBPerformanceMonitor::m_spherePlaneCollisionTests++;
		}
	}

	if (m_collisionTest)
	{
		MPB_LIST_LOOP( MPBPhysics::objects, obj, MPBPhysicsObject* )
		{
			if (obj == this) continue;
			if (!obj->m_collidable) continue;

			odeCollide( obj->m_geomID, obj->getBodyID(), obj->m_elasticity, obj, NULL );
			MPBPerformanceMonitor::m_sphereSphereCollisionTests++;
		}
	}

	if (m_countdownTTL && m_countdownTTL->isDone())
	{
		die();
	}
}

void MPBPhysicsObject::setTTL( int ms )
{
	m_countdownTTL = new MPBCountdown( ms, false );
}


///////////////////////////////////////////////////////
// graphics methods

void MPBPhysicsObject::draw( 
	bool shadow, 
	Plane* shadowReceiver, 
	MPBVector vectorToSun,
	bool forShadowVolume ) 
{
	if (!m_draw && !shadow && !forShadowVolume) return;

//	if (!shadow && !forShadowVolume && !MPBOpenGL::sphereInFrustum( getPosition(), m_radius )) return;

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

		glMultMatrixf( shadowMatrix );
	}
	else if (forShadowVolume)
	{
	}
	else
	{
		MPBOpenGL::bindTexture( m_texture );
	}

	// translation & rotation

	glTranslated( getPosition().x, getPosition().y, getPosition().z );

	MPBFLOAT matrix[16];

	const dReal *bq = dBodyGetQuaternion( m_bodyID );
	MPBQuaternion q( bq[0], bq[1], bq[2], bq[3] );
	q.toMatrix( matrix );

	glMultMatrixf( matrix );

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
		if (m_poType == POT_SPHERE)
		{
			gluSphere( MPBOpenGL::quadricObj, m_radius, 20, 20 );
		}
		else if (m_poType == POT_BOX)
		{
			m_aabb.draw();
		}
		else if (m_poType == POT_CYLINDER)
		{
			glPushMatrix();
			glTranslated( 0, 0, -m_cylinderLength / 2 );
			gluCylinder( MPBOpenGL::quadricObj, m_radius, m_radius, m_cylinderLength, 20, 20 );
			glPopMatrix();

			glPushMatrix();
			glTranslated( 0, 0, -m_cylinderLength / 2 );
			gluSphere( MPBOpenGL::quadricObj, m_radius, 20, 20 );
			glPopMatrix();

			glPushMatrix();
			glTranslated( 0, 0, m_cylinderLength / 2 );
			gluSphere( MPBOpenGL::quadricObj, m_radius, 20, 20 );
			glPopMatrix();
		}
	}

	glPopMatrix();

	MPB_LIST_LOOP( m_joints, j, dJointID )
	{
		if (dJointGetType( j ) == dJointTypeBall)
		{
			dVector3 result1;
			dVector3 result2;
			dJointGetBallAnchor( j, result1 );
			dJointGetBallAnchor2( j, result2 );
			dBodyID id1 = dJointGetBody( j, 0 );
			dBodyID id2 = dJointGetBody( j, 1 );

			if (id1 && id2)
			{
				const dReal *pos1 = dBodyGetPosition( id1 );
				const dReal *pos2 = dBodyGetPosition( id2 );

				glLineWidth( 4 );
				glColor4ub( 255, 255, 255, 255 );
				glBegin( GL_LINES );
				glVertex3d( pos1[0], pos1[1], pos1[2] );
				glVertex3d( result1[0], result1[1], result1[2] );
				glVertex3d( pos2[0], pos2[1], pos2[2] );
				glVertex3d( result2[0], result2[1], result2[2] );
				glEnd();

				glPushMatrix();
				glTranslated( result1[0], result1[1], result1[2] );
				gluSphere( MPBOpenGL::quadricObj, m_radius / 8, 8, 8 );
				glPopMatrix();

				glPushMatrix();
				glTranslated( result2[0], result2[1], result2[2] );
				gluSphere( MPBOpenGL::quadricObj, m_radius / 8, 8, 8 );
				glPopMatrix();
			}
		}
		else if (dJointGetType( j ) == dJointTypeSlider)
		{
			dBodyID id1 = dJointGetBody( j, 0 );
			dBodyID id2 = dJointGetBody( j, 1 );
			if (id1 && id2)
			{
				const dReal *pos1 = dBodyGetPosition( dJointGetBody( j, 0 ) );
				const dReal *pos2 = dBodyGetPosition( dJointGetBody( j, 1 ) );

				glLineWidth( 4 );
				glColor4ub( 255, 255, 255, 255 );
				glBegin( GL_LINES );
				glVertex3d( pos1[0], pos1[1], pos1[2] );
				glVertex3d( pos2[0], pos2[1], pos2[2] );
				glEnd();
			}
		}
		else if (dJointGetType( j ) == dJointTypeHinge)
		{
			dVector3 result1;
			dVector3 result2;
			dJointGetHingeAnchor( j, result1 );
			dJointGetHingeAnchor2( j, result2 );
			dBodyID id1 = dJointGetBody( j, 0 );
			dBodyID id2 = dJointGetBody( j, 1 );
			if (id1 && id2)
			{
				const dReal *pos1 = dBodyGetPosition( dJointGetBody( j, 0 ) );
				const dReal *pos2 = dBodyGetPosition( dJointGetBody( j, 1 ) );

	//			dVector3 axis;
	//			dJointGetHingeAxis( j, axis );

				glLineWidth( 4 );
				glColor4ub( 255, 255, 255, 255 );
				glBegin( GL_LINES );
				glVertex3d( pos1[0], pos1[1], pos1[2] );
				glVertex3d( result1[0], result1[1], result1[2] );
				glVertex3d( pos2[0], pos2[1], pos2[2] );
				glVertex3d( result2[0], result2[1], result2[2] );
				glEnd();

				glPushMatrix();
				glTranslated( result1[0], result1[1], result1[2] );
				gluSphere( MPBOpenGL::quadricObj, m_radius / 8, 8, 8 );
				glPopMatrix();

				glPushMatrix();
				glTranslated( result2[0], result2[1], result2[2] );
				gluSphere( MPBOpenGL::quadricObj, m_radius / 8, 8, 8 );
				glPopMatrix();
			}
		}
		else if (dJointGetType( j ) == dJointTypeUniversal)
		{
			dVector3 result1;
			dVector3 result2;
			dJointGetUniversalAnchor( j, result1 );
			dJointGetUniversalAnchor2( j, result2 );
			dBodyID id1 = dJointGetBody( j, 0 );
			dBodyID id2 = dJointGetBody( j, 1 );
			if (id1 && id2)
			{
				const dReal *pos1 = dBodyGetPosition( dJointGetBody( j, 0 ) );
				const dReal *pos2 = dBodyGetPosition( dJointGetBody( j, 1 ) );

				glLineWidth( 4 );
				glColor4ub( 255, 255, 255, 255 );
				glBegin( GL_LINES );
				glVertex3d( pos1[0], pos1[1], pos1[2] );
				glVertex3d( result1[0], result1[1], result1[2] );
				glVertex3d( pos2[0], pos2[1], pos2[2] );
				glVertex3d( result2[0], result2[1], result2[2] );
				glEnd();

				glPushMatrix();
				glTranslated( result1[0], result1[1], result1[2] );
				gluSphere( MPBOpenGL::quadricObj, m_radius / 8, 8, 8 );
				glPopMatrix();

				glPushMatrix();
				glTranslated( result2[0], result2[1], result2[2] );
				gluSphere( MPBOpenGL::quadricObj, m_radius / 8, 8, 8 );
				glPopMatrix();
			}
		}
		else if (dJointGetType( j ) == dJointTypeHinge2)
		{
			dVector3 result1;
			dVector3 result2;
			dJointGetHinge2Anchor( j, result1 );
			dJointGetHinge2Anchor2( j, result2 );
			dBodyID id1 = dJointGetBody( j, 0 );
			dBodyID id2 = dJointGetBody( j, 1 );
			if (id1 && id2)
			{
				const dReal *pos1 = dBodyGetPosition( dJointGetBody( j, 0 ) );
				const dReal *pos2 = dBodyGetPosition( dJointGetBody( j, 1 ) );

				glLineWidth( 4 );
				glColor4ub( 255, 255, 255, 255 );
				glBegin( GL_LINES );
				glVertex3d( pos1[0], pos1[1], pos1[2] );
				glVertex3d( result1[0], result1[1], result1[2] );
				glVertex3d( pos2[0], pos2[1], pos2[2] );
				glVertex3d( result2[0], result2[1], result2[2] );
				glEnd();

				glPushMatrix();
				glTranslated( result1[0], result1[1], result1[2] );
				gluSphere( MPBOpenGL::quadricObj, m_radius / 8, 8, 8 );
				glPopMatrix();

				glPushMatrix();
				glTranslated( result2[0], result2[1], result2[2] );
				gluSphere( MPBOpenGL::quadricObj, m_radius / 8, 8, 8 );
				glPopMatrix();
			}
		}
	}
}

void MPBPhysicsObject::addShape( MPBShape* s ) 
{ 
	s->setFrustumCull( false ); 
	m_shapes.push_back( s ); 

	MPB_LIST_LOOP( s->getPlanes(), p, MPBTexturedPlane* )
	{
		if (MPBOpenGL::getPointLight( 0 ))
		{
			p->addTexture( "generatedLightmap" );
		}
	}
}

void MPBPhysicsObject::removeShape( MPBShape* s ) 
{ 
	m_shapes.remove( s ); 
}

void MPBPhysicsObject::createShadowVolume( MPBVector& lightPos )
{
	if (!m_castsShadow) return;

	const MPBFLOAT edgeLength = 100;

//	if (m_poType == POT_SPHERE)
	{
		delete m_shadowVolumeShape;

		m_shadowVolumeShape = new MPBShapeTube( m_radius * 1.1f, m_radius * 1.1f, edgeLength, 20, true, "" );
		m_shadowVolumeShape->setFrustumCull( false );
	}
}

void MPBPhysicsObject::drawShadowVolume( const MPBVector& lightPos )
{
	if (!m_castsShadow) return;

	const MPBFLOAT edgeLength = 100;

//	if (m_poType == POT_SPHERE)
	{
		glPushMatrix();
		glTranslated( getPosition().x, getPosition().y, getPosition().z );

		if (m_shadowVolumeShape)
		{
			MPBVector lightToMe = getPosition() - lightPos;
			MPBQuaternion q;
			q.fromVector( lightToMe.normalized() );
			MPBMatrix m;
			q.toMatrix( m );

			m_shadowVolumeShape->setDrawMatrix( m );

			m_shadowVolumeShape->setDrawn( false );
			m_shadowVolumeShape->draw( false, false, false );
		}

		glPopMatrix();
	}
//	else MPB_ASSERT( 0 );
}


/////////////////////////////////////////////////////////
// physics methods

MPBVector MPBPhysicsObject::getPosition() const 
{ 
	const dReal *pos = dBodyGetPosition( m_bodyID );
	return MPBVector( pos[0], pos[1], pos[2] );
}

MPBVector MPBPhysicsObject::getVelocity() const
{
	const dReal* vel = dBodyGetLinearVel( m_bodyID );
	return MPBVector( vel[0], vel[1], vel[2] );
}

MPBVector MPBPhysicsObject::getAngularVelocity() const
{
	const dReal* vel = dBodyGetAngularVel( m_bodyID );
	return MPBVector( vel[0], vel[1], vel[2] );
}

MPBFLOAT MPBPhysicsObject::getSpeed() const
{
	return getVelocity().getLength();
}

MPBMatrix MPBPhysicsObject::getRotation() const
{
	const dReal* rot = dBodyGetRotation( m_bodyID );
	MPBMatrix m;
	for (int i=0; i<12; i++)
		m.m[i] = rot[i];
	m.m[12] = 0;
	m.m[13] = 0;
	m.m[14] = 0;
	m.m[15] = 1;
	return m;
}


void MPBPhysicsObject::setCylinderLength( MPBFLOAT s ) 
{
	MPB_ASSERT( m_poType == POT_CYLINDER );
	m_cylinderLength = s;
	dGeomCCylinderSetParams( m_geomID, (dReal)m_radius, (dReal)m_cylinderLength );
}

void MPBPhysicsObject::setRadius( MPBFLOAT s ) 
{
	setRadius( MPBVector ( s, s, s ) ) ;
}

void MPBPhysicsObject::setRadius( MPBVector v ) 
{
	m_radiusVector = v;
	m_radius = v.x;

	dMass mass;

	if (m_poType == POT_SPHERE)
	{
		dGeomSphereSetRadius( m_geomID, (dReal)m_radius );
		dMassSetSphere( &mass, (dReal)m_mass, (dReal)m_radius );
	}
	else if (m_poType == POT_BOX)
	{
		dGeomBoxSetLengths( m_geomID, (dReal)m_radiusVector.x * 2.0, (dReal)m_radiusVector.y * 2.0, (dReal)m_radiusVector.z * 2.0 );
		dMassSetBox( &mass, (dReal)m_mass, (dReal)m_radiusVector.x * 2.0, (dReal)m_radiusVector.y * 2.0, (dReal)m_radiusVector.z * 2.0 );
	}
	else if (m_poType == POT_CYLINDER)
	{
		dGeomCCylinderSetParams( m_geomID, (dReal)m_radius, (dReal)m_cylinderLength );
		dMassSetCappedCylinder( &mass, (dReal)m_mass, 3, (dReal)m_radius, (dReal)m_cylinderLength );
	}

	dBodySetMass( m_bodyID, &mass );

	m_aabb.setCenterAndExtents( MPBVector(), m_radiusVector );
}

void MPBPhysicsObject::setMass( MPBFLOAT f )
{
	m_mass = f;
	setRadius( m_radiusVector );
}

dReal MPBPhysicsObject::getMass() const
{
	dMass mass;

	dBodyGetMass( m_bodyID, &mass );
	return mass.mass;
}

void MPBPhysicsObject::setPosition( MPBVector pos ) 
{ 
	dBodySetPosition( m_bodyID, pos.x, pos.y, pos.z );
	dBodySetLinearVel( m_bodyID, 0, 0, 0 );
	dBodySetAngularVel( m_bodyID, 0, 0, 0 );
}

void MPBPhysicsObject::setGravity( bool b ) 
{ 
	if (b) dBodySetGravityMode( getBodyID(), 1 );
	else dBodySetGravityMode( getBodyID(), 0 );
}

// force is Newtons per second
void MPBPhysicsObject::applyForce( const MPBVector& force )
{
	dBodyAddForce( m_bodyID, force.x, force.y, force.z );
}

// force is Newtons
void MPBPhysicsObject::applyImpulse( const MPBVector& force )
{
	dVector3 f;
	dWorldImpulseToForce( MPBGameEntities::worldID, MPBFrameCounter::secondsElapsed, force.x, force.y, force.z, f );
	dBodyAddForce( m_bodyID, f[0], f[1], f[2] );
}

// apply a torque
// pointOfContact is (ignored) in units, relative to center of mass
// magnitude of force should be inertia * (radians per second squared)
void MPBPhysicsObject::applyTorque( const MPBVector& force, const MPBVector& pointOfContact )
{
	dBodyAddRelTorque( m_bodyID, force.x, force.y, force.z );
}

void MPBPhysicsObject::applyTorqueImpulse( const MPBVector& force, const MPBVector& pointOfContact )
{
	MPB_ASSERT( 0 );
	dVector3 f;
	dWorldImpulseToForce( MPBGameEntities::worldID, MPBFrameCounter::secondsElapsed, force.x, force.y, force.z, f );
	dBodyAddTorque( m_bodyID, f[0], f[1], f[2] );
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

dJointID MPBPhysicsObject::addBallJoint( MPBPhysicsObject* attachTo, MPBVector anchor )
{
	dJointID jID = dJointCreateBall( MPBGameEntities::worldID, 0 );
	if (attachTo) dJointAttach( jID, m_bodyID, attachTo->getBodyID() );
	else dJointAttach( jID, m_bodyID, 0 );
	dJointSetBallAnchor( jID, anchor.x, anchor.y, anchor.z );

	m_joints.push_back( jID );
	return jID;
}

dJointID MPBPhysicsObject::addHingeJoint( MPBPhysicsObject* attachTo, MPBVector anchor, MPBVector axis )
{
	dJointID jID = dJointCreateHinge( MPBGameEntities::worldID, 0 );
	if (attachTo) dJointAttach( jID, m_bodyID, attachTo->getBodyID() );
	else dJointAttach( jID, m_bodyID, 0 );
	dJointSetHingeAnchor( jID, anchor.x, anchor.y, anchor.z );
	dJointSetHingeAxis( jID, axis.x, axis.y, axis.z );

	m_joints.push_back( jID );
	return jID;
}

dJointID MPBPhysicsObject::addSliderJoint( MPBPhysicsObject* attachTo, MPBVector axis )
{
	dJointID jID = dJointCreateSlider( MPBGameEntities::worldID, 0 );
	if (attachTo) dJointAttach( jID, m_bodyID, attachTo->getBodyID() );
	else dJointAttach( jID, m_bodyID, 0 );
	dJointSetSliderAxis( jID, axis.x, axis.y, axis.z );

	m_joints.push_back( jID );
	return jID;
}

dJointID MPBPhysicsObject::addUniversalJoint( MPBPhysicsObject* attachTo, MPBVector anchor, MPBVector axis1, MPBVector axis2 )
{
	dJointID jID = dJointCreateUniversal( MPBGameEntities::worldID, 0 );
	if (attachTo) dJointAttach( jID, m_bodyID, attachTo->getBodyID() );
	else dJointAttach( jID, m_bodyID, 0 );
	dJointSetUniversalAnchor( jID, anchor.x, anchor.y, anchor.z );
	dJointSetUniversalAxis1( jID, axis1.x, axis1.y, axis1.z );
	dJointSetUniversalAxis2( jID, axis2.x, axis2.y, axis2.z );

	m_joints.push_back( jID );
	return jID;
}

dJointID MPBPhysicsObject::addHinge2Joint( MPBPhysicsObject* attachTo, MPBVector anchor, MPBVector axis1, MPBVector axis2 )
{
	dJointID jID = dJointCreateHinge2( MPBGameEntities::worldID, 0 );
	if (attachTo) dJointAttach( jID, m_bodyID, attachTo->getBodyID() );
	else dJointAttach( jID, m_bodyID, 0 );
	dJointSetHinge2Anchor( jID, anchor.x, anchor.y, anchor.z );
	dJointSetHinge2Axis1( jID, axis1.x, axis1.y, axis1.z );
	dJointSetHinge2Axis2( jID, axis2.x, axis2.y, axis2.z );

	m_joints.push_back( jID );
	return jID;
}

// Euler mode
dJointID MPBPhysicsObject::addAMotorJoint( MPBPhysicsObject* attachTo, MPBVector axis1, MPBVector axis2 )
{
	dJointID jID = dJointCreateAMotor( MPBGameEntities::worldID, 0 );
	if (attachTo) dJointAttach( jID, m_bodyID, attachTo->getBodyID() );
	else dJointAttach( jID, m_bodyID, 0 );
	dJointSetAMotorMode( jID, dAMotorEuler );
	dJointSetAMotorAxis( jID, 0, 1, axis1.x, axis1.y, axis1.z );
	dJointSetAMotorAxis( jID, 2, 2, axis2.x, axis2.y, axis2.z );

	m_joints.push_back( jID );
	return jID;
}


void MPBPhysicsObject::odeCollisionResponse(	int numContacts, 
															dContact* contacts,
															dBodyID obstacleBodyID, 
															MPBFLOAT obstacleElasticity,
															MPBPhysicsObject* obstaclePO,		// can be NULL
															MPBShape* obstacleShape	)			// can be NULL
{
	m_collided = true;

	for (int i=0; i<numContacts; i++)
	{
		Collision collision( 0, MPBVector(), MPBVector() );
		collision.m_targetObject = obstaclePO;
		collision.m_targetPlane = NULL;
		collision.m_targetShape = obstacleShape;
		collision.m_normal = MPBVector( contacts[i].geom.normal[0], contacts[i].geom.normal[1], contacts[i].geom.normal[2] );

		handleCollision1( &collision );

		contacts[i].surface.mode = 0;

		if (m_useContactFriction2) contacts[i].surface.mode |= dContactMu2;
		if (m_useContactFDir) contacts[i].surface.mode |= dContactFDir1;
		if (m_useContactSlip1) contacts[i].surface.mode |= dContactSlip1;

		contacts[i].surface.mu = m_friction;
		if (m_useContactFriction2) contacts[i].surface.mu2 = m_friction2;
			// Coulomb friction coefficient. This must be in the range 0 to dInfinity. 
			// 0 results in a frictionless contact, and dInfinity results in a contact that never slips.
		contacts[i].surface.bounce = m_elasticity * obstacleElasticity;
		if (m_useContactFDir)
		{
			contacts[i].fdir1[0] = m_contactFDir.x;
			contacts[i].fdir1[1] = m_contactFDir.y;
			contacts[i].fdir1[2] = m_contactFDir.z;
		}

		if (m_useContactSlip1)
		{
			contacts[i].surface.slip1 = m_slip1;
		}

		dJointID jID = dJointCreateContact(	MPBGameEntities::worldID, 
														MPBGameEntities::contactJointGroup, 
														&contacts[i] );
		dJointAttach( jID, getBodyID(), obstacleBodyID );
	}
}

void MPBPhysicsObject::odeCollide( 
	dGeomID obstacleGeomID, 
	dBodyID obstacleBodyID, 
	MPBFLOAT obstacleElasticity,
	MPBPhysicsObject* obstaclePO,		// can be NULL
	MPBShape* obstacleShape				// can be NULL
)
{
	const int MAX_CONTACTS = 128;
	dContact contacts[MAX_CONTACTS];
	for (int i=0; i<MAX_CONTACTS; i++)
		memset( &contacts[i], 0, sizeof( dContact ) );

	int c = dCollide( m_geomID, obstacleGeomID, MAX_CONTACTS, &contacts[0].geom, sizeof( dContact ) );

	if (c)
	{
		odeCollisionResponse(	c, 
										contacts, 
										obstacleBodyID, 
										obstacleElasticity, 
										obstaclePO,
										obstacleShape	);
	}
}

void MPBPhysicsObject::heightMapCollide()
{
	dGeomID g = MPBGameEntities::getHeightMap()->getGeomID();
	if (g) odeCollide( g, 0, 1, NULL, NULL );
}


///////////////////////////////////////////////////////////
// sound methods

void MPBPhysicsObject::setStreamingSound( MPBStreamingSound* s )
{
	m_streamingSound = s;
	MPBSoundManager::addStreamingSound( s );
}

void MPBPhysicsObject::killStreamingSound()
{
	if (m_streamingSound)
	{
		MPBSoundManager::removeStreamingSound( m_streamingSound );
		NULLIFY( m_streamingSound );
	}
}

void MPBPhysicsObject::setSound( char* filename )
{
	m_sound = MPBSoundManager::createSoundSource( filename );

	if (m_sound)
	{
		alSourcef( m_sound, AL_GAIN, 1.0 );
		alSourcei( m_sound, AL_LOOPING, true );

		alSourcePlay( m_sound );
		MPB_ASSERT( alGetError() == AL_NO_ERROR );
	}
}

void MPBPhysicsObject::stopSound()
{
	if (m_sound) MPBSoundManager::destroySoundSource( m_sound );
	m_sound = 0;
}







