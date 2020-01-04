#ifndef __MPB_LIST_H__
#define __MPB_LIST_H__

#include "MPBLibrary.h"


//#define MPB_LIST_LOOP( list, object, type )	\
//	(list).reset();	\
//	while (type object = (type)(list).next())


#define MPB_LIST_LOOP( list, object, type )	\
	type object = (type)(list)[0];	\
	if (0) {} else for (int _listIndex=0; _listIndex<(list).size(); object = (type)(list)[++_listIndex])

#define MPB_LIST_DELETE( list, type )	\
	(list).reset();	\
	while (type object = (type)(list).next()) delete object;	\
	(list).clear();

typedef int(__cdecl *CMP_FUNC)(const void *, const void *);

class MPBList
{
public:

	MPBList();
	~MPBList();
	int size() { return m_size; }
	void push_back( void* o );
	void push_front( void* o );
	void reset();
	void* next();
	void clear();
	void deleteAll();
	void* get( int i );
	void* operator[]( int i );
	void remove( void* o );
	bool contains( void* o );
	void** getObjects() { return m_objects; }
	void sort( CMP_FUNC compare );
	void copy( MPBList& from );

protected:

	void**		m_objects;
	int			m_capacity;
	int			m_size;
	int			m_index;
};





#endif
