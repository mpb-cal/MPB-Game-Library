
#include "MPBTexturedPlane.h"
#include "MPBCollision.h"
#include "MPBOpenGL.h"
#include "MPBPhysics.h"
#include "MPBPerformanceMonitor.h"
//#include "MPBDoorway.h"
#include "MPBPhysicsObject.h"
#include "MPBTriangle.h"
#include <gl\glext.h>
#include "mmgr.h"

//
// GL_TEXTURE1 (texture 1) is used for attenuation map
//


const bool RANDOM_COLORS = false;

MPBTexturedPlane::MPBTexturedPlane():
	Plane(),
	m_textures( 0 ),
	m_hasShadowVolume( false ),
	m_doorway( NULL ),
	m_doorwayStencilIndex( 0 ),
	m_isTriangle( false ),
	m_radiosity( 1 ),
	m_emissivity( 0 ),
	m_reflectivity( 1 ),
	m_useFogCoords( false )
{
	resetTexCoords();
	if (RANDOM_COLORS)
	{
		m_red = RANDOM_FRACTION;
		m_green = RANDOM_FRACTION;
		m_blue = RANDOM_FRACTION;
	}
}

MPBTexturedPlane::MPBTexturedPlane(	const MPBVector& LL,
												const MPBVector& LR,
												const MPBVector& UR,
												const MPBVector& UL	):
	Plane( LL, LR, UR, UL ),
	m_textures( 0 ),
	m_doorway( NULL ),
	m_doorwayStencilIndex( 0 ),
	m_isTriangle( false ),
	m_radiosity( 1 ),
	m_emissivity( 0 ),
	m_reflectivity( 1 ),
	m_useFogCoords( false )
{
	resetTexCoords();
	if (RANDOM_COLORS)
	{
		m_red = RANDOM_FRACTION;
		m_green = RANDOM_FRACTION;
		m_blue = RANDOM_FRACTION;
	}
}

MPBTexturedPlane::MPBTexturedPlane(	const MPBVector& LL,
												const MPBVector& LR,
												const MPBVector& UR	):
	Plane( LL, LR, UR ),
	m_textures( 0 ),
	m_doorway( NULL ),
	m_doorwayStencilIndex( 0 ),
	m_isTriangle( true ),
	m_radiosity( 1 ),
	m_emissivity( 0 ),
	m_reflectivity( 1 ),
	m_useFogCoords( false )
{
	resetTexCoords();
	if (RANDOM_COLORS)
	{
		m_red = RANDOM_FRACTION;
		m_green = RANDOM_FRACTION;
		m_blue = RANDOM_FRACTION;
	}
}

MPBTexturedPlane::~MPBTexturedPlane()
{
}

void MPBTexturedPlane::setRadiosity( MPBFLOAT d ) 
{ 
	m_radiosity = d; 
}

void MPBTexturedPlane::setEmissivity( MPBFLOAT d ) 
{ 
	m_emissivity = d;
}

void MPBTexturedPlane::setReflectivity( MPBFLOAT d ) 
{ 
	m_reflectivity = d;
}


// can be called up to MAX_TEXTURES times for multitextures
void MPBTexturedPlane::addTexture( const char* name )
{
	strcpy( m_texture[m_textures], name );
	resetTexCoords( m_textures );
	m_textures++;
	MPB_ASSERT( m_textures <= MAX_TEXTURES );
}

void MPBTexturedPlane::resetTexCoords( int texture )
{
	m_texCoordLL[texture].set( 0, 0, 0 );
	m_texCoordLR[texture].set( 1, 0, 0 );
	m_texCoordUR[texture].set( 1, 1, 0 );
	m_texCoordUL[texture].set( 0, 1, 0 );
}

void MPBTexturedPlane::resetTexCoords()
{
	for (int i=0; i<m_textures; i++)
	{
		resetTexCoords( i );
	}
}

void MPBTexturedPlane::setTexSize( int texture, MPBFLOAT sizeX, MPBFLOAT sizeY )
{
	MPBFLOAT width = (m_LL - m_LR).getLength();
	MPBFLOAT height = (m_LL - m_UL).getLength();

	MPBVector LL( 0, 0, 0 );
	MPBVector LR( width / sizeX, 0, 0 );
	MPBVector UR( width / sizeX, height / sizeY, 0 );
	MPBVector UL( 0, height / sizeY, 0 );

	setTexCoords( texture, LL, LR, UR, UL );
}

