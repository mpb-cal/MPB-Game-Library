
#include "MPBGlobal.h"
#include "MPBOpenGL.h"
#include "MPBParticleSystem.h"
#include "MPBParticle.h"
#include "MPBPhysics.h"
#include "MPBFrameCounter.h"
#include "MPBCountdown.h"
#include "mmgr.h"


MPBParticleSystem::MPBParticleSystem( int numParticles ) :
	MPBPhysicsObject( POT_SPHERE ),
	m_numParticles( numParticles ),
	m_particles( NULL ),
	m_countdownParticle( NULL ),
	m_particleTubeBackfire( false ),
	m_particleTube( false ),
	m_particleCone( false ),
	m_addVelocityToParticle( true ),
	m_particleTubeRadialOffsets( NULL ),
	m_useLines( false ),
	m_dieWhenDoneEmitting( false ),
	m_drawSolid( false )
{
	m_isParticleSystem = true;
	strcpy( m_particleTexture, "" );
}

MPBParticleSystem::~MPBParticleSystem()
{
	NULLIFY( m_countdownParticle );
	NULLIFY_ARRAY( m_particles );
	NULLIFY_ARRAY( m_particleTubeRadialOffsets );
}

// can be overridden by children to
// set behavior of particle at spawn-time
void MPBParticleSystem::getParticleStartPositionAndDirection(	int i,
																					MPBVector& startPosition,
																					MPBVector& direction	)
{
//	if (!isActive()) return;

	MPBFLOAT secondsElapsed = max( MPBFrameCounter::secondsElapsed, .01 );

	MPBVector offset = m_offset;

	startPosition = startPosition + offset;

	// find particle direction (unit vector)
	if (m_particleTube)
	{
		if (m_particleTubeBackfire)
		{
			direction = getVelocity() * secondsElapsed * -1;
		}
		else
		{
			direction = m_particleTubeDirection;
		}

		startPosition = startPosition + m_particleTubeRadialOffsets[rand() % m_numParticles];
	}
	else if (m_particleCone)
	{
		direction = m_particleTubeDirection;

		direction.rotateX( m_particleConeDegrees * RANDOM_PERCENTAGE - m_particleConeDegrees/2 );
		direction.rotateY( m_particleConeDegrees * RANDOM_PERCENTAGE - m_particleConeDegrees/2 );
		direction.rotateZ( m_particleConeDegrees * RANDOM_PERCENTAGE - m_particleConeDegrees/2 );
	}
	else
	{
		// shoot particles in random direction
		direction.set( 0, 0, -1 );
		direction.rotateX( (float)(rand() % 360) );
		direction.rotateY( (float)(rand() % 360) );
		direction.rotateZ( (float)(rand() % 360) );
	}
}

// create a new particle
void MPBParticleSystem::initParticle( int i, bool burst )
{
//	if (!isActive()) return;

	MPBFLOAT secondsElapsed = max( MPBFrameCounter::secondsElapsed, .01 );

	MPBVector startPosition = getPosition();
	MPBVector particleVelocity;
	getParticleStartPositionAndDirection( i, startPosition, particleVelocity );

	// multiply particle velocity by speed
	particleVelocity = 
		particleVelocity * (m_particleSpeed + m_particleSpeed * (.4f * RANDOM_PERCENTAGE - .2f));

	// add robot's velocity to particle velocity
	if (m_addVelocityToParticle) particleVelocity = particleVelocity + getVelocity();
	particleVelocity = particleVelocity * secondsElapsed;

	MPBFLOAT life;
	if (burst) life = 1.0f;
	else life = 1.0f * RANDOM_PERCENTAGE;

//	MPBFLOAT fade = .03 + .01 * RANDOM_PERCENTAGE;
//	MPBFLOAT fade = m_particleFadeRate;// + m_particleFadeRate * (.4 * RANDOM_PERCENTAGE - .2)

	m_particles[i].init(	startPosition,
								particleVelocity,
								m_particleAcceleration * secondsElapsed,
								m_particleColor,
								m_particleRadius,
								360 * RANDOM_FRACTION,
								m_particleRotationRate,
								life,
								m_particleFadeRate * secondsElapsed,
								m_particleColorFadeRate * secondsElapsed,
								m_particleGrowRate * secondsElapsed	);
}

