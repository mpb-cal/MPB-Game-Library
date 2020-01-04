#ifndef __MPBVECTOR2D_H__
#define __MPBVECTOR2D_H__


class MPBQuaternion;
class MPBMatrix;

// don't use x,y,z as parameter names!
// use px,py,pz instead

class MPBVector2D
{
public:
	
	MPBFLOAT x;
	MPBFLOAT y;

	MPBFLOAT xn;
	MPBFLOAT yn;

	bool		m_normalized;

	MPBVector2D();
	MPBVector2D( const MPBVector2D& v );
	MPBVector2D( MPBFLOAT px, MPBFLOAT py );
	const MPBVector2D& operator=( const MPBVector2D& v );

	MPBVector2D normalized();
	void set( MPBFLOAT px, MPBFLOAT py );
	void reset() { x = 0.0; y = 0.0; };
	MPBFLOAT dotProduct( const MPBVector2D& v ) const;
//	MPBVector2D crossProduct( const MPBVector2D& v ) const;
	bool operator==( const MPBVector2D& v ) const;
	bool operator!=( const MPBVector2D& v ) const;
	MPBVector2D operator+( const MPBVector2D& v ) const;
	MPBVector2D operator-( const MPBVector2D& v ) const;
	MPBVector2D operator*( MPBFLOAT v ) const;
	MPBVector2D operator/( MPBFLOAT v ) const;
	MPBFLOAT getLength() const;
	void setLength( MPBFLOAT length );
	void normalize();
	void move( const MPBVector2D& direction, MPBFLOAT distance );
	void translate( MPBFLOAT px, MPBFLOAT py );
	void scale( MPBFLOAT px, MPBFLOAT py );
	void rotate( MPBFLOAT degrees );
	void rotateRadians( MPBFLOAT radians );
	MPBFLOAT getRotation();
	void getMidpoint( const MPBVector2D& lhs, const MPBVector2D& rhs );
	void toArray( MPBFLOAT array[2] ) const;
	void toString( char* buf ) const;

private:

};


#endif