void MPBTexturedPlane::setTexCoords(	int texture,
													const MPBVector& LL,
													const MPBVector& LR,
													const MPBVector& UR,
													const MPBVector& UL	)
{
	MPB_ASSERT( texture < m_textures );
	m_texCoordLL[texture] = LL;
	m_texCoordLR[texture] = LR;
	m_texCoordUR[texture] = UR;
	m_texCoordUL[texture] = UL;
}

void MPBTexturedPlane::translateTexCoords( int texture, MPBFLOAT transS, MPBFLOAT transT )
{
	MPB_ASSERT( texture < m_textures );
	m_texCoordLL[texture].translate( transS, transT, 0 );
	m_texCoordLR[texture].translate( transS, transT, 0 );
	m_texCoordUR[texture].translate( transS, transT, 0 );
	m_texCoordUL[texture].translate( transS, transT, 0 );
}

void MPBTexturedPlane::scaleTexCoords( int texture, MPBFLOAT scaleS, MPBFLOAT scaleT )
{
	MPB_ASSERT( texture < m_textures );
	m_texCoordLL[texture].scale( scaleS, scaleT, 0 );
	m_texCoordLR[texture].scale( scaleS, scaleT, 0 );
	m_texCoordUR[texture].scale( scaleS, scaleT, 0 );
	m_texCoordUL[texture].scale( scaleS, scaleT, 0 );
}

void MPBTexturedPlane::bindTextures()
{
	GLenum texId = GL_TEXTURE0;
	for (int i=0; i<m_textures; i++)
	{
		MPBOpenGL::bindTexture( m_texture[i], texId );
		texId++;
	}
}

void MPBTexturedPlane::unbindTextures()
{
	GLenum texId = GL_TEXTURE0 + m_textures - 1;
	for (int i=m_textures-1; i>=0; i--)
	{
		MPBOpenGL::unbindTexture( m_texture[i], texId );
		texId--;
	}
}

void MPBTexturedPlane::drawDoorway( int stencilIndex )
{
/*	if (m_doorway)
	{
		glStencilFunc( GL_NEVER, stencilIndex, ~0 );	// paint stencilIndex into stencil buffer
		m_doorwayStencilIndex = stencilIndex;

		glBegin( GL_QUADS );
		m_doorway->getLL().drawAsVertex();
		m_doorway->getLR().drawAsVertex();
		m_doorway->getUR().drawAsVertex();
		m_doorway->getUL().drawAsVertex();
		glEnd();
	}*/
}

