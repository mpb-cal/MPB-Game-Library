
#include "MPBCreature.h"
#include "MPBShapeBox.h"
#include "MPBShapeBone.h"
#include "MPBShapeTube.h"
#include "MPBAnimation.h"
#include "mmgr.h"


const MPBFLOAT LOOK_MAX_UPDOWN = PI / 3;


MPBCreature::MPBCreature( MPBVector radiusVector ):
	MPBPhysicsObject( POT_AABB )
{
	setRadiusVector( radiusVector );
	setFreeRotate( true );
	updateLookVector();
}

MPBCreature::~MPBCreature()
{
}

void MPBCreature::updateFrame()
{
	MPBPhysicsObject::updateFrame();

	MPBFLOAT xRot = m_desiredLookRotation.x - m_lookRotation.x;
	MPBFLOAT yRot = m_desiredLookRotation.y - m_lookRotation.y;
	MPBFLOAT zRot = m_desiredLookRotation.z - m_lookRotation.z;

	if (xRot > PI) xRot = -_2PI + xRot;
	if (xRot < -PI) xRot = _2PI + xRot;

	if (yRot > PI) yRot = -_2PI + yRot;
	if (yRot < -PI) yRot = _2PI + yRot;

	if (zRot > PI) zRot = -_2PI + zRot;
	if (zRot < -PI) zRot = _2PI + zRot;

	rotateLook( MPBVector( xRot, yRot, zRot ) / 10.0 );
}

MPBVector MPBCreature::getEyeball() const
{
	return getPosition() + MPBVector( 0, 4.0 * getRadius() / 5.0, 0 );
}

// in radians
void MPBCreature::setDesiredLookRotation( MPBVector rotation )
{
	m_desiredLookRotation = rotation;
}

// in radians
void MPBCreature::setLookRotation( MPBVector rotation )
{
	m_lookRotation = rotation;
	if (m_lookRotation.x > LOOK_MAX_UPDOWN) m_lookRotation.x = LOOK_MAX_UPDOWN;
	if (m_lookRotation.x < -LOOK_MAX_UPDOWN) m_lookRotation.x = -LOOK_MAX_UPDOWN;
	updateLookVector();

	rotateToY( rotation.y );
}

// in radians
void MPBCreature::rotateLook( MPBVector rotation )
{
	if (rotation.x || rotation.y || rotation.z)
	{
		m_lookRotation = m_lookRotation + rotation;
		if (m_lookRotation.x > LOOK_MAX_UPDOWN) m_lookRotation.x = LOOK_MAX_UPDOWN;
		if (m_lookRotation.x < -LOOK_MAX_UPDOWN) m_lookRotation.x = -LOOK_MAX_UPDOWN;
		m_desiredLookRotation = m_lookRotation;
		updateLookVector();
	}

	rotate( MPBVector( 0, rotation.y, 0 ) );
}

// call when m_lookRotation has changed
void MPBCreature::updateLookVector()
{
	m_lookVector.set( 0, 0, -1 );

	m_lookVector.rotateXRadians( m_lookRotation.x );
	m_lookVector.rotateYRadians( m_lookRotation.y );
	m_lookVector.rotateZRadians( m_lookRotation.z );
}

// in radians
void MPBCreature::setMoveRotation( MPBVector rotation )
{
	m_moveRotation = rotation;
	if (m_moveRotation.x > LOOK_MAX_UPDOWN) m_moveRotation.x = LOOK_MAX_UPDOWN;
	if (m_moveRotation.x < -LOOK_MAX_UPDOWN) m_moveRotation.x = -LOOK_MAX_UPDOWN;
	updateMoveVector();
}
// call when m_moveRotation has changed
void MPBCreature::updateMoveVector()
{
	m_moveVector.set( 0, 0, -1 );

	m_moveVector.rotateXRadians( m_moveRotation.x );
	m_moveVector.rotateYRadians( m_moveRotation.y );
	m_moveVector.rotateZRadians( m_moveRotation.z );
}





