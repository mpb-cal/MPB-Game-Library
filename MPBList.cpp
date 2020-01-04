
#include "MPBList.h"
#include "mmgr.h"


MPBList::MPBList():
	m_objects( NULL ),
	m_capacity( 1 ),
	m_size( 0 ),
	m_index( 0 )
{
	m_objects = new void*[ m_capacity ];
}

MPBList::~MPBList()
{
	NULLIFY_ARRAY( m_objects );
}

void MPBList::push_back( void* o )
{
	m_size++;
	if (m_size > m_capacity)
	{
		int oldCapacity = m_capacity;

		void** objectsTmp = new void*[ oldCapacity ];
		for (int i=0; i<oldCapacity; i++) objectsTmp[i] = m_objects[i];

		m_capacity *= 2;
		NULLIFY_ARRAY( m_objects );
		m_objects = new void*[ m_capacity ];
		for (i=0; i<oldCapacity; i++) m_objects[i] = objectsTmp[i];

		delete[] objectsTmp;
	}

	m_objects[ m_size - 1 ] = o;
}

void MPBList::push_front( void* o )
{
	int oldCapacity = m_capacity;

	void** objectsTmp = new void*[ oldCapacity ];
	for (int i=0; i<oldCapacity; i++) objectsTmp[i] = m_objects[i];

	m_size++;
	if (m_size > m_capacity)
	{
		m_capacity *= 2;
		NULLIFY_ARRAY( m_objects );
		m_objects = new void*[ m_capacity ];
	}

	m_objects[ 0 ] = o;
	for (i=1; i<oldCapacity; i++) m_objects[i] = objectsTmp[i-1];

	delete[] objectsTmp;
}


void MPBList::reset()
{
	m_index = 0;
}

void* MPBList::next()
{
	if (m_index >= m_size) return NULL;

	return m_objects[m_index++];
}

void MPBList::clear()
{
	m_size = 0;
}

void MPBList::deleteAll()
{
	for (int i=0; i<m_size; i++)
	{
		delete m_objects[i];
	}

	clear();
}

void* MPBList::get( int i )
{
	if (i < m_size) return m_objects[i];
	return NULL;
}

void* MPBList::operator[]( int i )
{
	return get( i );
}

void MPBList::remove( void* o )
{
	if (m_size)
	{
		int oldSize = m_size;
		void** objectsTmp = new void*[ oldSize ];

		for (int i=0; i<oldSize; i++) objectsTmp[i] = m_objects[i];

		clear();

		for (i=0; i<oldSize; i++) 
		{
			if (objectsTmp[i] != o) push_back( objectsTmp[i] );
		}

		delete[] objectsTmp;
	}
}

bool MPBList::contains( void* o )
{
	MPB_LIST_LOOP( *this, obj, void* )
	{
		if (obj == o) return true;
	}

	return false;
}

void MPBList::sort( CMP_FUNC compare )
{
	qsort( m_objects, m_size, sizeof( void* ), compare );
}

void MPBList::copy( MPBList& from )
{
	clear();

	MPB_LIST_LOOP( from, obj, void* )
	{
		push_back( obj );
	}
}