void MPBTexturedPlane::calculatePointLightTexCoords( int texture )
{
	MPBPhysicsObject* light = MPBOpenGL::getClosestPointLight( getCenter() );

	if (light && m_textures > texture)
	{
		MPBVector lightPos = light->getPosition();
		bool facingAway = false;

		if (lightPos != m_LL)
		{
			MPBVector vectorToLight = lightPos - m_LL;
			vectorToLight.normalize();
			MPBFLOAT angleToLight = vectorToLight.dotProduct( getNormal() );
			if (angleToLight <= 0) facingAway = true;
		}

		// use 3D texture for lightmap
		MPBVector texCoordLL;
		MPBVector texCoordLR;
		MPBVector texCoordUR;
		MPBVector texCoordUL;

		if (facingAway)
		{
			texCoordLL.set( 10, 10, 0 );
			texCoordLR.set( 11, 10, 0 );
			texCoordUR.set( 11, 11, 0 );
			texCoordUL.set( 10, 11, 0 );
		}
		else
		{
			texCoordLL = m_LL - lightPos;
			texCoordLR = m_LR - lightPos;
			texCoordUR = m_UR - lightPos;
			texCoordUL = m_UL - lightPos;

			MPBFLOAT lightRadius = light->getRadius() * 2;
//			lightRadius *= angleToLight + 1;
			MPB_ASSERT( lightRadius );

			texCoordLL = texCoordLL / lightRadius;
			texCoordLR = texCoordLR / lightRadius;
			texCoordUR = texCoordUR / lightRadius;
			texCoordUL = texCoordUL / lightRadius;

			texCoordLL = MPBVector( .5, .5, .5 ) - texCoordLL / 4.0;
			texCoordLR = MPBVector( .5, .5, .5 ) - texCoordLR / 4.0;
			texCoordUR = MPBVector( .5, .5, .5 ) - texCoordUR / 4.0;
			texCoordUL = MPBVector( .5, .5, .5 ) - texCoordUL / 4.0;
		}

		setTexCoords( texture, texCoordLL, texCoordLR, texCoordUR, texCoordUL );
	}
	else
	{
		light = MPBOpenGL::getClosestStaticPointLight( getCenter() );

		if (light && m_textures > texture)
		{
			MPBVector lightPos = light->getPosition();
			bool facingAway = false;

			if (lightPos != m_LL)
			{
				MPBVector vectorToLight = lightPos - m_LL;
				vectorToLight.normalize();
				MPBFLOAT angleToLight = vectorToLight.dotProduct( getNormal() );
				if (angleToLight <= 0) facingAway = true;
			}

			MPBVector texCoordLL;
			MPBVector texCoordLR;
			MPBVector texCoordUR;
			MPBVector texCoordUL;

			if (facingAway)
			{
				texCoordLL.set( 10, 10, 0 );
				texCoordLR.set( 11, 10, 0 );
				texCoordUR.set( 11, 11, 0 );
				texCoordUL.set( 10, 11, 0 );
			}
			else
			{
				texCoordLL.set( 1, 1, 0 );
				texCoordLR.set( 0, 1, 0 );
				texCoordUR.set( 0, 0, 0 );
				texCoordUL.set( 1, 0, 0 );

/*				texCoordLL = m_LL - lightPos;
				texCoordLR = m_LR - lightPos;
				texCoordUR = m_UR - lightPos;
				texCoordUL = m_UL - lightPos;

				MPBFLOAT lightRadius = light->getRadius() * 2;
	//			lightRadius *= angleToLight + 1;
				MPB_ASSERT( lightRadius );

				texCoordLL = texCoordLL / lightRadius;
				texCoordLR = texCoordLR / lightRadius;
				texCoordUR = texCoordUR / lightRadius;
				texCoordUL = texCoordUL / lightRadius;

				texCoordLL = MPBVector( .5, .5, .5 ) - texCoordLL / 4.0;
				texCoordLR = MPBVector( .5, .5, .5 ) - texCoordLR / 4.0;
				texCoordUR = MPBVector( .5, .5, .5 ) - texCoordUR / 4.0;
				texCoordUL = MPBVector( .5, .5, .5 ) - texCoordUL / 4.0;
*/			}

			setTexCoords( texture, texCoordLL, texCoordLR, texCoordUR, texCoordUL );
		}
	}
}

