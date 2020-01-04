#ifndef __SHAPE_H__
#define __SHAPE_H__

#include "MPBAABB.h"
#include "MPBList.h"
#include "ode/ode.h"

#define LODS 1

class MPBShape;
class Plane;
class MPBTexturedPlane;
class MPBLine;
class MPBAnimation;
class MPBCompositeShapeCell;


bool SortShapeFunc( MPBShape* a, MPBShape* b );

class MPBShape
{
public:

	MPBShape();
	virtual ~MPBShape();

	virtual void draw( bool useTexture, bool shadowersOnly, bool useColor );
	virtual void updateFrame();
	void drawShadowVolume();
	void createShadowVolume( const MPBAABB* cellAABB, const MPBVector& lightPos );
	void resetColor();
	void compile();
	virtual void drawList( int lod, bool useTexture, bool useColor );
	void addChild( MPBShape* s );
	void addTexture( const char* name );
	
	// transform drawing
	void setDrawMatrix( MPBMatrix& m );
	void multDrawMatrix( MPBMatrix& m );
	void resetDrawMatrix();
	void translateDraw( MPBFLOAT x, MPBFLOAT y, MPBFLOAT z );
	void rotateXDraw( MPBFLOAT degrees );
	void rotateYDraw( MPBFLOAT degrees );
	void rotateZDraw( MPBFLOAT degrees );
	void scaleDraw( MPBFLOAT x, MPBFLOAT y, MPBFLOAT z );

	// animation transform
	void resetAnimationMatrix();
	void translateAnimation( MPBFLOAT x, MPBFLOAT y, MPBFLOAT z );
	void rotateXAnimation( MPBFLOAT degrees );
	void rotateYAnimation( MPBFLOAT degrees );
	void rotateZAnimation( MPBFLOAT degrees );
	void scaleAnimation( MPBFLOAT x, MPBFLOAT y, MPBFLOAT z );
	void scaleTimeAnimations( MPBFLOAT factor );
	void startAnimations();
	void pauseAnimations();
	void pauseAnimationsWhenDone();
	void unpauseAnimations();
	void stopAnimations();
	void setLoopAnimations( bool b );

	// texture coord transform
	void setTexCoords(	int texture,
								const MPBVector& LL,
								const MPBVector& LR,
								const MPBVector& UR,
								const MPBVector& UL	);
	void setTexSize( int texture, MPBFLOAT x, MPBFLOAT y );

	void setDraw( bool set ) { m_draw = set; }
	void setCastsShadow( bool set ) { m_castsShadow = set; }
	bool getCastsShadow() { return m_castsShadow; }
	void setReceivesShadow( bool set ) { m_receivesShadow = set; }
	bool getReceivesShadow() { return m_receivesShadow; }
	bool getDraw() { return m_draw; }
	void setColor( GLfloat r, GLfloat g, GLfloat b, GLfloat a );
	void setAmbientColor( GLfloat r, GLfloat g, GLfloat b, GLfloat a );
	void setDiffuseColor( GLfloat r, GLfloat g, GLfloat b, GLfloat a );
	void setSpecularColor( GLfloat r, GLfloat g, GLfloat b, GLfloat a );
	void setEmissionColor( GLfloat r, GLfloat g, GLfloat b, GLfloat a );
	void setShininess( GLfloat s );
	void setFrustumCull( bool set ) { m_frustumCull = set; }
	void setBackfaceCull( bool	set ) { m_backfaceCull = set; }
	void setCompile( bool set ) { m_compile = set; }
//	void setBillboard( bool set ) { m_billboard = set; setCompile( !set ); }
	void setDrawn( bool set );
	void setDisableFog( bool set ) { m_disableFog = set; }
	bool getFrustumCull() { return m_frustumCull; }
	void setCollidable( bool set );
	void setAlpha( bool s ) { m_isAlpha = s; }
	bool getAlpha() { return m_isAlpha; }
	void setLit( bool s ) { m_isLit = s; }
	bool getLit() { return m_isLit; }
	void setCell( MPBCompositeShapeCell* cell ) { m_cell = cell; }
	MPBCompositeShapeCell* getCell() { return m_cell; }
	void setElasticity( MPBFLOAT d ) { m_elasticity = d; }
	MPBFLOAT getElasticity() { return m_elasticity; }

	// transform planes
	void translate( MPBFLOAT x, MPBFLOAT y, MPBFLOAT z );
	void setRotation( MPBFLOAT degreesX, MPBFLOAT degreesY, MPBFLOAT degreesZ );
	void rotateX( MPBFLOAT degrees );
	void rotateY( MPBFLOAT degrees );
	void rotateZ( MPBFLOAT degrees );
	void rotateToVector( MPBVector v );
	void scale( MPBFLOAT x, MPBFLOAT y, MPBFLOAT z );

	Plane* getPlane( int i );
	MPBList& getPlanes();
	void getPlanes( MPBList& planes );
	bool isInViewFrustum();
	void addPlane( MPBTexturedPlane* plane );
	void addLine( MPBLine* l );
	void addAnimation( MPBAnimation* l );
	bool getPenetrationPoint( MPBVector start, MPBVector& point );

	bool isCollidable() { return m_collidable; }
	const MPBAABB&	getAABB() { return m_aabb; }
	void savePOVFile( FILE* fp );
	dGeomID getGeomID() { return m_geomID; }

protected:

//	void doTransform() const;
	dGeomID							m_geomID;

private:

	void findAABB();

	MPBList							m_children;
	MPBList							m_planes;
	MPBList							m_lines;
	MPBList							m_animations;
	MPBAABB							m_aabb;					// For view frustum culling.
	bool								m_collidable;
	MPBFLOAT							m_drawMatrix[16];
	MPBFLOAT							m_animationMatrix[16];
	bool								m_compiled;
	bool								m_compile;
	GLuint							m_displayList[LODS];
	bool								m_frustumCull;
	bool								m_backfaceCull;
	bool								m_draw;
	bool								m_castsShadow;
	bool								m_receivesShadow;
//	bool								m_billboard;
	bool								m_drawn;
	bool								m_disableFog;
	bool								m_isAlpha;
	bool								m_isLit;
	MPBCompositeShapeCell*		m_cell;
	MPBFLOAT 						m_elasticity;					// 0 - 1.0

	MPBVector						m_billboardOffset1;
	MPBVector 						m_billboardOffset2;
	MPBVector 						m_billboardOffset3;
	MPBVector						m_billboardOffset4;

	// material properties
	GLfloat 						m_ambientColor[4];
	GLfloat 						m_diffuseColor[4];
	GLfloat 						m_specularColor[4];
	GLfloat 						m_emissionColor[4];
	GLfloat 						m_shininess[1];
};


#endif
