#ifndef __MPBWAVE_H__
#define __MPBWAVE_H__

// associates a SoundBite with the name of a .WAV file

#include "MPBLibrary.h"

class MPBSoundBite;

class MPBWave
{
public:

	MPBWave::MPBWave( char* dir, char* fileName, bool _3D );
	MPBWave::~MPBWave();

	const char* getDir() const { return m_dir; }
	const char* getFileName() const { return m_fileName; }
//	void setSoundBite( MPBSoundBite* sb ) { m_soundBite = sb; }
	void addSoundBite( MPBSoundBite* sb );
	MPBSoundBite* getSoundBite();// { return m_soundBite; }
	bool is3D() const { return m_3D; }

private:

	char							m_dir[MAX_PATH];
	char							m_fileName[MAX_PATH];
	vector<MPBSoundBite*>	m_soundBiteVector;
	bool							m_3D;
	int							m_index;
};




#endif