void MPBParticleSystem::startEmittingParticlesCone(	int timeout, 
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
																		MPBFLOAT degrees	)
{
//	if (!isActive()) return;

	m_particleTubeDirection = direction;
	m_particleTubeDirection.normalize();
	m_particleCone = true;
	m_particleConeDegrees = degrees;
	m_particleTubeBackfire = backfire;

	startEmittingParticles(	timeout, 
									dieWhenDone, 
									burst, 
									addVelocityToParticle,
									speed,
									fadeRate,
									colorFadeRate,
									growRate,
									acceleration,
									color,
									particleRadius,
									particleRotationRate,
									useLines	);
}
/*
// direction is ignored if backfire = true
void MPBParticleSystem::startEmittingParticlesTube(	int timeout, 
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
																		bool useLines,
																		bool backfire,
																		const MPBVector& direction,
																		MPBFLOAT tubeRadius	)
{
//	if (!isActive()) return;

	m_particleTubeDirection = direction;
	m_particleTubeDirection.normalize();
	m_particleTubeRadialOffsets = new MPBVector[m_numParticles];

	for (int i=0; i<m_numParticles; i++)
	{
		m_particleTubeRadialOffsets[i] = 
			m_particleTubeDirection.crossProduct( getOrientationVector() ) * tubeRadius * RANDOM_FRACTION;

		MPBQuaternion q;
		q.fromAxisAngle( m_particleTubeDirection, _2PI * RANDOM_FRACTION );
		q.rotateVector( m_particleTubeRadialOffsets[i] );
	}

	m_particleTubeRadius = tubeRadius;
	m_particleTube = true;
	m_particleTubeBackfire = backfire;

	startEmittingParticles(	timeout, 
									dieWhenDone, 
									burst, 
									addVelocityToParticle,
									speed,
									fadeRate,
									colorFadeRate,
									growRate,
									acceleration,
									color,
									particleRadius,
									useLines	);
}
*/
// if timeout == -1, stay alive indefinitely
// don't call until at least 1 seconds has elapsed
void MPBParticleSystem::startEmittingParticles(	int timeout, 
																bool dieWhenDone, 
																bool burst, 
																bool addVelocityToParticle,
																MPBFLOAT speed,							// in meters per second
																MPBFLOAT fadeRate,						// percentage per second
																MPBFLOAT colorFadeRate,				// percentage per second
																MPBFLOAT growRate,						// percentage per second
																const MPBVector& acceleration,	// meters squared per second
																const MPBVector& color,
																MPBFLOAT particleRadius,
																MPBFLOAT particleRotationRate,
																bool useLines	)
{
//	MPB_ASSERT( MPBFrameCounter::totalSecondsElapsed > .01 );

//	if (!isActive()) return;

//	m_particleUseVelocity = useVelocity;
	m_stopEmitting = false;
	m_useLines = useLines;

	m_particleRadius = particleRadius;
	m_particleRotationRate = particleRotationRate;
	m_particleAcceleration = acceleration;
	m_particleColor = color;
	m_particleSpeed = speed;
	m_particleFadeRate = fadeRate / 100.0f;
	m_particleColorFadeRate = colorFadeRate / 100.0f;
	m_particleGrowRate = growRate / 100.0f;
	m_addVelocityToParticle = addVelocityToParticle;

	if (timeout != -1)
	{
		m_countdownParticle = new MPBCountdown( timeout, false );
	}

	m_dieWhenDoneEmitting = dieWhenDone;

	NULLIFY_ARRAY( m_particles );
	m_particles = new MPBParticle[ m_numParticles ];
	MPB_ASSERT( m_particles );

	for (int i=0; i<m_numParticles; i++)
	{
		initParticle( i, burst );
	}
}

void MPBParticleSystem::stopEmittingParticles()
{
	m_stopEmitting = true;
}

