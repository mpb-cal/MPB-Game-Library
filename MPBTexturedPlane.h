#ifndef __TEXTUREDPLANE_H__
#define __TEXTUREDPLANE_H__

#include "MPBPlane.h"
#include "MPBList.h"

const int MAX_TEXTURES = 3;

class MPBDoorway;
class MPBPhysicsObject;

class MPBTexturedPlane: public Plane
{
public:

	MPBTexturedPlane();
	MPBTexturedPlane(	const MPBVector& LL,
							const MPBVector& LR,
							const MPBVector& UR,
							const MPBVector& UL	);
	MPBTexturedPlane(	const MPBVector& LL,
							const MPBVector& LR,
							const MPBVector& UR	);

	~MPBTexturedPlane();

	void draw( bool useTexture, bool useColor );
	void drawDoorway( int stencilIndex );
	void drawShadowVolume();
	void createShadowVolume( const MPBAABB* cellAABB, const MPBVector& lightPos );

	void resetTexCoords();
	void resetTexCoords( int texture );
	void addTexture( const char* name );
	int getTextureCount() { return m_textures; }
	void setTexSize( int texture, MPBFLOAT sizeX, MPBFLOAT sizeY );
	void setTexCoords(	int texture, 
								const MPBVector& LL,
								const MPBVector& LR,
								const MPBVector& UR,
								const MPBVector& UL	);
	void translateTexCoords( int texture, MPBFLOAT transS, MPBFLOAT transT );
	void scaleTexCoords( int texture, MPBFLOAT scaleS, MPBFLOAT scaleT );
	MPBVector getTexCoordLL( int texture ) const { return m_texCoordLL[texture]; }
	MPBVector getTexCoordLR( int texture ) const { return m_texCoordLR[texture]; }
	MPBVector getTexCoordUR( int texture ) const { return m_texCoordUR[texture]; }
	MPBVector getTexCoordUL( int texture ) const { return m_texCoordUL[texture]; }

	void bindTextures();
	void unbindTextures();
	void addDoorway( MPBDoorway* d ) { m_doorway = d; }
	void setLightmapTexture( const char* name );

	void setRadiosity( MPBFLOAT d );
	void setEmissivity( MPBFLOAT d );
	void setReflectivity( MPBFLOAT d );
	MPBFLOAT getRadiosity() { return m_radiosity; }
	MPBFLOAT getEmissivity() { return m_emissivity; }
	MPBFLOAT getReflectivity() { return m_reflectivity; }
	void setUseFogCoords( bool s ) { m_useFogCoords = s; }
	bool getUseFogCoords() { return m_useFogCoords; }

private:

	int				m_textures;
	char				m_texture[MAX_TEXTURES][MAX_PATH];
	MPBVector		m_texCoordLL[MAX_TEXTURES];
	MPBVector		m_texCoordLR[MAX_TEXTURES];
	MPBVector		m_texCoordUR[MAX_TEXTURES];
	MPBVector		m_texCoordUL[MAX_TEXTURES];

	MPBVector		m_shadowVolumeLightPos;
	MPBVector		m_shadowVolumeNLL;
	MPBVector		m_shadowVolumeNLR;
	MPBVector		m_shadowVolumeNUR;
	MPBVector		m_shadowVolumeNUL;
	MPBVector		m_shadowVolumeFLL;
	MPBVector		m_shadowVolumeFLR;
	MPBVector		m_shadowVolumeFUR;
	MPBVector		m_shadowVolumeFUL;
	MPBList			m_shadowVolumePlanes;
	bool				m_hasShadowVolume;
	MPBDoorway*		m_doorway;
	int				m_doorwayStencilIndex;
	bool				m_isTriangle;
	MPBFLOAT 			m_radiosity;
	MPBFLOAT 		m_emissivity;
	MPBFLOAT 		m_reflectivity;
	bool				m_useFogCoords;

	void calculatePointLightTexCoords( int texture );
};



#endif
