#ifndef __VECTOR_H__
#define __VECTOR_H__

template <class T>
class Vector
{
	public:
	
	T	m_x;
	T	m_y;
	T	m_z;

	Vector();
	Vector( const Vector<T>& v );
	Vector( T x, T y, T z );
	const Vector<T>& operator=( const Vector<T>& v );

	void set( T x, T y, T z );
	T dotProduct( const Vector<T>& v ) const;
	Vector<T> crossProduct( const Vector<T>& v ) const;
	bool operator==( const Vector<T>& v ) const;
	bool operator!=( const Vector<T>& v ) const;
	Vector<T> operator+( const Vector<T>& v ) const;
	Vector<T> operator-( const Vector<T>& v ) const;
	Vector<T> operator*( T v ) const;
	Vector<T> operator/( T v ) const;
	double getLength() const;
	void setLength( double length );
	void normalize();
	void move( const Vector<T>& direction, T distance );
	void translate( T x, T y, T z );
	void scale( T x, T y, T z );
	void rotateX( float degrees );
	void rotateY( float degrees );
	void rotateZ( float degrees );
	void getMidpoint( const Vector<T>& lhs, const Vector<T>& rhs );
//	Vector<T> getRotation() const;
	void toArray( T array[3] ) const;
//	T* getArray();
	void toString( char* buf );

	private:

//	T	m_array[3];
};

template <class T, class T2>
void vectorCopy( Vector<T>& to, const Vector<T2>& from );

template <class T> Vector<T>::Vector() :
	m_x( 0 ),
	m_y( 0 ),
	m_z( 0 )
{
}

template <class T>
Vector<T>::Vector( const Vector<T>& v ) :
	m_x( v.m_x ),
	m_y( v.m_y ),
	m_z( v.m_z )
{
}

template <class T>
Vector<T>::Vector( T x, T y, T z ) :
	m_x( x ),
	m_y( y ),
	m_z( z )
{
}

template <class T>
const Vector<T>& Vector<T>::operator=( const Vector<T>& v )
{
	m_x = v.m_x;
	m_y = v.m_y;
	m_z = v.m_z;

	return *this;
}

template <class T>
void Vector<T>::set( T x, T y, T z )
{
	m_x = x;
	m_y = y;
	m_z = z;
}

template <class T>
bool Vector<T>::operator==( const Vector<T>& v ) const
{
	if (
		m_x == v.m_x &&
		m_y == v.m_y &&
		m_z == v.m_z
	)
	{
		return true;
	}

	return false;
}

template <class T>
bool Vector<T>::operator!=( const Vector<T>& v ) const
{
	return !(*this == v);
}

template <class T>
T Vector<T>::dotProduct( const Vector<T>& v ) const
{
	return	m_x * v.m_x + 
				m_y * v.m_y + 
				m_z * v.m_z;
}

template <class T>
Vector<T> Vector<T>::crossProduct( const Vector<T>& v ) const
{
	Vector<T> result(	m_y * v.m_z - m_z * v.m_y,
							m_z * v.m_x - m_x * v.m_z,
							m_x * v.m_y - m_y * v.m_x	);

	return result;
}

template <class T>
Vector<T> Vector<T>::operator+( const Vector<T>& v ) const
{
	Vector<T> result;
	result.m_x = m_x + v.m_x;
	result.m_y = m_y + v.m_y;
	result.m_z = m_z + v.m_z;

	return result;
}

template <class T>
Vector<T> Vector<T>::operator-( const Vector<T>& v ) const
{
	Vector<T> result;
	result.m_x = m_x - v.m_x;
	result.m_y = m_y - v.m_y;
	result.m_z = m_z - v.m_z;

	return result;
}

template <class T>
Vector<T> Vector<T>::operator*( const T v ) const
{
	Vector<T> result;
	result.m_x = m_x * v;
	result.m_y = m_y * v;
	result.m_z = m_z * v;

	return result;
}

template <class T>
Vector<T> Vector<T>::operator/( const T v ) const
{
	if (v == 0) return *this;

	Vector<T> result;
	result.m_x = m_x / v;
	result.m_y = m_y / v;
	result.m_z = m_z / v;

	return result;
}

template <class T>
double Vector<T>::getLength() const
{
	return sqrt( pow( m_x, 2 ) + pow( m_y, 2 ) + pow( m_z, 2 ) );
}

template <class T>
void Vector<T>::setLength( double length )
{
	normalize();
	*this = *this * length;
}

template <class T>
void 
Vector<T>::normalize()
{
	*this = *this / getLength();
}

template <class T, class T2>
void vectorCopy( Vector<T>& to, const Vector<T2>& from )
{
	to.m_x = from.m_x;
	to.m_y = from.m_y;
	to.m_z = from.m_z;
}

template <class T>
void Vector<T>::move( const Vector<T>& direction, T distance )
{
	m_x += direction.m_x * distance;
	m_y += direction.m_y * distance;
	m_z += direction.m_z * distance;
}

template <class T>
void Vector<T>::translate( T x, T y, T z )
{
	m_x += x;
	m_y += y;
	m_z += z;
}

template <class T>
void Vector<T>::scale( T x, T y, T z )
{
	m_x *= x;
	m_y *= y;
	m_z *= z;
}

template <class T>
void Vector<T>::rotateX( float degrees )
{
	float radians = degreesToRadians( degrees );

	float cosine = cos( radians );
	float sine = sin( radians );

	Vector<T> result;
	result.m_x = m_x;
	result.m_y = m_y * cosine - m_z * sine;
	result.m_z = m_y * sine + m_z * cosine;
	*this = result;
}

template <class T>
void Vector<T>::rotateY( float degrees )
{
	float radians = degreesToRadians( degrees );

	float cosine = cos( radians );
	float sine = sin( radians );

	Vector<T> result;
	result.m_x = m_x * cosine + sine * m_z;
	result.m_y = m_y;
	result.m_z = m_x * -sine + m_z * cosine;
	*this = result;
}

template <class T>
void Vector<T>::rotateZ( float degrees )
{
	float radians = degreesToRadians( degrees );

	float cosine = cos( radians );
	float sine = sin( radians );

	Vector<T> result;
	result.m_x = m_x * cosine - m_y * sine;
	result.m_y = m_x * sine + m_y * cosine;
	result.m_z = m_z;
	*this = result;
}

template <class T>
void Vector<T>::getMidpoint( const Vector<T>& lhs, const Vector<T>& rhs )
{
	*this = (lhs + rhs) / 2;
}

/*
template <class T>
Vector<T> Vector<T>::getRotation() const
{
	_ASSERTE( 0 );
	double azimuth;
	double colatitude;
	cartesianToSpherical( *this, azimuth, colatitude );
	Vector<double> rotation( radiansToDegrees( colatitude ), radiansToDegrees( azimuth ), 0 );
	return rotation;
}
*/
template <class T>
void Vector<T>::toArray( T array[3] ) const
{
	array[0] = m_x;
	array[1] = m_y;
	array[2] = m_z;
}

template <class T>
void Vector<T>::toString( char* buf )
{
	sprintf( buf, "%f, %f, %f", m_x, m_y, m_z );
}

/*
template <class T>
T* Vector<T>::getArray()
{
	m_array[0] = m_x;
	m_array[1] = m_y;
	m_array[2] = m_z;

	return m_array;
}
*/

#endif


