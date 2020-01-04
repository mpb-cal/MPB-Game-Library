#ifndef __MPBDOCUMENT_H__
#define __MPBDOCUMENT_H__

#ifdef WIN32
#include <windows.h>
#include <stdio.h>


class MPBDocument
{
public:

	MPBDocument();
	~MPBDocument();

	const char* getName() const;
//	const char* getDir() const;
	const char* getPath() const;
//	void getPath( char** path ) const;
	bool getChanged() const;

	void setName( const char* name );
//	void setDir( const char* dir );
	void setPath( const char* path );
	void setChanged( bool set );

	void saveAs( HWND hwnd, HINSTANCE hInstance );
	void save( HWND hwnd, HINSTANCE hInstance );
	void open( HWND hwnd, HINSTANCE hInstance );

protected:

	virtual void read( FILE* fp ) = 0;
	virtual void write( FILE* fp ) const = 0;

private:

	bool okToCloseFile( HWND hwnd, HINSTANCE hInstance );

	char		m_name[MAX_PATH];
//	char		m_dir[MAX_PATH];
	char		m_path[MAX_PATH];
	bool		m_changed;
};



#endif

#endif
