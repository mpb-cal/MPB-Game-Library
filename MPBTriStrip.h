#ifndef __MPBTRISTRIP_H__
#define __MPBTRISTRIP_H__

#include "F:/MPB/MPBLibrary/MPBLibrary.h"
#include <ode/ode.h>
#include "MPBVectorList.h"

class MPBTriStrip
{
public:

		// default constructor
	MPBTriStrip();
		// destructor
	virtual ~MPBTriStrip();

	MPBTriStrip( char* texture );
	void addVertex( MPBVector v );
	void compile();
	void draw();
	dGeomID getGeomID() { return m_geomID; }

private:

		// copy constructor:
		//		MPBTriStrip A = B;
		//		MPBTriStrip A( B );
		//		MPBTriStrip passed or returned by value
	MPBTriStrip( const MPBTriStrip& copyFrom );

		// assignment operators:
		//		A = B;
	const MPBTriStrip& operator= ( const MPBTriStrip& rhs );
	const MPBTriStrip& operator+=( const MPBTriStrip& rhs );
	const MPBTriStrip& operator-=( const MPBTriStrip& rhs );
	const MPBTriStrip& operator/=( const MPBTriStrip& rhs );
	const MPBTriStrip& operator*=( const MPBTriStrip& rhs );

		// relational ops
	int operator< ( const MPBTriStrip& rhs ) const;
	int operator<=( const MPBTriStrip& rhs ) const;
	int operator> ( const MPBTriStrip& rhs ) const;
	int operator>=( const MPBTriStrip& rhs ) const;
	int operator==( const MPBTriStrip& rhs ) const;
	int operator!=( const MPBTriStrip& rhs ) const;

		// binary ops
	MPBTriStrip operator+( const MPBTriStrip& rhs ) const;
	MPBTriStrip operator-( const MPBTriStrip& rhs ) const;
	MPBTriStrip operator/( const MPBTriStrip& rhs ) const;
	MPBTriStrip operator*( const MPBTriStrip& rhs ) const;

		// unary ops
		// prefix form: ++A;
	const MPBTriStrip& operator++();
		// postfix form: A++;
	MPBTriStrip operator++( int );
		// prefix form: --A;
	const MPBTriStrip& operator--();
		// postfix form: A--;
	MPBTriStrip operator--( int );

protected:

private:

	GLuint							m_displayList;
	MPBVectorList					m_vertexList;
	dGeomID							m_geomID;
	char								m_texture[MAX_PATH];

};



#endif



