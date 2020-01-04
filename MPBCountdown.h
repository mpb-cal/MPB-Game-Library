#ifndef __COUNTDOWN_H__
#define __COUNTDOWN_H__


// all times are in milliseconds

class MPBCountdown
{
public:

	MPBCountdown( int length, bool shouldRandomize );
	~MPBCountdown();

	void update( int ms );
	void start();
	void start( int length );
	bool isDone() { return m_elapsedTime <= 0; }
	int getLength() { return m_length; }
	void setLength( int length ) { m_length = length; }
	bool shouldRandomize() { return m_shouldRandomize; }

private:

	int			m_elapsedTime;
	int			m_length;
	bool			m_shouldRandomize;
};








#endif

