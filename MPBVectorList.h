#ifndef __MPB_VECTORLIST_H__
#define __MPB_VECTORLIST_H__

#include "MPBLibrary.h"
#include "MPBList.h"


class MPBVectorList
{
public:

	MPBVectorList();
	~MPBVectorList();
	int size() { return m_list.size(); }
	void push_back( MPBVector& o );
	void push_front( MPBVector& o );
	void reset();
	MPBVector& next();
	void clear();
	void deleteAll();
	MPBVector& get( int i );
	MPBVector& operator[]( int i );

protected:

	MPBList		m_list;
};




#endif
