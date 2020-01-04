
#include "MPBGlobal.h"
#include "MPBAnimation.h"
#include "MPBShape.h"
#include "mmgr.h"



MPBAnimation::MPBAnimation():
	m_loop( true ),
	m_started( false ),
	m_paused( false ),
	m_startTime( 0 ),
	m_pauseTime( 0 ),
	m_animateTexCoords0( false ),
	m_animateTexCoords1( false ),
	m_pauseWhenDone( false )
{
}

MPBAnimation::~MPBAnimation()
{
	MPB_LIST_DELETE( m_keyFrames, MPBKeyFrame* );
}

void MPBAnimation::start()
{
	if (!m_started)
	{
		m_started = true;
		m_paused = false;
		m_startTime = MPBFrameCounter::totalSecondsElapsed;
	}
}

void MPBAnimation::pause()
{
	if (m_started && !m_paused)
	{
		m_paused = true;
		m_pauseTime = MPBFrameCounter::totalSecondsElapsed;
	}
}

void MPBAnimation::pauseWhenDone()
{
	m_pauseWhenDone = true;
}

void MPBAnimation::unpause()
{
	if (m_paused)
	{
		m_paused = false;
		m_startTime = (MPBFrameCounter::totalSecondsElapsed - (m_pauseTime - m_startTime));
	}

	m_pauseWhenDone = false;
}

void MPBAnimation::stop()
{
	if (m_started)
	{
		m_started = false;
		m_paused = false;
		m_startTime = 0;
		if (m_loop) start();
	}
}

// returns seconds since animation was started
MPBFLOAT MPBAnimation::getDeltaTime() const 
{ 
	MPB_ASSERT( m_started ); 

	if (m_paused) return m_pauseTime - m_startTime; 

	return MPBFrameCounter::totalSecondsElapsed - m_startTime; 
}

