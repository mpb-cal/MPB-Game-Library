#ifndef __MPBANIMATION_H__
#define __MPBANIMATION_H__


#include "MPBLibrary.h"
#include "MPBFrameCounter.h"
#include "MPBList.h"


class MPBShape;

// time is in seconds

class MPBKeyFrame
{
public:

	MPBKeyFrame(
		MPBFLOAT time, 
		MPBVector translation, 
		MPBVector rotation,			// in degrees
		MPBVector texCoordsLL0,
		MPBVector texCoordsLR0,
		MPBVector texCoordsUR0,
		MPBVector texCoordsUL0,
		MPBVector texCoordsLL1,
		MPBVector texCoordsLR1,
		MPBVector texCoordsUR1,
		MPBVector texCoordsUL1
	): 
		m_time( time ),
		m_scaledTime( time ),
		m_translation( translation ),
		m_rotation( rotation ),
		m_texCoordsLL0( texCoordsLL0 ),
		m_texCoordsLR0( texCoordsLR0 ),
		m_texCoordsUR0( texCoordsUR0 ),
		m_texCoordsUL0( texCoordsUL0 ),
		m_texCoordsLL1( texCoordsLL1 ),
		m_texCoordsLR1( texCoordsLR1 ),
		m_texCoordsUR1( texCoordsUR1 ),
		m_texCoordsUL1( texCoordsUL1 )
	{
	}

	MPBKeyFrame( MPBFLOAT time, MPBVector translation, MPBVector rotation ):	// in degrees
		m_time( time ),
		m_scaledTime( time ),
		m_translation( translation ),
		m_rotation( rotation )
	{
	}

	MPBFLOAT 	m_time;
	MPBFLOAT 	m_scaledTime;
	MPBVector	m_translation;
	MPBVector	m_rotation;			// in degrees
	MPBVector	m_texCoordsLL0;
	MPBVector	m_texCoordsLR0;
	MPBVector	m_texCoordsUR0;
	MPBVector	m_texCoordsUL0;
	MPBVector	m_texCoordsLL1;
	MPBVector	m_texCoordsLR1;
	MPBVector	m_texCoordsUR1;
	MPBVector	m_texCoordsUL1;
};

class MPBAnimation
{
public:

	MPBAnimation();
	virtual ~MPBAnimation();

	void start();
	void pause();
	void pauseWhenDone();
	void unpause();
	void stop();
	bool isRunning() { return m_started; }
	void setLoop( bool b ) { m_loop = b; }
	void addKeyFrame( MPBKeyFrame* f ) { m_keyFrames.push_back( f ); }
	void updateFrame( MPBShape* shape );
	void scaleTime( MPBFLOAT factor );
	void setAnimateTexCoords0( bool s ) { m_animateTexCoords0 = s; }
	void setAnimateTexCoords1( bool s ) { m_animateTexCoords1 = s; }


protected:

	MPBList						m_keyFrames;
	bool							m_loop;
	bool							m_started;
	bool							m_paused;
	MPBFLOAT 					m_startTime;
	MPBFLOAT 					m_pauseTime;
	bool							m_animateTexCoords0;
	bool							m_animateTexCoords1;
	bool							m_pauseWhenDone;

	MPBFLOAT getDeltaTime() const;
};



#endif




