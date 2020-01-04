#ifndef __PARTICLESYSTEM_H__
#define __PARTICLESYSTEM_H__

#include "MPBVector.h"
#include "MPBPhysicsObject.h"
#include "MPBPhysics.h"


class MPBCountdown;
class MPBParticle;

class MPBParticleSystem: public MPBPhysicsObject
{
public:

	MPBParticleSystem( int numParticles );
	virtual ~MPBParticleSystem();
	void setOffset( MPBVector o ) { m_offset = o; }
	virtual void draw( bool shadow, bool solid, Plane* shadowReceiver, MPBVector vectorToSun );
	void drawParticles() const;
	virtual void updateFrame();
	void startEmittingParticles(	int timeout, 
											bool dieWhenDone, 
											bool burst, 
											bool addVelocityToParticle,
											MPBFLOAT speed,
											MPBFLOAT fadeRate,
											MPBFLOAT colorFadeRate,
											MPBFLOAT growRate,
											const MPBVector& acceleration,
											const MPBVector& color,
											MPBFLOAT particleRadius,
											MPBFLOAT particleRotationRate,
											bool useLines	);
	void startEmittingParticlesTube(	int timeout, 
												bool dieWhenDone, 
												bool burst, 
												bool addVelocityToParticle,
												MPBFLOAT speed,
												MPBFLOAT fadeRate,
												MPBFLOAT colorFadeRate,
												MPBFLOAT growRate,
												const MPBVector& acceleration,
												const MPBVector& color,
												MPBFLOAT particleRadius,
												MPBFLOAT particleRotationRate,
												bool useLines,
												bool backfire,
												const MPBVector& direction,
												MPBFLOAT tubeRadius	);
	void startEmittingParticlesCone(	int timeout, 
												bool dieWhenDone, 
												bool burst, 
												bool addVelocityToParticle,
												MPBFLOAT speed,
												MPBFLOAT fadeRate,
												MPBFLOAT colorFadeRate,
												MPBFLOAT growRate,
												const MPBVector& acceleration,
												const MPBVector& color,
												MPBFLOAT particleRadius,
												MPBFLOAT particleRotationRate,
												bool useLines,
												bool backfire,
												const MPBVector& direction,
												MPBFLOAT degrees	);
	void stopEmittingParticles();
	void 	setDieWhenDoneEmitting( bool s ) { m_dieWhenDoneEmitting = s; }
	void initParticle( int i, bool burst );
	virtual void getParticleStartPositionAndDirection(	int i,
																		MPBVector& startPosition,
																		MPBVector& direction	);
	void setDrawSolid( bool s ) { m_drawSolid = s; }
	void setParticleTexture( const char* texture ) { strcpy( m_particleTexture, texture ); }

protected:

private:

	int							m_numParticles;
	MPBVector					m_offset;
	MPBParticle*				m_particles;
	MPBCountdown*				m_countdownParticle;
	bool							m_useLines;
	bool							m_dieWhenDoneEmitting;
	bool							m_stopEmitting;
	bool							m_addVelocityToParticle;
	MPBFLOAT 					m_particleSpeed;
	MPBVector					m_particleAcceleration;
	MPBVector					m_particleColor;
	MPBVector					m_particleTubeDirection;
	MPBVector*					m_particleTubeRadialOffsets;
	MPBFLOAT 					m_particleTubeRadius;
	bool							m_particleTube;
	bool							m_particleCone;
	bool							m_particleUseVelocity;
	bool							m_particleTubeBackfire;
	MPBFLOAT 					m_particleFadeRate;
	MPBFLOAT 					m_particleColorFadeRate;
	MPBFLOAT 					m_particleGrowRate;
	MPBFLOAT 					m_particleConeDegrees;
	MPBFLOAT 					m_particleRadius;
	MPBFLOAT 					m_particleRotationRate;
	bool							m_drawSolid;
	char							m_particleTexture[MAX_PATH];
};

#endif


