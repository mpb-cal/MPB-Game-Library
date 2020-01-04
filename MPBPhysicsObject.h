#ifndef __MPBPHYSICS_OBJECT_H__
#define __MPBPHYSICS_OBJECT_H__

#include "MPBLibrary.h"
#include "MPBQuaternion.h"
#include "MPBShape.h"
#include "MPBMatrix.h"
#include "MPBList.h"
#include "MPBVectorList.h"
#include "MPBPlane.h"
#include "MPBStreamingSound.h"
#include "MPBShapeTube.h"
#include "MPBShapeBall.h"
#include "MPBCountdown.h"


class MPBPhysics;
class Plane;
class Collision;
class MPBParticleSystem;
class MPBHardSpring;
class MPBRigidBody;
class MPBPhysicsParticle;


enum PhysicsObjectType
{
	POT_SPHERE,
	POT_BOX,
	POT_CYLINDER
};


class MPBPhysicsObject
{
public:

	MPBPhysicsObject( PhysicsObjectType t );
	virtual ~MPBPhysicsObject();

	virtual void draw( bool shadow, Plane* shadowReceiver, MPBVector vectorToSun, bool forShadowVolume );
	virtual void updateFrame();
	virtual void handleCollision1( Collision* c ) {}	// called before collision response
	virtual void handleCollision2( Collision* c ) {}	// called after collision response

	void applyTorque( const MPBVector& force, const MPBVector& pointOfContact );
	void applyTorqueImpulse( const MPBVector& force, const MPBVector& pointOfContact );
	void drawShadowVolume( const MPBVector& lightPos );
	void createShadowVolume( MPBVector& lightPos );
	void applyForce( const MPBVector& force );
	void applyImpulse( const MPBVector& force );
	void addShape( MPBShape* s );
	void removeShape( MPBShape* s );
	void stopSound();
	void killStreamingSound();

	void setOwner( MPBPhysicsObject* owner, MPBVector ownerOffset );
	void setOwner( MPBRigidBody* owner, MPBVector ownerOffset );
	void setOwner( MPBPhysicsParticle* owner, MPBVector ownerOffset );
	void setPosition( MPBVector pos );
	void setRadius( MPBFLOAT s );
	void setRadius( MPBVector v );
	void setCylinderLength( MPBFLOAT s );
	void setMass( MPBFLOAT s );
	void setCollisionTest( bool set ) { m_collisionTest = set; }
	void setPlaneCollisionTest( bool set ) { m_planeCollisionTest = set; }
	void setCollidable( bool set ) { m_collidable = set; }
	void setDraw( bool set ) { m_draw = set; }
	void setElasticity( MPBFLOAT d ) { MPB_ASSERT( m_elasticity >= 0.0 && m_elasticity <= 1.0 ); m_elasticity = d; }
	void setAlive( bool b ) { m_alive = b; }
	void setDamage( MPBFLOAT s ) { m_damage = s; }
	void addDamage( MPBFLOAT d );
	void setTexture( const char* texture ) { strcpy( m_texture, texture ); }
//	void setRollingResistance( MPBFLOAT d ) { m_rollingResistance = d; }
	void setDrag( MPBFLOAT d ) { m_drag = d; }
	void setAngularDrag( MPBFLOAT d ) { m_angularDrag = d; }
	void setGravity( bool b );
	void setColor( BYTE r, BYTE g, BYTE b ) { m_red = r; m_green = g; m_blue = b; }
	void setPlaneCollisionSound( char* filename ) { m_planeCollisionSound = filename; }
	void setObjectCollisionSound( char* filename ) { m_objectCollisionSound = filename; }
	void setNetworkId( unsigned short id ) { m_networkId = id; }
	void setSound( char* filename );
	void setStreamingSound( MPBStreamingSound* s );
	void setCastsShadow( bool s ) { m_castsShadow = s; }
	void setFriction( dReal d ) { m_friction = d; }
	void setFriction2( dReal d ) { m_friction2 = d; m_useContactFriction2 = true; }
	void setSlip1( dReal d ) { m_slip1 = d; m_useContactSlip1 = true; }
	void setContactFDir( MPBVector v ) { m_contactFDir = v; m_useContactFDir = true; }
	void setTTL( int ms );

