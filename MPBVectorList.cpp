
#include "MPBVectorList.h"
#include "mmgr.h"



MPBVectorList::MPBVectorList()
{
}

MPBVectorList::~MPBVectorList()
{
}

void MPBVectorList::push_back( MPBVector& o )
{
	m_list.push_back( new MPBVector( o ) );
}

void MPBVectorList::push_front( MPBVector& o )
{
	m_list.push_front( new MPBVector( o ) );
}

void MPBVectorList::reset()
{
	m_list.reset();
}

MPBVector& MPBVectorList::next()
{
	return *((MPBVector*)m_list.next());
}

void MPBVectorList::clear()
{
	m_list.deleteAll();
}

void MPBVectorList::deleteAll()
{
	m_list.deleteAll();
}

MPBVector& MPBVectorList::get( int i )
{
	return *((MPBVector*)m_list.get( i ));
}

MPBVector& MPBVectorList::operator[]( int i )
{
	return *((MPBVector*)m_list.get( i ));
}



