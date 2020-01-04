#ifndef __MPBPHYSICS_OBJECT_H__
#define __MPBPHYSICS_OBJECT_H__

#include "MPBLibrary.h"
#include "MPBQuaternion.h"
#include "MPBShape.h"
#include "MPBMatrix.h"
#include "MPB3DObject.h"
#include "MPBList.h"
#include "MPBVectorList.h"
#include "MPBPlane.h"


class MPBPhysics;
class Plane;
class Collision;
class MPBParticleSystem;
class MPBHardSpring;
class MPBRigidBody;
class MPBPhysicsParticle;


const int CONSTRAIN_MOVEMENT_X = 0x01;
const int CONSTRAIN_MOVEMENT_Y = 0x02;
const int CONSTRAIN_MOVEMENT_Z = 0x04;
const int CONSTRAIN_ROTATION_X = 0x08;
const int CONSTRAIN_ROTATION_Y = 0x10;
const int CONSTRAIN_ROTATION_Z = 0x20;

const MPBFLOAT AIR_DENSITY = 1.3f;		// kg/m^3
const MPBFLOAT WATER_DENSITY = 1000;	// kg/m^3

enum PhysicsObjectType
{
	POT_SPHERE,
	POT_AABB,
	POT_BODY,
	POT_BOX
};


class MPBPhysicsObject: public MPB3DObject
{
public:

	MPBPhysicsObject( PhysicsObjectType t );
//	MPBPhysicsObject( MPBPhysicsObject* copyFrom );
	virtual ~MPBPhysicsObject();

	virtual void updateFrame();
	virtual void handleCollision1( Collision* c ) {}	// called before collision response
	virtual void handleCollision2( Collision* c ) {}	// called after collision response
	virtual void setOwner( MPBPhysicsObject* owner, MPBVector ownerOffset );
	virtual void setOwner( MPBRigidBody* owner, MPBVector ownerOffset );
	virtual void setOwner( MPBPhysicsParticle* owner, MPBVector ownerOffset );
	virtual void setPosition( MPBVector pos );
	virtual void setRadius( MPBFLOAT s );
	virtual void setRadiusVector( MPBVector v );
	virtual void setMass( MPBFLOAT s );
	virtual void setCollisionTest( bool set ) { m_collisionTest = set; }
	virtual void setPlaneCollisionTest( bool set ) { m_planeCollisionTest = set; }
	virtual void setCollidable( bool set ) { m_collidable = set; }
	virtual bool getCollidable() { return m_collidable; }
	virtual void setDraw( bool set ) { m_draw = set; }
	virtual void constrain( int type ) { m_constraints = type; }
	virtual void setElasticity( MPBFLOAT d ) { m_elasticity = d; }
//	virtual void draw2D( HDC& hdc );
	virtual void draw( bool shadow, Plane* shadowReceiver, MPBVector vectorToSun, bool forShadowVolume );
	virtual void applyTorque( const MPBVector& force, const MPBVector& pointOfContact );
	virtual void applyTorqueImpulse( const MPBVector& force, const MPBVector& pointOfContact );
	virtual MPBVector getOrientationVector() const { return m_orientationVector; }
	MPBQuaternion getOrientationQuaternion() const { return m_orientationQuaternion; }
	void setOrientationQuaternion( MPBQuaternion q ) { m_orientationQuaternion = q; updateOrientationVector(); }
	void setRotationMatrix( MPBMatrix& m );
	virtual MPBFLOAT getRadius() const { return m_radius; }
	virtual MPBVector getRadiusVector() const { return m_radiusVector; }
	virtual void rotate( const MPBVector& rotation );
	virtual void rotateToX( MPBFLOAT radians );
	virtual void rotateToY( MPBFLOAT radians );
	virtual void rotateToZ( MPBFLOAT radians );
	virtual void move( const MPBVector& moveVector, bool testCollisions );
	virtual void setFreeRotate( bool set ) { m_freeRotate = set; }
	void turnToFaceNowX( MPBVector target );
	void turnToFaceNowY( MPBVector target );
	virtual MPBVector getAxisVector( DirectionType direction ) const;
	bool isAlive() const { return m_alive; }
	void setAlive( bool b ) { m_alive = b; }
	MPBFLOAT getInertia() const { return m_inertia; }
	void setLateralFriction( bool s ) { m_lateralFriction = s; }
	virtual void setDamage( MPBFLOAT s ) { m_damage = s; }
	virtual void addDamage( MPBFLOAT d );
	MPBFLOAT getDamage() { return m_damage; }
	void setCastsShadow( bool s ) { m_castsShadow = s; }
	bool getCastsShadow() { return m_castsShadow; }
	void setImmovable( bool s ) { m_immovable = s; }
	bool getImmovable() { return m_immovable; }
	void setLift( MPBVector v ) { m_lift = v; }
	MPBVector getLift() { return m_lift; }
	void setAirDensity( MPBFLOAT d ) { m_airDensity = d; }
	MPBFLOAT getAirDensity() { return m_airDensity; }
	void drawShadowVolume( const MPBVector& lightPos );
	void createShadowVolume( MPBVector& lightPos );
	void setTexture( const char* texture ) { strcpy( m_texture, texture ); }
	bool pathIsClear( MPBVector to, MPBFLOAT radius, bool testObjects, MPB3DObject* target );
	void setRollingResistance( MPBFLOAT d ) { m_rollingResistance = d; }

protected:

	bool									odeCollide(
												dGeomID obstacleGeomID, 
												dBodyID obstacleBodyID, 
												MPBFLOAT obstacleElasticity,
												MPBPhysicsObject* obstaclePO,		// can be NULL
												MPBShape* obstacleShape				// can be NULL
											);

	dGeomID								m_geomID;
	PhysicsObjectType					m_poType;
	MPBPhysicsObject*					m_owner;							// this object will follow its owner
	MPBPhysicsParticle*				m_ownerPP;
	MPBRigidBody*						m_ownerRB;
	MPBVector							m_ownerOffset;
	MPBFLOAT 							m_inertia;
	MPBFLOAT 							m_radius;						// in meters
	MPBVector							m_radiusVector;				// x,y,z in meters

	MPBVector							m_orientationVector;			// stays normalized
	MPBQuaternion						m_orientationQuaternion;
	MPBVector							m_eulerRotation;				// in radians; only used when freeRotate is true
	bool									m_freeRotate;

	MPBVector							m_localVelocity;				// in meters per second (local coords)
	MPBVector							m_angularVelocity;			// in radians per second (object coords)
	MPBVector							m_angularAcceleration;		// in radians per second squared (object coords)
	bool									m_collisionTest;
	bool									m_planeCollisionTest;
	bool									m_collidable;
	bool									m_castsShadow;
	int									m_collisionTestCalls;
	char									m_texture[MAX_PATH];
	MPBFLOAT 							m_elasticity;					// 0 - 1.0
	MPBFLOAT 							m_rollingResistance;
	bool									m_draw;
	int									m_constraints;
	bool									m_alive;
	bool									m_lateralFriction;
	bool									m_isParticleSystem;
	MPBAABB								m_aabb;
	MPBFLOAT 							m_damage;
	MPBVector							m_lift;
	MPBFLOAT 							m_airDensity;
	bool									m_immovable;
	Plane									m_aabbPlanes[6];
	MPBVectorList						m_vertices;
	MPBShape*							m_shadowVolumeShape;


	virtual void collisionTest();
	virtual void updateOrientationVector();
	void die() { m_alive = false; }
	void setupAABBPlanes();

	friend class MPBPhysics;
};



#endif

