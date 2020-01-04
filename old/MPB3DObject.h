#ifndef __MPB3DOBJECT_H__
#define __MPB3DOBJECT_H__

#include "MPBLibrary.h"
#include "MPBShape.h"
#include "MPBMatrix.h"
#include "OpenAL/al.h"
#include "MPBList.h"
#include "ode/ode.h"

enum DirectionType
{
	DIR_LEFT			= 0x01,
	DIR_RIGHT		= 0x02,
	DIR_FORWARDS	= 0x04,
	DIR_BACKWARDS	= 0x08,
	DIR_UP			= 0x10,
	DIR_DOWN			= 0x20
};


class MPBStreamingSound;

class MPB3DObject
{
public:

	MPB3DObject();
	virtual ~MPB3DObject();

	virtual void updateFrame();
	virtual void setMass( MPBFLOAT d );
	virtual void setPosition( MPBVector pos );
	virtual void setPrevPosition( MPBVector pos );
	virtual void changePosition( MPBVector v );
	virtual void setGravity( MPBFLOAT metersPerSecondSquared ) { m_gravity = metersPerSecondSquared; }
	virtual void applyForce( const MPBVector& force );
	virtual void applyImpulse( const MPBVector& force );
	virtual MPBVector getPosition() const { return m_position; }
	virtual MPBVector getPrevPosition() const { return m_prevPosition; }
	virtual MPBVector getVelocity() const { return m_velocity; }
	virtual void setVelocity( MPBVector v ) { m_velocity = v; }
	virtual void inheritVelocity( MPB3DObject* o );
	virtual void forceStop();
	virtual MPBVector getAcceleration() const { return m_acceleration; }
	virtual void addShape( MPBShape* s );
	virtual MPBFLOAT getMass() const { return m_mass; }		// in kg
	virtual void setColor( BYTE r, BYTE g, BYTE b ) { m_red = r; m_green = g; m_blue = b; }
	virtual MPBVector getMomentum() const;
	virtual MPBFLOAT getWeight() const { return m_mass * m_gravity; }		// in Newtons
	virtual MPBVector getLateralVelocity() const;
	virtual MPBVector getLongitudinalVelocity() const;
	virtual MPBVector getAxisVector( DirectionType direction ) const = 0;
	virtual bool isGrounded() const { return m_collided; }
	virtual bool pointIsPointedTo( const MPBVector& point, DirectionType moveType, MPBFLOAT thresholdRadians ) const;
	virtual bool pointIsInFront( const MPBVector& point, MPBFLOAT thresholdRadians ) const;
	virtual bool pointIsBehind( const MPBVector& point, MPBFLOAT thresholdRadians ) const;
	virtual bool pointIsToLeft( const MPBVector& point, MPBFLOAT thresholdRadians ) const;
	virtual bool pointIsToRight( const MPBVector& point, MPBFLOAT thresholdRadians ) const;
	virtual bool pointIsAbove( const MPBVector& point, MPBFLOAT thresholdRadians ) const;
	virtual bool pointIsBelow( const MPBVector& point, MPBFLOAT thresholdRadians ) const;
	virtual void setPlaneCollisionSound( char* filename ) { m_planeCollisionSound = filename; }
	virtual void setObjectCollisionSound( char* filename ) { m_objectCollisionSound = filename; }
	virtual void setInfiniteMass( bool set ) { m_ininiteMass = set; }
	void setNetworkId( unsigned short id ) { m_networkId = id; }
	unsigned short getNetworkId() { return m_networkId; }
	void setSound( char* filename );
	void stopSound();
	void setStreamingSound( MPBStreamingSound* s );
	MPBStreamingSound* getStreamingSound() { return m_streamingSound; }
	void killStreamingSound();
	dBodyID getBodyID() { return m_bodyID; }


protected:

	dBodyID								m_bodyID;
	MPBFLOAT 							m_mass;							// in kg
	MPBVector							m_position;						// in meters
	MPBVector							m_prevPosition;				// in meters
	MPBVector							m_velocity;						// in meters per second (world coords)
	MPBVector							m_acceleration;				// in meters per second squared
	MPBFLOAT 							m_gravity;
	MPBList								m_shapes;
	BYTE									m_red;
	BYTE									m_blue;
	BYTE									m_green;
	bool									m_collided;
	char*									m_planeCollisionSound;
	char*									m_objectCollisionSound;
	bool									m_ininiteMass;
	unsigned short						m_networkId;
	ALuint								m_sound;
	MPBStreamingSound*				m_streamingSound;
};




#endif

