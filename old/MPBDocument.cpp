
#include "MPBDocument.h"
#include "mmgr.h"


MPBDocument::MPBDocument():
	m_changed( false )
{
	strcpy( m_name, "Document1" );
//	strcpy( m_dir, "" );
	strcpy( m_path, "" );
}

MPBDocument::~MPBDocument()
{
}

const char* MPBDocument::getName() const
{
	return m_name;
}

//const char* MPBDocument::getDir() const
//{
//	return m_dir;
//}

const char* MPBDocument::getPath() const
{
	return m_path;
}

//void MPBDocument::getPath( char** path ) const
//{
//	strcpy( *path, m_dir );
//	strcat( *path, m_name );
//}

bool MPBDocument::getChanged() const
{
	return m_changed;
}

void MPBDocument::setName( const char* name )
{
	strcpy( m_name, name );
}

//void MPBDocument::setDir( const char* dir )
//{
//	strcpy( m_dir, dir );
//}

void MPBDocument::setPath( const char* path )
{
	strcpy( m_path, path );
}

void MPBDocument::setChanged( bool set )
{
	m_changed = set;
}

void MPBDocument::saveAs( HWND hwnd, HINSTANCE hInstance )
{
	char		name[MAX_PATH] = "";
	char		path[MAX_PATH] = "";
	strcpy( name, m_name );
	strcpy( path, m_path );

	OPENFILENAME ofn;
	ZeroMemory( &ofn, sizeof( OPENFILENAME ) );

	ofn.lStructSize			= sizeof( OPENFILENAME );
	ofn.hwndOwner				= hwnd;
	ofn.hInstance				= hInstance;
	ofn.lpstrFilter			= NULL;
	ofn.lpstrCustomFilter	= NULL;
	ofn.nMaxCustFilter		= 0;
	ofn.nFilterIndex			= 0;
	ofn.lpstrFile				= path;
//	strcpy( ofn.lpstrFile, m_path );
	ofn.nMaxFile				= MAX_PATH;
	ofn.lpstrFileTitle		= name;
	ofn.nMaxFileTitle			= _MAX_FNAME + _MAX_EXT;
	ofn.lpstrInitialDir		= NULL;
	ofn.lpstrTitle				= NULL;
	ofn.Flags					= OFN_OVERWRITEPROMPT;
	ofn.nFileOffset			= 0;
	ofn.nFileExtension		= 0;
	ofn.lpstrDefExt			= NULL;
	ofn.lCustData				= 0L;
	ofn.lpfnHook				= NULL;
	ofn.lpTemplateName		= NULL;

	if (GetSaveFileName( &ofn ))
	{
		strcpy( m_name, name );
		strcpy( m_path, path );
		save( hwnd, hInstance );
	}
}

void MPBDocument::save( HWND hwnd, HINSTANCE hInstance )
{
//	char path[MAX_PATH];
//	getPath( (char**)&path );

	if (strlen( m_path ) > 0)
	{
		FILE* fp = fopen( m_path, "wb" );
		if (fp)
		{
			write( fp );
			fclose( fp );
		}
	}
	else
	{
		saveAs( hwnd, hInstance );
	}
}

void MPBDocument::open( HWND hwnd, HINSTANCE hInstance )
{
//	char szFileName[ MAX_PATH ] = "";
//	char szFileTitle[ _MAX_FNAME + _MAX_EXT ] = "";

	char		name[MAX_PATH] = "";
	char		path[MAX_PATH] = "";

	OPENFILENAME ofn;
	ZeroMemory( &ofn, sizeof( OPENFILENAME ) );

	ofn.lStructSize			= sizeof( OPENFILENAME );
	ofn.hwndOwner				= hwnd;
	ofn.hInstance				= hInstance;
	ofn.lpstrFilter			= NULL;
	ofn.lpstrCustomFilter	= NULL;
	ofn.nMaxCustFilter		= 0;
	ofn.nFilterIndex			= 0;
	ofn.lpstrFile				= path;
	ofn.nMaxFile				= MAX_PATH;
	ofn.lpstrFileTitle		= name;
	ofn.nMaxFileTitle			= _MAX_FNAME + _MAX_EXT;
	ofn.lpstrInitialDir		= NULL;
	ofn.lpstrTitle				= NULL;
	ofn.Flags					= OFN_CREATEPROMPT | OFN_EXPLORER;
	ofn.nFileOffset			= 0;
	ofn.nFileExtension		= 0;
	ofn.lpstrDefExt			= NULL;
	ofn.lCustData				= 0L;
	ofn.lpfnHook				= NULL;
	ofn.lpTemplateName		= NULL;

	if (GetOpenFileName( &ofn ) && okToCloseFile( hwnd, hInstance ))
	{
		strcpy( m_name, ofn.lpstrFileTitle );
		strcpy( m_path, ofn.lpstrFile );

		FILE* fp = fopen( m_path, "rb" );
		read( fp );
		fclose( fp );
	}
}

