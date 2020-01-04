#ifndef __MPBMATRIX_H__
#define __MPBMATRIX_H__


#include "MPBLibrary.h"
#include "MPBQuaternion.h"

class MPBMatrix
{
public:

	MPBFLOAT m[16];	

	MPBMatrix();
	MPBMatrix( const MPBMatrix& copyFrom );

	MPBMatrix& operator=( const MPBMatrix& rhs );
	void toOpenGLArray( MPBFLOAT* array ) const;
	void fromOpenGLArray( MPBFLOAT* array );
	void loadIdentity();
	MPBMatrix operator*( const MPBMatrix& rhs );
	void translate( MPBVector trans );
	void rotateX( MPBFLOAT radians );
	void rotateY( MPBFLOAT radians );
	void rotateZ( MPBFLOAT radians );
	void createTranslation( MPBVector trans );
	void createRotation( MPBVector right, MPBVector up, MPBVector forward );
	void createRotationFromVector( MPBVector v );
	void fromAxisAngle( MPBVector axis, MPBFLOAT radians );
	MPBQuaternion getQuaternion() const;
	MPBVector getRightAxis() const;
	MPBVector getUpAxis() const;
	MPBVector getOutAxis() const;

private:

};


#endif


