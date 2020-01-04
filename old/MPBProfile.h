#ifndef __PROFILE_H__
#define __PROFILE_H__

/*

instructions:

uncomment #define ENABLE_PROFILING

call profileInit() right before starting main loop

put PROFILE_START_MAINLOOP( "funcName" ) at start of game loop function

put PROFILE_START( "functionName" ) at start of functions to be included in profile

call profileWriteLog() and profileCleanUp() at finish

files written are:
	profile.log
	profile.csv

*/

#ifdef _DEBUG
//#define ENABLE_PROFILING
#endif

class ProfileObject
{
	public:

	ProfileObject::ProfileObject( const char* function, const char* file, int line );
	~ProfileObject();
};

void setMainloopFunction( const char* function );

#ifdef ENABLE_PROFILING

//#define PROFILE_START( x ) profileStart( x, __FILE__, __LINE__ );
//#define PROFILE_END profileEnd();
#define PROFILE_START( x ) ProfileObject __profileObject( x, __FILE__, __LINE__ );
#define PROFILE_END
#define PROFILE_START_MAINLOOP( x ) setMainloopFunction( x ); \
	ProfileObject __profileObject( x, __FILE__, __LINE__ );

#else

#define PROFILE_START( x )
#define PROFILE_END
#define PROFILE_START_MAINLOOP( x )

#endif

void profileInit();
void profileWriteLog();
void profileCleanUp();
void profileStart( const char* function, const char* file, int line );
void profileEnd();



#endif