void MPBParticleSystem::updateFrame()
{
	MPBPhysicsObject::updateFrame();

//	if (!isActive()) return;
	if (m_particles == NULL) return;

//	m_particleRadius += m_particleRadius * m_particleGrowRate * MPBFrameCounter::secondsElapsed;

	bool allDead = true;

	for (int i=0; i<m_numParticles; i++)
	{
		m_particles[i].update();

		if (m_particles[i].m_life < 0.0)
		{
			if (	!m_stopEmitting && 
					!m_dieWhenDoneEmitting && 
					(m_countdownParticle == NULL || !m_countdownParticle->isDone())	)
			{
				initParticle( i, false );
			}
		}
		else
		{
			allDead = false;
		}
	}

	if (allDead && m_dieWhenDoneEmitting)
	{
		die();
	}
}

void MPBParticleSystem::draw( bool shadow, bool solid, Plane* shadowReceiver, MPBVector vectorToSun )
{
	if (!m_draw) return;

	if (shadow)
	{
		MPBPhysicsObject::draw( shadow, shadowReceiver, vectorToSun, false );
		return;
	}
	else if (solid)
	{
		if (m_drawSolid)
			MPBPhysicsObject::draw( shadow, shadowReceiver, vectorToSun, false );
		return;
	}

//	if (!isActive()) return;
	if (m_particles == NULL) return;

	glPushAttrib( GL_DEPTH_BUFFER_BIT | GL_LIGHTING_BIT );

	glDepthMask( GL_FALSE );
	glEnable( GL_COLOR_MATERIAL );
	glColorMaterial( GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE );
	glDisable( GL_LIGHTING );
	glDisable( GL_CULL_FACE );
	glEnable( GL_BLEND );
	glBlendFunc( GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA );

	if (0)
	{
		glPointSize( m_particleRadius * 100 );
		glDisable( GL_TEXTURE_1D );
		glDisable( GL_TEXTURE_2D );
		glDisable( GL_TEXTURE_3D );

		glBegin( GL_POINTS );

		for (int i=0; i<m_numParticles; i++)
		{
			glColor4f(	m_particles[i].m_color.x,
							m_particles[i].m_color.y,
							m_particles[i].m_color.z,
							m_particles[i].m_life	);

			glVertex3d(	m_particles[i].m_position.x, 
							m_particles[i].m_position.y, 
							m_particles[i].m_position.z	);
		}

		glEnd();
	}
	else if (m_useLines)
	{
		glLineWidth( 1 );
		glDisable( GL_TEXTURE_1D );
		glDisable( GL_TEXTURE_2D );
		glDisable( GL_TEXTURE_3D );

		glBegin( GL_LINES );

		for (int i=0; i<m_numParticles; i++)
		{
			glColor4f(	m_particles[i].m_color.x,
							m_particles[i].m_color.y,
							m_particles[i].m_color.z,
							m_particles[i].m_life	);

			glVertex3d(	m_particles[i].m_position.x, 
							m_particles[i].m_position.y, 
							m_particles[i].m_position.z	);

			glVertex3d(	m_particles[i].m_position.x, 
							m_particles[i].m_position.y - m_particleRadius * 2, 
							m_particles[i].m_position.z	);
		}

		glEnd();
	}
	else if (0)
	{
		glPushMatrix();
		glTranslated( getPosition().x, getPosition().y, getPosition().z );
		gluSphere( MPBOpenGL::quadricObj, m_particleRadius, 10, 10 );
		glPopMatrix();
	}
	else if (1)
	{
		GLfloat cameraMatrix[16];
		glGetFloatv( GL_MODELVIEW_MATRIX, cameraMatrix );
		MPBVector cameraXAxis( cameraMatrix[0], cameraMatrix[4], cameraMatrix[8] );
		MPBVector cameraYAxis( cameraMatrix[1], cameraMatrix[5], cameraMatrix[9] );
		MPBVector cameraZAxis( cameraMatrix[2], cameraMatrix[6], cameraMatrix[10] );

		if (!strcmp( m_particleTexture, "" ))
			MPBOpenGL::bindParticleTexture();
		else
			MPBOpenGL::bindTexture( m_particleTexture );

		if (0)
		{
			for (int i=0; i<m_numParticles; i++)
			{
				MPBVector offset1 = (cameraXAxis * -1 + cameraYAxis * -1) * m_particles[i].m_radius;
				MPBVector offset2 = (cameraXAxis *  1 + cameraYAxis * -1) * m_particles[i].m_radius;
				MPBVector offset3 = (cameraXAxis *  1 + cameraYAxis *  1) * m_particles[i].m_radius;
				MPBVector offset4 = (cameraXAxis * -1 + cameraYAxis *  1) * m_particles[i].m_radius;

				glBegin( GL_TRIANGLE_FAN );

				glColor4f(	m_particles[i].m_color.x,
								m_particles[i].m_color.y,
								m_particles[i].m_color.z,
								m_particles[i].m_life	);

				glTexCoord2d( .5, .5 ); 
				glVertex3d(	m_particles[i].m_position.x, 
								m_particles[i].m_position.y, 
								m_particles[i].m_position.z	);

				glColor4f(	m_particles[i].m_color.x,
								m_particles[i].m_color.y,
								m_particles[i].m_color.z,
								0);//m_particles[i].m_life	);

				glTexCoord2d( 0, 1 ); 
				glVertex3d(	m_particles[i].m_position.x + offset4.x, 
								m_particles[i].m_position.y + offset4.y, 
								m_particles[i].m_position.z + offset4.z	);

				glTexCoord2d( 1, 1 ); 
				glVertex3d(	m_particles[i].m_position.x + offset3.x, 
								m_particles[i].m_position.y + offset3.y, 
								m_particles[i].m_position.z + offset3.z	);

				glTexCoord2d( 1, 0 ); 
				glVertex3d(	m_particles[i].m_position.x + offset2.x, 
								m_particles[i].m_position.y + offset2.y, 
								m_particles[i].m_position.z + offset2.z	);

				glTexCoord2d( 0, 0 ); 
				glVertex3d(	m_particles[i].m_position.x + offset1.x, 
								m_particles[i].m_position.y + offset1.y, 
								m_particles[i].m_position.z + offset1.z	);

				glTexCoord2d( 0, 1 ); 
				glVertex3d(	m_particles[i].m_position.x + offset4.x, 
								m_particles[i].m_position.y + offset4.y, 
								m_particles[i].m_position.z + offset4.z	);

				glEnd();
			}
		}
		else if (1)
		{
//			glBegin( GL_QUADS );

			for (int i=0; i<m_numParticles; i++)
			{
				MPBVector offset1 = (cameraXAxis * -1 + cameraYAxis * -1) * m_particles[i].m_radius;
				MPBVector offset2 = (cameraXAxis *  1 + cameraYAxis * -1) * m_particles[i].m_radius;
				MPBVector offset3 = (cameraXAxis *  1 + cameraYAxis *  1) * m_particles[i].m_radius;
				MPBVector offset4 = (cameraXAxis * -1 + cameraYAxis *  1) * m_particles[i].m_radius;

				glColor4f(	m_particles[i].m_color.x,
								m_particles[i].m_color.y,
								m_particles[i].m_color.z,
								m_particles[i].m_life	);

				glPushMatrix();
				glTranslatef(	m_particles[i].m_position.x, 
									m_particles[i].m_position.y, 
									m_particles[i].m_position.z );
				glRotatef( m_particles[i].m_rotation, cameraZAxis.x, cameraZAxis.y, cameraZAxis.z );

				glBegin( GL_QUADS );

				glTexCoord2d( .01, .01 ); 
				glVertex3d(	offset1.x, offset1.y, offset1.z );

				glTexCoord2d( .99, .01 ); 
				glVertex3d(	offset2.x, offset2.y, offset2.z );

				glTexCoord2d( .99, .99 ); 
				glVertex3d(	offset3.x, offset3.y, offset3.z );

				glTexCoord2d( .01, .99 ); 
				glVertex3d(	offset4.x, offset4.y, offset4.z );

				glEnd();
				glPopMatrix();
			}

//			glEnd();
		}
	}

	glEnable( GL_CULL_FACE );
	glDisable( GL_BLEND );
	glPopAttrib();
}




