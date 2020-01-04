#ifndef __PERLINNOISE_H__
#define __PERLINNOISE_H__


#include "MPBLibrary.h"

/*double perlinNoisePoint(	int x, 
									int y, 
									int octaves, 
									double amplitudeDivisor, 
									int startFrequencyDivisor,
									bool smooth	);*/
void perlinNoise(	BYTE* array, 
						int width, 
						int height, 
						int maxValue,
						double amplitude, 
						double frequency,
						bool smooth	);

double noise1(double arg);
double noise2(double vec[2]);
double noise3(double vec[3]);


#endif

