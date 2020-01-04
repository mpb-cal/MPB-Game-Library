#ifndef __MPBSHAPEQUAD_H__
#define __MPBSHAPEQUAD_H__

#include "MPBLibrary.h"
#include "MPBShape.h"

class Plane;

class MPBShapeQuad: public MPBShape
{
public:

	MPBShapeQuad( int width, int height, const char* texture );
	MPBShapeQuad( int width, int height, int depth, const char* texture );
	MPBShapeQuad( 	MPBVector& LL,
						MPBVector& LR,
						MPBVector& UR,
						MPBVector& UL, 
						const char* texture );
	MPBShapeQuad( 	MPBVector& LL,
						MPBVector& LR,
						MPBVector& UR,
						const char* texture );
	virtual ~MPBShapeQuad();

	virtual void updateFrame();
	void setBillboard( bool b ) { m_isBillboard = b; }
	void setXAxisBillboard( bool b ) { m_isXAxisBillboard = b; }
	void setYAxisBillboard( bool b ) { m_isYAxisBillboard = b; }
	void setZAxisBillboard( bool b ) { m_isZAxisBillboard = b; }
	bool getBillboard() { return m_isBillboard; }
	bool getXAxisBillboard() { return m_isXAxisBillboard; }
	bool getYAxisBillboard() { return m_isYAxisBillboard; }
	bool getZAxisBillboard() { return m_isZAxisBillboard; }

private:

	void makePlane( MPBVector LL, MPBVector LR, MPBVector UR, MPBVector UL, const char* texture );

	bool		m_isBillboard;
	bool		m_isXAxisBillboard;
	bool		m_isYAxisBillboard;
	bool		m_isZAxisBillboard;
	bool		m_isAxialBillboard;
};


#endif
