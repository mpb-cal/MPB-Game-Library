
#include "MPBLibrary.h"
#include "MPBTextureGenerator.h"
#include "MPBOpenGL.h"
#include "mmgr.h"


void generateLightmapTexture(	const char* file2D, 
										const char* file1D, 
										int width,
										int height	)
{
	const MPBFLOAT CONSTANT = 0;
	const MPBFLOAT LINEAR = .05;
	const MPBFLOAT QUADRATIC = .2;
	const MPBFLOAT DISTANCE_FACTOR = 1;

	if (0)	// use 1D and 2D textures
	{
		int centerX = width / 2;
		int centerY = height / 2;
		int centerZ = width / 2;

		BYTE* reds = new BYTE[ width * height ];
		BYTE* greens = new BYTE[ width * height ];
		BYTE* blues = new BYTE[ width * height ];

		BYTE* reds1D = new BYTE[ width ];
		BYTE* greens1D = new BYTE[ width ];
		BYTE* blues1D = new BYTE[ width ];

		for (int y=0; y<height; y++)
		{
			for (int x=0; x<width; x++)
			{
				MPBFLOAT distSq1D;
				MPBFLOAT distSq2D;
				MPBFLOAT dist;

				int relativeX = x - centerX;
				int relativeY = y - centerY;
				int z;
				int relativeZ;
				int index;

				if (y == centerY)
				{
					z = x;
					relativeZ = z - centerZ;
					distSq1D = relativeZ * relativeZ;
					dist = sqrt( distSq1D ) * DISTANCE_FACTOR;
					MPBFLOAT attenuationD = CONSTANT + LINEAR * dist + QUADRATIC * distSq1D;
					MPBFLOAT attenuation = 0;
					if (attenuationD) attenuation = 1.0 / attenuationD;
					MPBFLOAT color = 255.0 * attenuation;
					if (color > 255) color = 255;
					reds1D[z] = color;
					greens1D[z] = color;
					blues1D[z] = color;
				}

				index = y * width + x;
				distSq2D = relativeX * relativeX + relativeY * relativeY;
				dist = sqrt( distSq2D ) * DISTANCE_FACTOR;
				MPBFLOAT attenuationD = CONSTANT + LINEAR * dist + QUADRATIC * distSq2D;
				MPB_ASSERT( attenuationD );
				MPBFLOAT attenuation = 1.0 / attenuationD;

				MPBFLOAT color = 255.0 * attenuation;
				if (color > 255) color = 255;
				reds[index] = color;
				greens[index] = color;
				blues[index] = color;
			}
		}

		MPBOpenGL::createTextureFromMemory( file2D, reds, greens, blues, NULL, width, height, 1, GL_CLAMP_TO_EDGE, true );
		MPBOpenGL::createTextureFromMemory( file1D, reds1D, greens1D, blues1D, NULL, width, 1, 1, GL_CLAMP_TO_EDGE, true );

		delete[] reds;
		delete[] greens;
		delete[] blues;

		delete[] reds1D;
		delete[] greens1D;
		delete[] blues1D;
	}
	else		// use 3D texture
	{
		int depth = width;

		int centerX = width / 2;
		int centerY = height / 2;
		int centerZ = depth / 2;

		BYTE* reds = new BYTE[ width * height * depth ];
		BYTE* greens = new BYTE[ width * height * depth ];
		BYTE* blues = new BYTE[ width * height * depth ];

		for (int z=0; z<depth; z++)
		{
			for (int y=0; y<height; y++)
			{
				for (int x=0; x<width; x++)
				{
					MPBFLOAT distSq;
					MPBFLOAT dist;

					int relativeX = x - centerX;
					int relativeY = y - centerY;
					int relativeZ = z - centerZ;
					int index;

					index = z * depth * width + y * width + x;
					distSq = relativeX * relativeX + relativeY * relativeY + relativeZ * relativeZ;
					dist = sqrt( distSq ) * DISTANCE_FACTOR;
					MPBFLOAT attenuationD = CONSTANT + LINEAR * dist + QUADRATIC * distSq;
					MPBFLOAT attenuation = 1;
					if (attenuationD) attenuation = 1.0 / attenuationD;

					MPBFLOAT color = 255.0 * attenuation;
					if (color > 255) color = 255;
					reds[index] = color;
					greens[index] = color;
					blues[index] = color;
				}
			}
		}

		MPBOpenGL::createTextureFromMemory( file2D, reds, greens, blues, NULL, width, height, depth, GL_CLAMP_TO_EDGE, true );

		delete[] reds;
		delete[] greens;
		delete[] blues;
	}
}

void generateParticleTexture( const char* name )
{
	const MPBFLOAT CONSTANT = 0;
	const MPBFLOAT LINEAR = 0;
	const MPBFLOAT QUADRATIC = .25;

	int width = 64;
	int height = 64;

	int centerX = width / 2;
	int centerY = height / 2;

	BYTE* reds = new BYTE[ width * height ];
	BYTE* greens = new BYTE[ width * height ];
	BYTE* blues = new BYTE[ width * height ];
	BYTE* alphas = new BYTE[ width * height ];
	
//	BYTE* colors = new BYTE[ width * height ];

	for (int y=0; y<height; y++)
	{
		for (int x=0; x<width; x++)
		{
			MPBFLOAT dist;

			int relativeX = x - centerX;
			int relativeY = y - centerY;
			int index;

			index = y * width + x;
			MPBFLOAT distSq = relativeX * relativeX + relativeY * relativeY;
			dist = sqrt( distSq );
			MPBFLOAT attenuationD = CONSTANT + LINEAR * dist + QUADRATIC * distSq;
			if (attenuationD < 1) attenuationD = 1;
			MPBFLOAT attenuation = 1.0 / attenuationD;

			MPBFLOAT color = 255.0;
			reds[index] = color;
			greens[index] = color;
			blues[index] = color;

			MPBFLOAT alpha = 255.0 * attenuation;
			if (alpha > 255) alpha = 255;
			alphas[index] = alpha;
//			colors[index] = alpha;
		}
	}

	MPBOpenGL::createTextureFromMemory( name, reds, greens, blues, alphas, width, height, 1, GL_CLAMP_TO_EDGE, true );
//	writeGrayscaleBitmap( name, colors, width, height );

	delete[] reds;
	delete[] greens;
	delete[] blues;
}

