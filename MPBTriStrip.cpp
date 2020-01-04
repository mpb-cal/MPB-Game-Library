
#include "MPBTriStrip.h"
#include "MPBOpenGL.h"


	// default constructor
MPBTriStrip::MPBTriStrip()
{
}

	// destructor
MPBTriStrip::~MPBTriStrip()
{
}


	// copy constructor:
	//		MPBTriStrip A = B;
	//		MPBTriStrip A( B );
	//		MPBTriStrip passed or returned by value
MPBTriStrip::MPBTriStrip( const MPBTriStrip& copyFrom )
{
	*this = copyFrom;
}


	// assignment operators:
	//		A = B;
const MPBTriStrip& MPBTriStrip::operator= ( const MPBTriStrip& rhs )
{
	if (this == &rhs) return *this;

	// implement here
	_ASSERTE( 0 );

	return *this;
}

const MPBTriStrip& MPBTriStrip::operator+=( const MPBTriStrip& rhs )
{
	*this = *this + rhs;
	return *this;
}

const MPBTriStrip& MPBTriStrip::operator-=( const MPBTriStrip& rhs )
{
	*this = *this - rhs;
	return *this;
}

const MPBTriStrip& MPBTriStrip::operator/=( const MPBTriStrip& rhs )
{
	*this = *this / rhs;
	return *this;
}

const MPBTriStrip& MPBTriStrip::operator*=( const MPBTriStrip& rhs )
{
	*this = *this * rhs;
	return *this;
}


	// relational ops
int MPBTriStrip::operator< ( const MPBTriStrip& rhs ) const
{
	// implement less-than here
	_ASSERTE( 0 );

	return 0;
}

int MPBTriStrip::operator<=( const MPBTriStrip& rhs ) const
{
	return (*this < rhs || *this == rhs);
}

int MPBTriStrip::operator> ( const MPBTriStrip& rhs ) const
{
	return !(*this <= rhs);
}

int MPBTriStrip::operator>=( const MPBTriStrip& rhs ) const
{
	return (*this > rhs || *this == rhs);
}

int MPBTriStrip::operator==( const MPBTriStrip& rhs ) const
{
	// implement comparison here
	_ASSERTE( 0 );

	return 0;
}

int MPBTriStrip::operator!=( const MPBTriStrip& rhs ) const
{
	return !(*this == rhs);
}


	// binary ops
MPBTriStrip MPBTriStrip::operator+( const MPBTriStrip& rhs ) const
{
	MPBTriStrip answer( *this );

	// implement here
	_ASSERTE( 0 );

	return answer;
}

MPBTriStrip MPBTriStrip::operator-( const MPBTriStrip& rhs ) const
{
	MPBTriStrip answer( *this );

	// implement here
	_ASSERTE( 0 );

	return answer;
}

MPBTriStrip MPBTriStrip::operator/( const MPBTriStrip& rhs ) const
{
	MPBTriStrip answer( *this );

	// implement here
	_ASSERTE( 0 );

	return answer;
}

MPBTriStrip MPBTriStrip::operator*( const MPBTriStrip& rhs ) const
{
	MPBTriStrip answer( *this );

	// implement here
	_ASSERTE( 0 );

	return answer;
}

	// unary ops
	// prefix form: ++A;
const MPBTriStrip& MPBTriStrip::operator++()
{
	// implement here
	_ASSERTE( 0 );

	return *this;
}

	// postfix form: A++;
MPBTriStrip MPBTriStrip::operator++( int )
{
	MPBTriStrip tmp = *this;
	// implement here
	_ASSERTE( 0 );

	return tmp;
}

	// prefix form: --A;
const MPBTriStrip& MPBTriStrip::operator--()
{
	// implement here
	_ASSERTE( 0 );

	return *this;
}

	// postfix form: A--;
MPBTriStrip MPBTriStrip::operator--( int )
{
	MPBTriStrip tmp = *this;
	// implement here
	_ASSERTE( 0 );

	return tmp;
}

////////////////////////////////////////////////////////////


MPBTriStrip::MPBTriStrip( char* texture )
{
	MPBTriStrip();
	strcpy( m_texture, texture );
}

void MPBTriStrip::compile()
{
	MPB_ASSERT( m_vertexList.size() >= 4 );
	MPB_ASSERT( m_vertexList.size() % 2 == 0 );

	m_displayList = glGenLists( 1 );
	glNewList( m_displayList, GL_COMPILE );

	glBegin( GL_TRIANGLE_STRIP );

	for (int i=0; i<m_vertexList.size(); i++)
	{
		glTexCoord3f( i % 2, (i % 4) / 2, 0 );
		m_vertexList[i].drawAsVertex();
	}

	glEnd();

	glEndList();

	/////////////////////////////////////////////////////////////
	// create ODE mesh

	dTriMeshDataID triMeshDataID = dGeomTriMeshDataCreate();

	int numVertices = m_vertexList.size();
	dVector3* vertices = new dVector3[numVertices];

	for (i=0; i<m_vertexList.size(); i++)
	{
		vertices[i][0] = m_vertexList[i].x;
		vertices[i][1] = m_vertexList[i].y;
		vertices[i][2] = m_vertexList[i].z;
	}

	int numTris = m_vertexList.size() - 2;
	int numIndices = numTris * 3;
	int* indices = new int[numIndices];

	int ind = 0;
	for (i=0; i<m_vertexList.size() - 2; i+=2)
	{
		int i1 = i + 1;

		indices[ind++] = i1;
		indices[ind++] = i1 + 1;
		indices[ind++] = i1 - 1;

		indices[ind++] = i1;
		indices[ind++] = i1 + 2;
		indices[ind++] = i1 + 1;
	}

	MPB_ASSERT( ind == numIndices );

	dGeomTriMeshDataBuildSimple( triMeshDataID, (const dReal*)vertices, numVertices, indices, numIndices );

	m_geomID = dCreateTriMesh( 0, triMeshDataID, NULL, NULL, NULL );
}

void MPBTriStrip::draw()
{
	MPBOpenGL::bindTexture( m_texture, GL_TEXTURE0 );

	glColor4f( 1, 1, 1, 1 );
	glCallList( m_displayList );
}

void MPBTriStrip::addVertex( MPBVector v )
{
	m_vertexList.push_back( v );
}














