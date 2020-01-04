
#include "MPBLibrary.h"
#include "MPBProfile.h"

using namespace std;

class FunctionData;

static HANDLE									s_profileLogFile = NULL;
static HANDLE									s_profileCSVFile = NULL;
static stack<const char*>					s_currentFunctionStack;
static const char*							s_mainloopFunction = NULL;
static int										s_mainloopCalls = 0;
static DWORD									s_tickCountInit;
static DWORD									s_tickCountStart;
static map<const char*, FunctionData*>	s_functionMap;
static list<FunctionData*>					s_functionList;

class FunctionData
{
	public:

	FunctionData( const char* function, const char* file, int line, int depth )
		{
			s_function = function;
			s_file = file;
			s_line = line;
			s_tickCountStart = 0;
			s_totalTicks = 0;
			s_totalCalls = 0;
			s_depth = depth;
		}

	void print( DWORD totalTime, bool indent );
	bool operator<( const FunctionData& rhs );
	bool operator>( const FunctionData& rhs );

	const char*			s_function;
	const char*			s_file;
	int					s_line;
	DWORD					s_tickCountStart;
	DWORD					s_totalTicks;
	int					s_totalCalls;
	int					s_depth;
};

void FunctionData::print( DWORD totalTime, bool indent )
{
	char buf[1000];
	DWORD written;

	char funcName[1000] = "";

	if (indent) for (int i=0; i<s_depth; i++) strcat( funcName, " " );
	strcat( funcName, s_function );

	char buf2[1000];
	sprintf( buf2, "%02.2f", (float)(s_totalTicks / (float)totalTime) * 100.0f );

	char buf3[1000];
	sprintf( buf3, "%5.4f", (float)(s_totalTicks / (float)s_totalCalls) );

	if (s_mainloopCalls == 0) s_mainloopCalls = 1;

	sprintf( buf, "%-50s %8d %8d %10s %8d %5s %-40s %5d\n",
		funcName, 
		s_totalCalls,
		s_totalCalls / s_mainloopCalls,
		buf3,
		s_totalTicks,
		buf2,
		s_file,
		s_line	);

	WriteFile(
		s_profileLogFile,
		buf,
		strlen( buf ),
		&written,
		NULL
	);

	sprintf( buf, "%d,%s,%d,%s,%d,%s,%s,%d\n",
		s_depth,
		s_function, 
		s_totalCalls,
		buf3,
		s_totalTicks,
		buf2,
		s_file,
		s_line	);

	WriteFile(
		s_profileCSVFile,
		buf,
		strlen( buf ),
		&written,
		NULL
	);
}


bool FunctionData::operator<( const FunctionData& rhs )
{
	return (s_totalTicks > rhs.s_totalTicks);
}

bool FunctionData::operator>( const FunctionData& rhs )
{
	return (s_totalTicks < rhs.s_totalTicks);
}

int compareFunctionDataByName( const void* a, const void* b )
{
	return strcmp( ((FunctionData*)a)->s_function, ((FunctionData*)b)->s_function );
}

void profileInit()
{
#ifdef ENABLE_PROFILING

	s_profileLogFile = CreateFile( 
		"profile.log", 
		GENERIC_WRITE, 
		0, 
		NULL, 
		CREATE_ALWAYS, 
		FILE_ATTRIBUTE_NORMAL, 
		NULL	);

	s_profileCSVFile = CreateFile( 
		"profile.csv", 
		GENERIC_WRITE, 
		0, 
		NULL, 
		CREATE_ALWAYS, 
		FILE_ATTRIBUTE_NORMAL, 
		NULL	);

	s_tickCountInit = GetTickCount();

#endif
}

