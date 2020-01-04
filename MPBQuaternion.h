#ifndef __MPBQUATERNION_H__
#define __MPBQUATERNION_H__

#include "MPBVector.h"
#include "MPBMatrix.h"

class MPBQuaternion
{
public:

	MPBFLOAT 		n;
	MPBVector		v;

	MPBQuaternion();
	MPBQuaternion( const MPBQuaternion& q );
	MPBQuaternion( MPBFLOAT pn, const MPBVector& pv );
	MPBQuaternion( MPBFLOAT pn, MPBFLOAT vx, MPBFLOAT vy, MPBFLOAT vz );
	const MPBQuaternion& operator=( const MPBQuaternion& q );

	MPBQuaternion operator*( const MPBQuaternion& p ) const;
	MPBQuaternion operator+( const MPBQuaternion& p ) const;

	MPBQuaternion operator*( MPBFLOAT s ) const;
	MPBQuaternion operator/( MPBFLOAT s ) const;

	MPBQuaternion operator*( const MPBVector& u ) const;
	MPBQuaternion operator~() const;

	MPBFLOAT getMagnitude() const;
	void normalize();
	void toAxisAngle( MPBVector& axis, MPBFLOAT& angle ) const;
	void fromAxisAngle( const MPBVector& axis, MPBFLOAT radians );
	void fromEuler( MPBVector radians );
	void fromEuler( MPBFLOAT xRadians, MPBFLOAT yRadians, MPBFLOAT zRadians );
//	void toMatrix( MPBFLOAT matrix[16] ) const;
	void toMatrix( MPBMatrix& matrix ) const;
	void toMatrix( MPBFLOAT matrix[16] ) const;
	void toEuler( MPBFLOAT& xRadians, MPBFLOAT& yRadians, MPBFLOAT& zRadians ) const;
	void rotateVector( MPBVector& u ) const;
	void fromVector( const MPBVector& v );
	void fromVectors( const MPBVector& v1, const MPBVector& v2 );

private:

};


#endif