void MPBTexturedPlane::draw( bool useTexture, bool useColor )
{
	if (0)	// draw normal
	{
		glLineWidth( 3 );
		glBegin( GL_LINES );
		MPBVector mp;
		mp.getMidpoint( m_LL, m_UR );
		mp.drawAsVertex();
		(mp + getNormal()).drawAsVertex();
		glEnd();
	}

	if (0)	// cull backfaces
	{
		MPBVector vectorToCamera = MPBOpenGL::getCameraPos() - m_LL;
		vectorToCamera.normalize();
		if (vectorToCamera.dotProduct( getNormal() ) <= 0) return;
	}

/*	if (m_doorway)
	{
		glPushAttrib( GL_STENCIL_BUFFER_BIT );
		glEnable( GL_STENCIL_TEST );
		// draw where we didn't draw window into the stencil buffer
		glStencilFunc( GL_NOTEQUAL, m_doorwayStencilIndex, ~0 );
		glPopAttrib();
	}
*/
	///////////////////////////////////////////////

	if (useTexture)
	{
//		calculatePointLightTexCoords( 1 );
	}

	///////////////////////////////////////////////

	if (m_polygonOffsetFactor || m_polygonOffsetUnits)
	{
		glEnable( GL_POLYGON_OFFSET_FILL );
		glPolygonOffset( m_polygonOffsetFactor, m_polygonOffsetUnits );
	}

	if (useColor) glColor4f( m_red * m_radiosity, m_blue * m_radiosity, m_green * m_radiosity, m_alpha );

	glNormal3f( m_normal.x, m_normal.y, m_normal.z );

	if (MPBOpenGL::is2D())
	{
		glLineWidth( 1 );
		glBegin( GL_LINE_STRIP );
		m_LL.drawAsVertex();
		m_LR.drawAsVertex();
		m_UR.drawAsVertex();
		m_UL.drawAsVertex();
		glEnd();
	}
	else if (useTexture && m_textures > 1)		// multitexturing
	{
		bindTextures();

		glBegin( GL_QUADS );

		GLenum texId = GL_TEXTURE0;
		for (int i=0; i<m_textures; i++)
		{
			MPBOpenGL::glMultiTexCoord3f( texId++, m_texCoordLL[i].x, m_texCoordLL[i].y, m_texCoordLL[i].z );
		}
		if (m_useFogCoords) MPBOpenGL::setFogCoord( m_LL );
		m_LL.drawAsVertex();

		texId = GL_TEXTURE0;
		for (i=0; i<m_textures; i++)
		{
			MPBOpenGL::glMultiTexCoord3f( texId++, m_texCoordLR[i].x, m_texCoordLR[i].y, m_texCoordLR[i].z );
		}
		if (m_useFogCoords) MPBOpenGL::setFogCoord( m_LR );
		m_LR.drawAsVertex();

		texId = GL_TEXTURE0;
		for (i=0; i<m_textures; i++)
		{
			MPBOpenGL::glMultiTexCoord3f( texId++, m_texCoordUR[i].x, m_texCoordUR[i].y, m_texCoordUR[i].z );
		}
		if (m_useFogCoords) MPBOpenGL::setFogCoord( m_UR );
		m_UR.drawAsVertex();

		texId = GL_TEXTURE0;
		for (i=0; i<m_textures; i++)
		{
			MPBOpenGL::glMultiTexCoord3f( texId++, m_texCoordUL[i].x, m_texCoordUL[i].y, m_texCoordUL[i].z );
		}
		if (m_useFogCoords) MPBOpenGL::setFogCoord( m_UL );
		m_UL.drawAsVertex();

		glEnd();

		unbindTextures();
	}
	else		// single texture
	{
		if (useTexture) bindTextures();

		if (m_isTriangle)
		{
			glBegin( GL_TRIANGLES );
			if (m_useFogCoords) MPBOpenGL::setFogCoord( m_LL ); 
			glTexCoord2d( m_texCoordLL[0].x, m_texCoordLL[0].y ); m_LL.drawAsVertex();
			if (m_useFogCoords) MPBOpenGL::setFogCoord( m_LR ); 
			glTexCoord2d( m_texCoordLR[0].x, m_texCoordLR[0].y ); m_LR.drawAsVertex();
			if (m_useFogCoords) MPBOpenGL::setFogCoord( m_UR ); 
			glTexCoord2d( m_texCoordUR[0].x, m_texCoordUR[0].y ); m_UR.drawAsVertex();
		}
		else
		{
			glBegin( GL_QUADS );

			if (m_useFogCoords) MPBOpenGL::setFogCoord( m_LL ); 
			glTexCoord2d( m_texCoordLL[0].x, m_texCoordLL[0].y );
			m_LL.drawAsVertex();

			if (m_useFogCoords) MPBOpenGL::setFogCoord( m_LR ); 
			glTexCoord2d( m_texCoordLR[0].x, m_texCoordLR[0].y );
			m_LR.drawAsVertex();

			if (m_useFogCoords) MPBOpenGL::setFogCoord( m_UR ); 
			glTexCoord2d( m_texCoordUR[0].x, m_texCoordUR[0].y );
			m_UR.drawAsVertex();

			if (m_useFogCoords) MPBOpenGL::setFogCoord( m_UL ); 
			glTexCoord2d( m_texCoordUL[0].x, m_texCoordUL[0].y );
			m_UL.drawAsVertex();
		}
		glEnd();

//			unbindTextures();
	}

	if (MPBOpenGL::glFogCoordd) MPBOpenGL::glFogCoordd( 0 );

	if (m_polygonOffsetFactor || m_polygonOffsetUnits)
		glDisable( GL_POLYGON_OFFSET_FILL );

	MPBPerformanceMonitor::m_planesDrawn++;
}