bool MPBDocument::okToCloseFile( HWND hwnd, HINSTANCE hInstance )
{
	bool result = false;

	if (m_changed)
	{
		char szMessage[ MAX_PATH + _MAX_FNAME ] = "Save changes to ";
		strcat( szMessage, m_name );
		strcat( szMessage, "?" );

		int iResult = MessageBox(	hwnd,
											szMessage,
											"Confirm File Close",
											MB_YESNOCANCEL	);

		switch (iResult)
		{
			case IDYES :
				save( hwnd, hInstance );
				result = false;
				break;

			case IDNO :
				result = true;
				break;

			case IDCANCEL :
				result = false;
				break;

			default :
				result = false;
		}
	}
	else
	{
		result = true;
	}

	return result;
}

/*
BOOL CApplication::OnFileExit()
{
	OnPlayStop( TRUE );

	if (OKToCloseFile())
	{
		PostQuitMessage( 0 );
		return TRUE;
	}

	return FALSE;
}

void CApplication::SetMainWindowTitle()
{
	char szWindowTitle[ _MAX_FNAME ];

	lstrcpy( szWindowTitle, APPLICATION_APPNAME );
	lstrcat( szWindowTitle, " - " );
	lstrcat( szWindowTitle, m_szFilePath );

	if (TheSong.IsModified())
	{
		lstrcat( szWindowTitle, "*" );
	}

	::SetWindowText( m_hwndMainWindow, szWindowTitle );
}

void CApplication::SetModified( BOOL blModified )
{
	SetMainWindowTitle();
}

				case ID_FILE_NEW :
				{
					if (!OKToCloseFile()) return FALSE;

					::HideCaret( ::GetParent( hwnd ) );

					OnPlayStop( TRUE );
					TheSong.Reset();

					lstrcpy( m_szFilePath, DEFAULT_FILEPATH );
					SetMainWindowTitle();

					::InvalidateRect( m_hwndClientWindow, NULL, TRUE );

					::SetCaretPos(	TheCursor.GetXpos() - m_iHScrollPos,
									TheCursor.GetYpos() - m_iVScrollPos  );
					::ShowCaret( ::GetParent( hwnd ) );

					break;
				}

				case ID_FILE_OPEN :
				{
					char szFileName[ MAX_PATH ] = "";
					char szFileTitle[ _MAX_FNAME + _MAX_EXT ] = "";

					OPENFILENAME ofn;
					ZeroMemory( &ofn, sizeof OPENFILENAME );
					
					ofn.lStructSize			= sizeof OPENFILENAME;
					ofn.hwndOwner			= m_hwndMainWindow;
					ofn.hInstance			= m_hInstance;
					ofn.lpstrFilter			= APPLICATION_OPENFILEFILTER;
					ofn.lpstrCustomFilter	= NULL;
					ofn.nMaxCustFilter		= 0;
					ofn.nFilterIndex		= 0;
					ofn.lpstrFile			= szFileName;
					ofn.nMaxFile			= MAX_PATH;
					ofn.lpstrFileTitle		= NULL;
					ofn.nMaxFileTitle		= _MAX_FNAME + _MAX_EXT;
					ofn.lpstrInitialDir		= NULL;
					ofn.lpstrTitle			= NULL;
					ofn.Flags				= OFN_CREATEPROMPT | OFN_EXPLORER;
					ofn.nFileOffset			= 0;
					ofn.nFileExtension		= 0;
					ofn.lpstrDefExt			= APPLICATION_DEFAULTEXTENSION;
					ofn.lCustData			= 0L;
					ofn.lpfnHook			= NULL;
					ofn.lpTemplateName		= NULL;

					if (GetOpenFileName( &ofn ) && OKToCloseFile())
					{
						::HideCaret( ::GetParent( hwnd ) );

						OnPlayStop( TRUE );
						TheSong.Reset();

						lstrcpy( m_szFilePath, ofn.lpstrFile );

						// read the file
						FILE* pFile = fopen( ofn.lpstrFile, "rb" );
						TheStructuredSong.Input( pFile );
						fclose( pFile );

						::SetCaretPos(	TheCursor.GetXpos() - m_iHScrollPos,
										TheCursor.GetYpos() - m_iVScrollPos  );
						::ShowCaret( ::GetParent( hwnd ) );
					}

					::InvalidateRect( m_hwndClientWindow, NULL, TRUE );

					break;
				}

				case ID_FILE_SAVE :
				{
					if (TheSong.HasNeverBeenSaved())
					{
						return ::SendMessage( hwnd, WM_COMMAND, ID_FILE_SAVE_AS, 0 );
					}
					else
					{
						FILE* pFile = fopen( m_szFilePath, "wb" );
						TheStructuredSong.Output( pFile );
						fclose( pFile );

						return TRUE;
					}

					break;
				}

				case ID_FILE_SAVE_AS :
				{
					OPENFILENAME ofn;
					ZeroMemory( &ofn, sizeof OPENFILENAME );
					
					ofn.lStructSize			= sizeof OPENFILENAME;
					ofn.hwndOwner			= m_hwndMainWindow;
					ofn.hInstance			= m_hInstance;
					ofn.lpstrFilter			= APPLICATION_OPENFILEFILTER;
					ofn.lpstrCustomFilter	= NULL;
					ofn.nMaxCustFilter		= 0;
					ofn.nFilterIndex		= 0;
					ofn.lpstrFile			= m_szFilePath;
					ofn.nMaxFile			= MAX_PATH;
					ofn.lpstrFileTitle		= NULL;
					ofn.nMaxFileTitle		= _MAX_FNAME + _MAX_EXT;
					ofn.lpstrInitialDir		= NULL;
					ofn.lpstrTitle			= NULL;
					ofn.Flags				= OFN_OVERWRITEPROMPT;
					ofn.nFileOffset			= 0;
					ofn.nFileExtension		= 0;
					ofn.lpstrDefExt			= APPLICATION_DEFAULTEXTENSION;
					ofn.lCustData			= 0L;
					ofn.lpfnHook			= NULL;
					ofn.lpTemplateName		= NULL;

					if (GetSaveFileName( &ofn ))
					{
						lstrcpy( m_szFilePath, ofn.lpstrFile );

						FILE* pFile = fopen( ofn.lpstrFile, "wb" );
						TheStructuredSong.Output( pFile );
						fclose( pFile );

						return TRUE;
					}

					break;
				}


BOOL CSong::Output( FILE* pFile )
{
	if (!CMusicalThing::Output( pFile )) return FALSE;

	if (fwrite( &m_iVoices, sizeof( m_iVoices ), 1, pFile ) < 1 ) return FALSE;
	if (fwrite( &m_sizeLayout, sizeof( m_sizeLayout ), 1, pFile ) < 1 ) return FALSE;

	for (int i = 0; i < m_iVoices; i++)
	{
		if (!((CVoice*)m_VoiceList[ i ])->Output( pFile )) return FALSE;
	}

	if (!m_Staff.Output( pFile )) return FALSE;
	if (!m_GrandStaff.Output( pFile )) return FALSE;

	SetModified( FALSE );
	m_blNeverBeenSaved = FALSE;

	return TRUE;
}

BOOL CSong::Input( FILE* pFile )
{
	if (!CMusicalThing::Input( pFile )) return FALSE;

	if (fread( &m_iVoices, sizeof( m_iVoices ), 1, pFile ) < 1 ) return FALSE;
	if (fread( &m_sizeLayout, sizeof( m_sizeLayout ), 1, pFile ) < 1 ) return FALSE;

	m_VoiceList.RemoveAll();
	InitVoices();

	for (int i = 0; i < m_iVoices; i++)
	{
		if (!((CVoice*)m_VoiceList[ i ])->Input( pFile )) return FALSE;
	}

	if (!m_Staff.Input( pFile )) return FALSE;
//	if (!m_GrandStaff.Input( pFile )) return FALSE;
	
	Normalize();

	SetModified( FALSE );
	m_blNeverBeenSaved = FALSE;

	return TRUE;
}

BOOL CSong::HasNeverBeenSaved() const
{
	return m_blNeverBeenSaved;
}

*/