	MPBVector getPosition() const;
	MPBMatrix getRotation() const;
	MPBVector getVelocity() const;
	MPBVector getAngularVelocity() const;
	MPBFLOAT getSpeed() const;
	dReal getMass() const;
	bool getCollidable() { return m_collidable; }
	MPBFLOAT getRadius() const { return m_radius; }
	bool isAlive() const { return m_alive; }
	MPBFLOAT getDamage() { return m_damage; }
	bool getCastsShadow() { return m_castsShadow; }
	bool isGrounded() const { return m_collided; }
	unsigned short getNetworkId() { return m_networkId; }
	MPBStreamingSound* getStreamingSound() { return m_streamingSound; }
	dBodyID getBodyID() { return m_bodyID; }
	dReal getFriction() { return m_friction; }
	dReal getFriction2() { return m_friction2; }
	dJointID addBallJoint( MPBPhysicsObject* attachTo, MPBVector anchor );
	dJointID addHingeJoint( MPBPhysicsObject* attachTo, MPBVector anchor, MPBVector axis );
	dJointID addSliderJoint( MPBPhysicsObject* attachTo, MPBVector axis );
	dJointID addUniversalJoint( MPBPhysicsObject* attachTo, MPBVector anchor, MPBVector axis1, MPBVector axis2 );
	dJointID addHinge2Joint( MPBPhysicsObject* attachTo, MPBVector anchor, MPBVector axis1, MPBVector axis2 );
	dJointID addAMotorJoint( MPBPhysicsObject* attachTo, MPBVector axis1, MPBVector axis2 );


protected:

	void									odeCollisionResponse(	
												int numContacts, 
												dContact* contacts,
												dBodyID obstacleBodyID, 
												MPBFLOAT obstacleElasticity,
												MPBPhysicsObject* obstaclePO,		// can be NULL
												MPBShape* obstacleShape				// can be NULL
											);
	void									odeCollide(
												dGeomID obstacleGeomID, 
												dBodyID obstacleBodyID, 
												MPBFLOAT obstacleElasticity,
												MPBPhysicsObject* obstaclePO,		// can be NULL
												MPBShape* obstacleShape				// can be NULL
											);
	void heightMapCollide();
	void makeCylinderShape();

	dGeomID								m_geomID;
	PhysicsObjectType					m_poType;
	MPBPhysicsObject*					m_owner;							// this object will follow its owner
	MPBPhysicsParticle*				m_ownerPP;
	MPBRigidBody*						m_ownerRB;
	MPBVector							m_ownerOffset;
	MPBFLOAT 							m_radius;						// in meters
	MPBVector							m_radiusVector;
	MPBFLOAT 							m_cylinderLength;
	MPBFLOAT 							m_mass;

	bool									m_collisionTest;
	bool									m_planeCollisionTest;
	bool									m_collidable;
	bool									m_castsShadow;
	char									m_texture[MAX_PATH];
	MPBFLOAT 							m_elasticity;					// 0 - 1.0
//	MPBFLOAT 							m_rollingResistance;
	MPBFLOAT 							m_drag;							// -.00001f works well
	MPBFLOAT 							m_angularDrag;					// -.0001f works well
	bool									m_draw;
	bool									m_alive;
	bool									m_isParticleSystem;
	MPBAABB								m_aabb;
	MPBFLOAT 							m_damage;
	MPBShape*							m_shadowVolumeShape;

	dBodyID								m_bodyID;
	dReal									m_friction;
	dReal									m_friction2;
	dReal									m_slip1;
	bool									m_useContactFriction2;
	bool									m_useContactFDir;
	bool									m_useContactSlip1;
	MPBVector							m_contactFDir;

	MPBList								m_shapes;
	BYTE									m_red;
	BYTE									m_blue;
	BYTE									m_green;
	bool									m_collided;
	char*									m_planeCollisionSound;
	char*									m_objectCollisionSound;
	unsigned short						m_networkId;
	ALuint								m_sound;
	MPBStreamingSound*				m_streamingSound;
	MPBShapeTube*						m_cylinderShape;
	MPBShapeBall*						m_cylinderShapeBall1;
	MPBShapeBall*						m_cylinderShapeBall2;
	MPBList								m_joints;
	MPBCountdown*						m_countdownTTL;

	void die() { m_alive = false; }

	friend class MPBPhysics;
};



#endif