// call glBegin( GL_QUADS ) first
void MPBTexturedPlane::drawShadowVolume()
{
	if (m_hasShadowVolume)
	{
//		m_shadowVolumeFront.drawVertices2();
//		m_shadowVolumeBack.drawVertices2();
//		m_shadowVolumeLeft.drawVertices2();
//		m_shadowVolumeRight.drawVertices2();
//		m_shadowVolumeTop.drawVertices2();
//		m_shadowVolumeBottom.drawVertices2();

		if (USE_LIGHT_VOLUMES)
		{
			m_shadowVolumeNLR.drawAsVertex();
			m_shadowVolumeNUL.drawAsVertex();
			m_shadowVolumeNUR.drawAsVertex();

			m_shadowVolumeNLR.drawAsVertex();
			m_shadowVolumeNLL.drawAsVertex();
			m_shadowVolumeNUL.drawAsVertex();

			m_shadowVolumeLightPos.drawAsVertex();
			m_shadowVolumeNLR.drawAsVertex();
			m_shadowVolumeNUR.drawAsVertex();

			m_shadowVolumeLightPos.drawAsVertex();
			m_shadowVolumeNUR.drawAsVertex();
			m_shadowVolumeNUL.drawAsVertex();

			m_shadowVolumeLightPos.drawAsVertex();
			m_shadowVolumeNUL.drawAsVertex();
			m_shadowVolumeNLL.drawAsVertex();

			m_shadowVolumeLightPos.drawAsVertex();
			m_shadowVolumeNLL.drawAsVertex();
			m_shadowVolumeNLR.drawAsVertex();
		}
/*		else if (SHADOW_VOLUME_CELLS)
		{
			MPB_LIST_LOOP( m_shadowVolumePlanes, plane, Plane* )
			{
				plane->getLL().drawAsVertex();
				plane->getLR().drawAsVertex();
				plane->getUR().drawAsVertex();
				plane->getUL().drawAsVertex();
			}
		}
*/		else
		{
			// front face
			glVertex3d( m_shadowVolumeNLL.x, m_shadowVolumeNLL.y, m_shadowVolumeNLL.z );
			glVertex3d( m_shadowVolumeNLR.x, m_shadowVolumeNLR.y, m_shadowVolumeNLR.z );
			glVertex3d( m_shadowVolumeNUR.x, m_shadowVolumeNUR.y, m_shadowVolumeNUR.z );
			glVertex3d( m_shadowVolumeNUL.x, m_shadowVolumeNUL.y, m_shadowVolumeNUL.z );

			// back face
			glVertex3d( m_shadowVolumeFLR.x, m_shadowVolumeFLR.y, m_shadowVolumeFLR.z );
			glVertex3d( m_shadowVolumeFLL.x, m_shadowVolumeFLL.y, m_shadowVolumeFLL.z );
			glVertex3d( m_shadowVolumeFUL.x, m_shadowVolumeFUL.y, m_shadowVolumeFUL.z );
			glVertex3d( m_shadowVolumeFUR.x, m_shadowVolumeFUR.y, m_shadowVolumeFUR.z );

			// left face
			glVertex3d( m_shadowVolumeFLL.x, m_shadowVolumeFLL.y, m_shadowVolumeFLL.z );
			glVertex3d( m_shadowVolumeNLL.x, m_shadowVolumeNLL.y, m_shadowVolumeNLL.z );
			glVertex3d( m_shadowVolumeNUL.x, m_shadowVolumeNUL.y, m_shadowVolumeNUL.z );
			glVertex3d( m_shadowVolumeFUL.x, m_shadowVolumeFUL.y, m_shadowVolumeFUL.z );

			// right face
			glVertex3d( m_shadowVolumeNLR.x, m_shadowVolumeNLR.y, m_shadowVolumeNLR.z );
			glVertex3d( m_shadowVolumeFLR.x, m_shadowVolumeFLR.y, m_shadowVolumeFLR.z );
			glVertex3d( m_shadowVolumeFUR.x, m_shadowVolumeFUR.y, m_shadowVolumeFUR.z );
			glVertex3d( m_shadowVolumeNUR.x, m_shadowVolumeNUR.y, m_shadowVolumeNUR.z );

			// top face
			glVertex3d( m_shadowVolumeNUL.x, m_shadowVolumeNUL.y, m_shadowVolumeNUL.z );
			glVertex3d( m_shadowVolumeNUR.x, m_shadowVolumeNUR.y, m_shadowVolumeNUR.z );
			glVertex3d( m_shadowVolumeFUR.x, m_shadowVolumeFUR.y, m_shadowVolumeFUR.z );
			glVertex3d( m_shadowVolumeFUL.x, m_shadowVolumeFUL.y, m_shadowVolumeFUL.z );

			// bottom face
			glVertex3d( m_shadowVolumeFLL.x, m_shadowVolumeFLL.y, m_shadowVolumeFLL.z );
			glVertex3d( m_shadowVolumeFLR.x, m_shadowVolumeFLR.y, m_shadowVolumeFLR.z );
			glVertex3d( m_shadowVolumeNLR.x, m_shadowVolumeNLR.y, m_shadowVolumeNLR.z );
			glVertex3d( m_shadowVolumeNLL.x, m_shadowVolumeNLL.y, m_shadowVolumeNLL.z );
		}
	}
}

