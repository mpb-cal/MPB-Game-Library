#ifndef _GAMEGEMS3_UNIT_SINEOSCILLATOR_H
#define _GAMEGEMS3_UNIT_SINEOSCILLATOR_H

#include "ggsynth.h"
#include "Unit_Oscillator.h"

/**
 *	Unit_SineOscillator. 
 *
 * @author (C) 2002 Phil Burk, SoftSynth.com, All Rights Reserved
 */

class Unit_SineOscillator : public Unit_Oscillator
{
public:
	Unit_SineOscillator();
	~Unit_SineOscillator();

/* Define functions for various scalar or vector input ocmbinations. */
    virtual void gen( GGSynthFloat amplitude, GGSynthFloat frequency);
    virtual void gen( GGSynthFloat amplitude, GGSynthFloat *frequencies );
    virtual void gen( GGSynthFloat *amplitudes, GGSynthFloat frequency );
    virtual void gen( GGSynthFloat *amplitudes, GGSynthFloat *frequencies );

private:
protected:

};

#endif /* _GAMEGEMS3_UNIT_SINEOSCILLATOR_H */