void printHeader()
{
	DWORD written;

	char buf[1000];

	sprintf(	buf, "\n%-50s %-8s %-8s %-10s %-8s %-5s %-40s %-5s\n\n", 
				"function",  
				"clls(tot)", 
				"clls(avg)", 
				"ms(avg.)", 
				"ms(tot.)", 
				"%", 
				"file", 
				"line"	);

	WriteFile( s_profileLogFile, buf, strlen( buf ), &written, NULL );

	char buf2[] = "order,depth,function,calls,ms(avg.),ms(tot.),%,file,line\n";
	WriteFile( s_profileCSVFile, buf2, strlen( buf ), &written, NULL );
}

void profileWriteLog()
{
#ifdef ENABLE_PROFILING

	DWORD written;
	char buf[1000];

	DWORD endTime = GetTickCount();
	DWORD totalTime = endTime - s_tickCountInit;

	sprintf( buf, "\nTotal (ms): %d\n\n", totalTime );
	WriteFile( s_profileLogFile, buf, strlen( buf ), &written, NULL );
	WriteFile( s_profileCSVFile, buf, strlen( buf ), &written, NULL );

	list<FunctionData> sortedList;
//	FunctionData** sortedArray = new FunctionData*[ s_functionList.size() ];

	printHeader();
	int i = 0;
	for (	list<FunctionData*>::iterator itr = s_functionList.begin(); 
			itr != s_functionList.end(); 
			itr++	)
	{
		if (strcmp( (*itr)->s_function, s_mainloopFunction ) == 0)
		{
			s_mainloopCalls = (*itr)->s_totalCalls;
		}

		sprintf( buf, "%d,", i );
		WriteFile( s_profileCSVFile, buf, strlen( buf ), &written, NULL );
		
		(*itr)->print( totalTime, true );
		sortedList.push_back( *(*itr) );
//		sortedArray[i++] = (*itr);
	}
/*
	sortedList.sort();
	printHeader();
	for (	list<FunctionData>::iterator itr2 = sortedList.begin(); 
			itr2 != sortedList.end(); 
			itr2++	)
	{
		(*itr2).print( totalTime, false );
	}
*/
/*
	qsort( sortedArray, s_functionList.size(), sizeof (FunctionData*), compareFunctionDataByName );
	printHeader();
	for (i = 0; i < s_functionList.size(); i++)
	{
		FunctionData* pfd = sortedArray[i];
		(*pfd).print( totalTime, false );
	}
*/
#endif
}

void profileCleanUp()
{
#ifdef ENABLE_PROFILING

	LIST_LOOP( s_functionList, FunctionData* ) { delete (*itr); }}
	s_functionList.clear();

	CloseHandle( s_profileLogFile );
	CloseHandle( s_profileCSVFile );

#endif
}

ProfileObject::ProfileObject( const char* function, const char* file, int line )
{
	if (s_mainloopFunction)
	{
		s_currentFunctionStack.push( function );
		FunctionData* functionData = NULL;
		map<const char*, FunctionData*>::iterator itr;

		if ((itr = s_functionMap.find( function )) == s_functionMap.end())
		{
			functionData = new FunctionData( function, file, line, s_currentFunctionStack.size() - 1 );
			s_functionMap.insert( 
				map<const char*, FunctionData*>::value_type( function, functionData )
			);
			s_functionList.push_back( functionData );
		}
		else
		{
			functionData = (*itr).second;
		}

		functionData->s_tickCountStart = GetTickCount();
	}
}

ProfileObject::~ProfileObject()
{
	DWORD tickCountEnd = GetTickCount();

	if (s_mainloopFunction)
	{
		const char*	currentFunction = s_currentFunctionStack.top();
		s_currentFunctionStack.pop();

		map<const char*, FunctionData*>::iterator itr = s_functionMap.find( currentFunction );
		FunctionData* functionData = (*itr).second;

		functionData->s_totalCalls++;
		functionData->s_totalTicks += (tickCountEnd - functionData->s_tickCountStart);
	}
}

void setMainloopFunction( const char* function )
{
	s_mainloopFunction = function;
}