void MPBTexturedPlane::createShadowVolume( const MPBAABB* cellAABB, const MPBVector& lightPos )
{
	MPBFLOAT edgeLength = MPBOpenGL::getPerspectiveFar() / 2;

	// plane casts no shadow if light is inside plane
	if (isPointOutside( lightPos ))
	{
		m_hasShadowVolume = true;

		if (USE_LIGHT_VOLUMES)
		{
			m_shadowVolumeLightPos = lightPos;

			MPBVector nudge = getNormal() * -1;
			nudge.setLength( .1 );

			m_shadowVolumeNLL = m_LL + nudge;
			m_shadowVolumeNLR = m_LR + nudge;
			m_shadowVolumeNUR = m_UR + nudge;
			m_shadowVolumeNUL = m_UL + nudge;
		}
		else
		{
			m_shadowVolumeNLL = m_LL;
			m_shadowVolumeNLR = m_LR;
			m_shadowVolumeNUR = m_UR;
			m_shadowVolumeNUL = m_UL;

			MPBVector lightPosToLL = m_shadowVolumeNLL - lightPos;
			MPBVector lightPosToLR = m_shadowVolumeNLR - lightPos;
			MPBVector lightPosToUR = m_shadowVolumeNUR - lightPos;
			MPBVector lightPosToUL = m_shadowVolumeNUL - lightPos;

			////////////////////////////

			lightPosToLL.setLength( edgeLength );
			lightPosToLR.setLength( edgeLength );
			lightPosToUR.setLength( edgeLength );
			lightPosToUL.setLength( edgeLength );
			
			m_shadowVolumeFLL = m_shadowVolumeNLL + lightPosToLL;
			m_shadowVolumeFLR = m_shadowVolumeNLR + lightPosToLR;
			m_shadowVolumeFUR = m_shadowVolumeNUR + lightPosToUR;
			m_shadowVolumeFUL = m_shadowVolumeNUL + lightPosToUL;

/*			if (SHADOW_VOLUME_CELLS)
			{
				MPB_ASSERT( cellAABB );

				MPBList planes;

				// front face
				planes.push_back( 
					new Plane( m_shadowVolumeNLL, m_shadowVolumeNLR, m_shadowVolumeNUR, m_shadowVolumeNUL ) );

				// back face
				planes.push_back( 
					new Plane( m_shadowVolumeFLR, m_shadowVolumeFLL, m_shadowVolumeFUL, m_shadowVolumeFUR ) );

				// left face
				planes.push_back( 
					new Plane( m_shadowVolumeFLL, m_shadowVolumeNLL, m_shadowVolumeNUL, m_shadowVolumeFUL ) );

				// right face
				planes.push_back( 
					new Plane( m_shadowVolumeNLR, m_shadowVolumeFLR, m_shadowVolumeFUR, m_shadowVolumeNUR ) );

				// top face
				planes.push_back( 
					new Plane( m_shadowVolumeNUL, m_shadowVolumeNUR, m_shadowVolumeFUR, m_shadowVolumeFUL ) );

				// bottom face
				planes.push_back( 
					new Plane( m_shadowVolumeFLL, m_shadowVolumeFLR, m_shadowVolumeNLR, m_shadowVolumeNLL ) );

				MPBList cellPlanes;
				cellAABB->getPlanes( cellPlanes );
				MPB_LIST_LOOP( cellPlanes, plane2, Plane* )
				{
					planes.push_back( plane2 );
				}

				MPB_LIST_DELETE( m_shadowVolumePlanes, Plane* );
				findHalfspaceIntersection( planes, m_shadowVolumePlanes );
			}
*/		}
	}
	else
	{
		m_hasShadowVolume = false;
	}
}