void MPBAnimation::updateFrame( MPBShape* shape )
{
	MPB_ASSERT( m_keyFrames.size() >= 2 );

	MPBFLOAT dt = getDeltaTime();

	// stop if we've reached the last keyframe
	MPBKeyFrame* lastKF = (MPBKeyFrame*)m_keyFrames[m_keyFrames.size()-1];

	if (dt >= lastKF->m_scaledTime)
	{
		shape->rotateXAnimation( lastKF->m_rotation.x );
		shape->rotateYAnimation( lastKF->m_rotation.y );
		shape->rotateZAnimation( lastKF->m_rotation.z );

		shape->translateAnimation(	lastKF->m_translation.x,
											lastKF->m_translation.y,
											lastKF->m_translation.z	);

		if (m_animateTexCoords0)
		{
			shape->setTexCoords(	0,
										lastKF->m_texCoordsLL0,
										lastKF->m_texCoordsLR0,
										lastKF->m_texCoordsUR0,
										lastKF->m_texCoordsUL0	);
		}

		if (m_animateTexCoords1)
		{
			shape->setTexCoords(	1,
										lastKF->m_texCoordsLL1,
										lastKF->m_texCoordsLR1,
										lastKF->m_texCoordsUR1,
										lastKF->m_texCoordsUL1	);
		}

		if (m_pauseWhenDone)
			pause();
		else
			stop();

		return;
	}

	// find the keyframe we just passed
	for (int i=m_keyFrames.size()-2; i>=0; i--)
	{
		MPBKeyFrame* kf = (MPBKeyFrame*)m_keyFrames[i];
		MPBKeyFrame* kf2 = (MPBKeyFrame*)m_keyFrames[i+1];

		MPBFLOAT keyFrameTime = kf->m_scaledTime;
		MPBFLOAT nextKeyFrameTime = kf2->m_scaledTime;

		if (dt >= keyFrameTime)
		{
			// x represents where we are between this frame and the next frame (0.0 - 1.0)
			MPBFLOAT x = (dt - keyFrameTime) / (nextKeyFrameTime - keyFrameTime);

//			if (floatEQ( kf->m_rotation.x, kf2->m_rotation.x ))
//				shape->rotateXAnimation( kf->m_rotation.x );
//			else
				shape->rotateXAnimation( lerp( x, kf->m_rotation.x, kf2->m_rotation.x ) );

//			if (floatEQ( kf->m_rotation.y, kf2->m_rotation.y ))
//				shape->rotateYAnimation( kf->m_rotation.y );
//			else
				shape->rotateYAnimation( lerp( x, kf->m_rotation.y, kf2->m_rotation.y ) );

//			if (floatEQ( kf->m_rotation.z, kf2->m_rotation.z ))
//				shape->rotateZAnimation( kf->m_rotation.z );
//			else
				shape->rotateZAnimation( lerp( x, kf->m_rotation.z, kf2->m_rotation.z ) );

//			if (kf->m_translation == kf2->m_translation)
//				shape->translateAnimation(	kf->m_translation.x,
//													kf->m_translation.y,
//													kf->m_translation.z	);
//			else
				shape->translateAnimation(	lerp( x, kf->m_translation.x, kf2->m_translation.x ),
													lerp( x, kf->m_translation.y, kf2->m_translation.y ),
													lerp( x, kf->m_translation.z, kf2->m_translation.z )	);

			if (m_animateTexCoords0)
			{
				shape->setTexCoords(	
					0,
					MPBVector( 
						lerp( x, kf->m_texCoordsLL0.x, kf2->m_texCoordsLL0.x ),
						lerp( x, kf->m_texCoordsLL0.y, kf2->m_texCoordsLL0.y ),
						lerp( x, kf->m_texCoordsLL0.z, kf2->m_texCoordsLL0.z )	),
					MPBVector( 
						lerp( x, kf->m_texCoordsLR0.x, kf2->m_texCoordsLR0.x ),
						lerp( x, kf->m_texCoordsLR0.y, kf2->m_texCoordsLR0.y ),
						lerp( x, kf->m_texCoordsLR0.z, kf2->m_texCoordsLR0.z )	),
					MPBVector( 
						lerp( x, kf->m_texCoordsUR0.x, kf2->m_texCoordsUR0.x ),
						lerp( x, kf->m_texCoordsUR0.y, kf2->m_texCoordsUR0.y ),
						lerp( x, kf->m_texCoordsUR0.z, kf2->m_texCoordsUR0.z )	),
					MPBVector( 
						lerp( x, kf->m_texCoordsUL0.x, kf2->m_texCoordsUL0.x ),
						lerp( x, kf->m_texCoordsUL0.y, kf2->m_texCoordsUL0.y ),
						lerp( x, kf->m_texCoordsUL0.z, kf2->m_texCoordsUL0.z )	)
				);
			}

			if (m_animateTexCoords1)
			{
				shape->setTexCoords(	
					1,
					MPBVector( 
						lerp( x, kf->m_texCoordsLL1.x, kf2->m_texCoordsLL1.x ),
						lerp( x, kf->m_texCoordsLL1.y, kf2->m_texCoordsLL1.y ),
						lerp( x, kf->m_texCoordsLL1.z, kf2->m_texCoordsLL1.z )	),
					MPBVector( 
						lerp( x, kf->m_texCoordsLR1.x, kf2->m_texCoordsLR1.x ),
						lerp( x, kf->m_texCoordsLR1.y, kf2->m_texCoordsLR1.y ),
						lerp( x, kf->m_texCoordsLR1.z, kf2->m_texCoordsLR1.z )	),
					MPBVector( 
						lerp( x, kf->m_texCoordsUR1.x, kf2->m_texCoordsUR1.x ),
						lerp( x, kf->m_texCoordsUR1.y, kf2->m_texCoordsUR1.y ),
						lerp( x, kf->m_texCoordsUR1.z, kf2->m_texCoordsUR1.z )	),
					MPBVector( 
						lerp( x, kf->m_texCoordsUL1.x, kf2->m_texCoordsUL1.x ),
						lerp( x, kf->m_texCoordsUL1.y, kf2->m_texCoordsUL1.y ),
						lerp( x, kf->m_texCoordsUL1.z, kf2->m_texCoordsUL1.z )	)
				);
			}

			break;
		}
	}
}

void MPBAnimation::scaleTime( MPBFLOAT factor )
{
	MPB_LIST_LOOP( m_keyFrames, kf, MPBKeyFrame* )
	{
		kf->m_scaledTime = kf->m_time * factor;
	}
}

