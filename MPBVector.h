#ifndef __MPBVECTOR_H__
#define __MPBVECTOR_H__


class MPBQuaternion;
class MPBMatrix;
//class MPBVector2D;

// don't use x,y,z as parameter names!
// use px,py,pz instead

class MPBVector
{
public:
	
	MPBFLOAT x;
	MPBFLOAT y;
	MPBFLOAT z;

	MPBFLOAT xn;
	MPBFLOAT yn;
	MPBFLOAT zn;

	bool		m_normalized;

	MPBVector();
	MPBVector( const MPBVector& v );
//	MPBVector( const MPBVector2D& v );
	MPBVector( MPBFLOAT px, MPBFLOAT py, MPBFLOAT pz );
	MPBVector( const MPBFLOAT* xyz );
	const MPBVector& operator=( const MPBVector& v );
//	const MPBVector& operator=( const MPBVector2D& v );

	MPBVector normalized();
	void set( MPBFLOAT px, MPBFLOAT py, MPBFLOAT pz );
	void reset() { x = 0.0; y = 0.0; z = 0.0; };
	MPBFLOAT dotProduct( const MPBVector& v ) const;
	MPBVector crossProduct( const MPBVector& v ) const;
	bool operator==( const MPBVector& v ) const;
	bool operator!=( const MPBVector& v ) const;
	MPBVector operator+( const MPBVector& v ) const;
	MPBVector operator-( const MPBVector& v ) const;
	MPBVector operator*( MPBFLOAT v ) const;
	MPBVector operator/( MPBFLOAT v ) const;
	MPBVector operator*( const MPBQuaternion& q ) const;
	MPBVector operator*( const MPBMatrix& m ) const;
	MPBFLOAT getLength() const;
	void setLength( MPBFLOAT length );
	void normalize();
	void move( const MPBVector& direction, MPBFLOAT distance );
	void translate( MPBFLOAT px, MPBFLOAT py, MPBFLOAT pz );
	void scale( MPBFLOAT px, MPBFLOAT py, MPBFLOAT pz );
	void rotateX( MPBFLOAT degrees );
	void rotateY( MPBFLOAT degrees );
	void rotateZ( MPBFLOAT degrees );
	void rotateXRadians( MPBFLOAT radians );
	void rotateYRadians( MPBFLOAT radians );
	void rotateZRadians( MPBFLOAT radians );
	void doOpenGLRotation();
	void doOpenGLRotation( MPBVector right );
	MPBFLOAT getXRotation();
	MPBFLOAT getYRotation();
	MPBFLOAT getZRotation();
	void rotateMatrix( MPBMatrix& m );
	void getMidpoint( const MPBVector& lhs, const MPBVector& rhs );
	void toArray( MPBFLOAT array[3] ) const;
	void toString( char* buf ) const;
	void draw2D( const void* hdc, int xFrom, int yFrom ) const;
	void drawAsVertex() const;

	static void test();

private:

};


#endif


