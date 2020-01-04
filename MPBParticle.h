#ifndef __PARTICLE_H__
#define __PARTICLE_H__

class MPBVector;

class MPBParticle
{
public:

	MPBParticle();
	void init(	const MPBVector& position,
					const MPBVector& velocity,
					const MPBVector& acceleration,
					const MPBVector& color,
					MPBFLOAT radius,
					MPBFLOAT	rotation,
					MPBFLOAT	rotationRate,
					MPBFLOAT life,
					MPBFLOAT fadeRate,
					MPBFLOAT fadeColorRate,
					MPBFLOAT growRate	);
	void update();

//	bool	active;					// Active (Yes/No)
	MPBFLOAT 		m_radius;
	MPBFLOAT 		m_life;
	MPBFLOAT 		m_fadeRate;
	MPBFLOAT 		m_fadeColorRate;
	MPBFLOAT 		m_growRate;
	MPBFLOAT			m_rotation;
	MPBFLOAT			m_rotationRate;
	MPBVector		m_position;
	MPBVector		m_velocity;
	MPBVector		m_acceleration;
	MPBVector		m_color;
};


#endif

