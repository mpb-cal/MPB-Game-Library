
#include "MPBGlobal.h"
#include "MPBParticle.h"
#include "mmgr.h"

MPBParticle::MPBParticle()
{
}

void MPBParticle::init(	const MPBVector& position,
								const MPBVector& velocity,
								const MPBVector& acceleration,
								const MPBVector& color,
								MPBFLOAT radius,
								MPBFLOAT	rotation,		// degrees
								MPBFLOAT	rotationRate,	// degrees per frame
								MPBFLOAT life,				// 0.0 - 1.0; particle dies/respawns when 0
																// also used for alpha value
								MPBFLOAT fadeRate,			// amt to subtract from life each frame
								MPBFLOAT fadeColorRate,	// amt to subtract from color each frame
								MPBFLOAT growRate	)
{
//	active = true;								// Make All The Particles Active
	m_radius = radius;
	m_life = life;
	m_rotation = rotation;
	m_rotationRate = rotationRate;

//	fade = float(rand()%100) / 1000.0f + 0.003f;	// Random Fade Speed
	m_fadeRate = fadeRate;
	m_fadeColorRate = fadeColorRate;

	m_position = position;
	m_velocity = velocity;
	m_acceleration = acceleration;
	m_color = color;
	m_growRate = growRate;
}

void MPBParticle::update()
{
	m_radius += m_radius * m_growRate;
	m_position = m_position + m_velocity;
	m_velocity = m_velocity + m_acceleration;
	m_life -= m_fadeRate;
	m_color.x = m_color.x - m_fadeColorRate;
	m_color.y = m_color.y - m_fadeColorRate;
	m_color.z = m_color.z - m_fadeColorRate;
	m_rotation += m_rotationRate;
}

