
#include "MPBDialogBox.h"
//#include "resource.h"
#include "mmgr.h"


static MPBDialogBox* g_pThis = NULL;


MPBDialogBox::MPBDialogBox()
{
	if (!g_pThis)
	{
		g_pThis = this;
	}
}

MPBDialogBox::~MPBDialogBox()
{
	g_pThis = NULL;
}

BOOL MPBDialogBox::DoModal( HINSTANCE hInstance, HWND hwndParent )
{
	if (g_pThis)
	{
		return ::DialogBox(	hInstance, 
									MAKEINTRESOURCE( m_dialogResourceID ), 
									hwndParent, 
									(DLGPROC)DialogProcStub	);
	}

	return FALSE;
}

BOOL MPBDialogBox::DialogProc( HWND hDlg, UINT iMsg, WPARAM wParam, LPARAM lParam )
{
	switch (iMsg)
	{
		case WM_INITDIALOG :
		{
			// Get the owner window and dialog box rectangles. 
 
			RECT rc, rcDlg, rcOwner; 
 
			::GetWindowRect( ::GetParent( hDlg ), &rcOwner ); 
			::GetWindowRect( hDlg, &rcDlg ); 
			::CopyRect( &rc, &rcOwner ); 
 
			 // Offset the owner and dialog box rectangles so that 
			 // right and bottom values represent the width and 
			 // height, and then offset the owner again to discard 
			 // space taken up by the dialog box. 
 
			::OffsetRect( &rcDlg, -rcDlg.left, -rcDlg.top ); 
			::OffsetRect( &rc, -rc.left, -rc.top ); 
			::OffsetRect( &rc, -rcDlg.right, -rcDlg.bottom ); 
 
			 // The new position is the sum of half the remaining 
			 // space and the owner's original position. 
 
			::SetWindowPos(	hDlg, 
							HWND_TOP, 
							rcOwner.left + (rc.right / 2), 
							rcOwner.top + (rc.bottom / 2), 
							0, 0,          // ignores size arguments 
							SWP_NOSIZE	); 
 
			return OnInitDialog( hDlg );
		}

		case WM_COMMAND :
		{	
			switch (LOWORD( wParam ))
			{
				case IDOK :
				{
					if (OnOk( hDlg ))
					{
						::EndDialog( hDlg, TRUE );
					}

					return TRUE;
				}
				case IDCANCEL :
				{	
					::EndDialog( hDlg, FALSE );

					return TRUE;
				}

				default :
				{
					return CommandHandler( hDlg, wParam, lParam );
				}
			}

			break;
		}
	}

	return FALSE;
}


BOOL CALLBACK DialogProcStub( HWND hDlg, UINT iMsg, WPARAM wParam, LPARAM lParam )
{
	if (g_pThis)
	{
		return g_pThis->DialogProc( hDlg, iMsg, wParam, lParam );
	}

	return FALSE;
}


