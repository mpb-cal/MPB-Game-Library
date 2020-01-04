
#include "MPBGlobal.h"
#include "MPBDirectPlay.h"
#include "MPBMessageWindow.h"
#include "MPBPerformanceMonitor.h"
#include "MPBCountdown.h"
#include "MPBGameEntities.h"

#include <dxerr9.h>
#include "mmgr.h"

#pragma comment(lib, "dxerr9.lib")


/*


server:

	create and initialize an IDirectPlay8Server
		set server name and/or data with SetServerInfo
	create a server IDirectPlay8Address
		set service provider type (TCP, modem, serial, etc) with SetSP
		set port number
	call Host on the IDirectPlay8Server and wait for connections
	enumerate players on the IDirectPlay8Server
		call SendTo to send data


client:

	create and initialize an IDirectPlay8Client
		set client name and/or data with SetClientInfo
	create a client IDirectPlay8Address
		set service provider type (TCP, modem, serial, etc) with SetSP
	create a remote server IDirectPlay8Address
		set service provider type (TCP, modem, serial, etc) with SetSP
		set ip address
		set port number
	call Connect on the IDirectPlay8Client
	call Send to send data


*/


GUID								MPBDirectPlay::m_guid;
IDirectPlay8Server*			MPBDirectPlay::m_dpServer;
IDirectPlay8Client*			MPBDirectPlay::m_dpClient;
IDirectPlay8Address*			MPBDirectPlay::m_dpClientAddress;
IDirectPlay8Address*			MPBDirectPlay::m_dpServerAddress;
IDirectPlay8Address*			MPBDirectPlay::m_dpRemoteServerAddress;
DPN_CONNECTION_INFO			MPBDirectPlay::m_dpConnectionInfo;
ClientInput						MPBDirectPlay::m_clientInput;
MPBCountdown*					MPBDirectPlay::m_connectionInfoCountdownServer;


struct Player
{
	bool		m_readyToReceive;
};


void hrMessage( TCHAR *szFormat, ... )
{
    TCHAR szBuffer[512];  // Large buffer for very long filenames (like HTTP)

    // Format the input string
    va_list pArgs;
    va_start(pArgs, szFormat);
    _vstprintf(szBuffer, szFormat, pArgs);
    va_end(pArgs);

    // Display a message box with the formatted string
    MessageBox(NULL, szBuffer, TEXT("MPB"), MB_OK);
}

void checkHR( HRESULT hr )
{
	if (FAILED( hr ))
	{
		const TCHAR* c = DXGetErrorDescription9( hr );
		char s[1000];
		sprintf( s, "FAILED: %s\n", c );
		hrMessage( s );
		MPB_ASSERT( 0 );
	}
}


//////////////////////////////////////////////////////////////////////
// client and server
//////////////////////////////////////////////////////////////////////


void MPBDirectPlay::init()
{
	CoInitialize( NULL );
	CoCreateGuid( &m_guid );
	m_connectionInfoCountdownServer = new MPBCountdown( 5000, false );
}

void MPBDirectPlay::cleanUp()
{
	NULLIFY( m_connectionInfoCountdownServer );

	if (m_dpServer) 
	{
		m_dpServer->Close( 0 );
		m_dpServer->Release();
		m_dpServer = NULL;
	}

	if (m_dpClient)
	{
		m_dpClient->Close( 0 );
		m_dpClient->Release();
		m_dpClient = NULL;
	}

	CoUninitialize();
}

// for TCP/IP servers.
// ip ignored if NULL.
// port ignored if 0.
IDirectPlay8Address* createAddress( const WCHAR* ip, DWORD port )
{
	HRESULT hr;
	IDirectPlay8Address* adr;

	hr = CoCreateInstance(	CLSID_DirectPlay8Address,
									NULL,
									CLSCTX_INPROC_SERVER,
									IID_IDirectPlay8Address,
									(LPVOID*)&adr	); 
	checkHR( hr );

	hr = adr->SetSP( &CLSID_DP8SP_TCPIP );
	checkHR( hr );

	if (ip)
	{
		hr = adr->AddComponent(	DPNA_KEY_HOSTNAME, 
										ip, 
										2*(wcslen( ip ) + 1), 
										DPNA_DATATYPE_STRING	);
		checkHR( hr );
	}

	if (port > 0)
	{
		hr = adr->AddComponent(	DPNA_KEY_PORT,
										&port, 
										sizeof(port),
										DPNA_DATATYPE_DWORD	);
		checkHR( hr );
	}

	return adr;
}

//////////////////////////////////////////////////////////////////////
// server only
//////////////////////////////////////////////////////////////////////

void MPBDirectPlay::initDirectPlayServer( DWORD serverPort )
{
	MPBMessageWindow::writeMessage( "Starting DirectPlay server..." );

	HRESULT hr;

	// create the DP server object

	hr = CoCreateInstance(	CLSID_DirectPlay8Server, 
									NULL, 
									CLSCTX_INPROC_SERVER,
									IID_IDirectPlay8Server, 
									(LPVOID*)&m_dpServer	);

	checkHR( hr );

	MPB_ASSERT( m_dpServer );

	hr = m_dpServer->Initialize( NULL, DirectPlayMessageHandlerServer, 0 );
	checkHR( hr );

	// set server info

	DPN_PLAYER_INFO dpi;
	ZeroMemory( &dpi, sizeof(DPN_PLAYER_INFO) );
	dpi.dwSize = sizeof(DPN_PLAYER_INFO);
	dpi.dwInfoFlags = 0;
	dpi.pwszName = NULL;
	dpi.pvData = NULL;
	dpi.dwDataSize = 0;
	dpi.dwPlayerFlags = 0;

	DPNHANDLE hAsyncHandle;

	hr = m_dpServer->SetServerInfo( &dpi, NULL, &hAsyncHandle, 0 );
	checkHR( hr );

	// create local server address object

	m_dpServerAddress = createAddress( NULL, serverPort );

	// host the session
	
	DPN_APPLICATION_DESC dad;
	ZeroMemory( &dad, sizeof(DPN_APPLICATION_DESC) );
	dad.dwSize = sizeof(DPN_APPLICATION_DESC);
	dad.dwFlags = DPNSESSION_CLIENT_SERVER | DPNSESSION_NODPNSVR;
	dad.guidApplication = m_guid;
	dad.pwszSessionName = L"MPBDirectPlay Server";

	hr = m_dpServer->Host(	&dad,
									&m_dpServerAddress, 
									1,
									NULL,
									NULL,
									NULL,
									0	);

	checkHR( hr );

	MPBMessageWindow::writeMessage( "Done." );
}

// sends a msg to one client
void MPBDirectPlay::updateClient( DPNID playerID, const ClientUpdate& clientUpdate )
{
	DPN_BUFFER_DESC dbd;
	dbd.dwBufferSize = sizeof( clientUpdate );
	dbd.pBufferData = (BYTE*)&clientUpdate;

	m_dpServer->SendTo(	playerID,
								&dbd,
								1,
								0,	// timeout in ms
								NULL,
								NULL,
								0	);	

//	sprintf( g_logBuf, "Sent to client: %d", clientUpdate.playerID );
//	MPBMessageWindow::writeMessage();
}

// called once per frame
// sends a ClientUpdate struct to each client
void MPBDirectPlay::updateClients( ClientUpdate& clientUpdate )
{
	HRESULT hr;

	DWORD players = 0;
	DPNID* playerIDs = NULL;
	hr = m_dpServer->EnumPlayersAndGroups( playerIDs, &players, DPNENUM_PLAYERS );

	int sanity = 0;
	while (hr == DPNERR_BUFFERTOOSMALL && players > 0)
	{
		delete[] playerIDs;
		if (sanity++ > 10) return;
		playerIDs = new DPNID[ players ];
		hr = m_dpServer->EnumPlayersAndGroups( playerIDs, &players, DPNENUM_PLAYERS );
	}

	for (int i=0; i<players; i++)
	{
		void* playerContext = NULL;
		m_dpServer->GetPlayerContext( playerIDs[i], &playerContext, 0 );

		if (playerContext)
		{
//			writeClientUpdate( &clientUpdate, player );

			DPN_BUFFER_DESC dbd;
			dbd.dwBufferSize = sizeof( clientUpdate );
			dbd.pBufferData = (BYTE*)&clientUpdate;
			DPNHANDLE dpnHandle;

			m_dpServer->SendTo(	playerIDs[i],
										&dbd,
										1,
										0,	// timeout in ms
										NULL,
										&dpnHandle,
										0/*DPNSEND_COMPLETEONPROCESS | DPNSEND_NONSEQUENTIAL*/	/*| DPNSEND_SYNC*/	);	

			sprintf( g_logBuf, "Sent to client: %d", clientUpdate.playerID );
			MPBMessageWindow::writeMessage();

			if (0)//(m_connectionInfoCountdownServer->isDone())
			{
				m_connectionInfoCountdownServer->start();

				m_dpConnectionInfo.dwSize = sizeof m_dpConnectionInfo;
				hr = m_dpServer->GetConnectionInfo( playerIDs[i], &m_dpConnectionInfo, 0 );
				sprintf( g_logBuf, "dwBytesDropped: %d", m_dpConnectionInfo.dwBytesDropped );
				MPBMessageWindow::writeMessage();

				int x = 0;
				if (hr == DPNERR_INVALIDOBJECT)
				{
					x = 1;
				}
				if (hr == DPNERR_INVALIDPARAM)
				{
					x = 2;
				}
				if (hr == DPNERR_INVALIDPOINTER)
				{
					x = 3;
				}
				if (hr == DPNERR_UNINITIALIZED)
				{
					x = 4;
				}
//				MPB_ASSERT( hr == S_OK );

				DWORD dwNumMsgs;
				DWORD dwNumBytes;
				hr = m_dpServer->GetSendQueueInfo( playerIDs[i], &dwNumMsgs, &dwNumBytes, 0 );
				sprintf( g_logBuf, "msgs in send queue: %d", dwNumMsgs );
				MPBMessageWindow::writeMessage();
			}
		}
	}

	delete[] playerIDs;
}

HRESULT WINAPI DirectPlayMessageHandlerServer( void *pvContext, DWORD dwMsgId, void *pvMessage )
{
	HRESULT hr;

	if (dwMsgId == DPN_MSGID_RECEIVE)
	// when client sends something
	// msgs from the same client never arrive at the same time
	{
		MPBPerformanceMonitor::m_messagesReceived++;

		DPNMSG_RECEIVE* pMsg = (DPNMSG_RECEIVE*)pvMessage;
//		MPBMessageWindow::writeMessage( "DirectPlayMessageHandlerServer: DPN_MSGID_RECEIVE" );

		//pMsg->dpnidSender;

		ClientInput* clientInput = (ClientInput*)pMsg->pReceiveData;
		MPBGameEntities::inputDPPlayer( pMsg->dpnidSender, clientInput );

		sprintf( g_logBuf, "Received from client: %d", clientInput->xRotation );
		MPBMessageWindow::writeMessage();
	}
	else if (dwMsgId == DPN_MSGID_ENUM_HOSTS_QUERY)
	{
		DPNMSG_ENUM_HOSTS_QUERY* pMsg = (DPNMSG_ENUM_HOSTS_QUERY*)pvMessage;
		MPBMessageWindow::writeMessage( "DirectPlayMessageHandlerServer: DPN_MSGID_ENUM_HOSTS_QUERY" );
	}
	else if (dwMsgId == DPN_MSGID_SEND_COMPLETE)
	{
		DPNMSG_SEND_COMPLETE* pMsg = (DPNMSG_SEND_COMPLETE*)pvMessage;
//		MPBMessageWindow::writeMessage( "DirectPlayMessageHandlerServer: DPN_MSGID_SEND_COMPLETE" );
	}
	else if (dwMsgId == DPN_MSGID_INDICATE_CONNECT)
	// when client attempts to connect
	{
		DPNMSG_INDICATE_CONNECT* pMsg = (DPNMSG_INDICATE_CONNECT*)pvMessage;
		MPBMessageWindow::writeMessage( "DirectPlayMessageHandlerServer: DPN_MSGID_INDICATE_CONNECT" );
	}
	else if (dwMsgId == DPN_MSGID_INDICATED_CONNECT_ABORTED)
	// when client fails to connect
	{
		DPNMSG_INDICATED_CONNECT_ABORTED* pMsg = (DPNMSG_INDICATED_CONNECT_ABORTED*)pvMessage;
		MPBMessageWindow::writeMessage( "DirectPlayMessageHandlerServer: DPN_MSGID_INDICATED_CONNECT_ABORTED" );
	}
	else if (dwMsgId == DPN_MSGID_CREATE_PLAYER)
	// when client is added to session
	{
		DPNMSG_CREATE_PLAYER* pMsg = (DPNMSG_CREATE_PLAYER*)pvMessage;
		MPBMessageWindow::writeMessage( "DirectPlayMessageHandlerServer: DPN_MSGID_CREATE_PLAYER" );

		sprintf( g_logBuf, "   DPNID = %u", pMsg->dpnidPlayer );
		MPBMessageWindow::writeMessage( g_logBuf );

		DPN_PLAYER_INFO pi;
		ZeroMemory( &pi, sizeof pi );
		pi.dwSize = sizeof pi;
		MPBDirectPlay::m_dpServer->GetClientInfo( pMsg->dpnidPlayer, &pi, NULL, DPNINFO_NAME | DPNINFO_DATA );

		sprintf( g_logBuf, "   name = %S", pi.pwszName );
		MPBMessageWindow::writeMessage( g_logBuf );

		IDirectPlay8Address* adr;
		hr = MPBDirectPlay::m_dpServer->GetClientAddress( pMsg->dpnidPlayer, &adr, 0 );

		// player context set to 1 if valid player, otherwise set to 0

		if (SUCCEEDED( hr ))
		{
			DWORD dwNumChars = 1000;
			CHAR* szURL = new CHAR[dwNumChars];
			adr->GetURLA( szURL, &dwNumChars );

			sprintf( g_logBuf, "   address = %s", szURL );
			MPBMessageWindow::writeMessage( g_logBuf );
			delete[] szURL;

			pMsg->pvPlayerContext = (void*)1;
			MPBGameEntities::addDPPlayer( pMsg->dpnidPlayer );
		}
		else
		{
			pMsg->pvPlayerContext = NULL;

			int x = 0;

			if (hr == DPNERR_INVALIDOBJECT)
			{
				x = 1;
			}
 			if (hr == DPNERR_INVALIDPARAM)
			{
				x = 2;
			}
			if (hr == DPNERR_INVALIDPLAYER)
			{
				x = 3;
			}
			if (hr == DPNERR_INVALIDPOINTER)
			{
				x = 4;
			}
			if (hr == DPNERR_UNINITIALIZED)
			{
				x = 5;
			}

			MPBDirectPlay::m_dpServer->DestroyClient( pMsg->dpnidPlayer, NULL, 0, 0 );
		}
	}
	else if (dwMsgId == DPN_MSGID_DESTROY_PLAYER)
	// when client leaves a session
	{
		DPNMSG_DESTROY_PLAYER* pMsg = (DPNMSG_DESTROY_PLAYER*)pvMessage;
		MPBMessageWindow::writeMessage( "DirectPlayMessageHandlerServer: DPN_MSGID_DESTROY_PLAYER" );

		sprintf( g_logBuf, "   DPNID = %u", pMsg->dpnidPlayer );
		MPBMessageWindow::writeMessage( g_logBuf );

		//if (pMsg->pvPlayerContext) MPBGameEntities::removeDPPlayer( pMsg->dpnidPlayer );
		MPBGameEntities::removeDPPlayer( pMsg->dpnidPlayer );
	}
	else if (dwMsgId == DPN_MSGID_RETURN_BUFFER)
	{
		DPNMSG_RETURN_BUFFER* pMsg = (DPNMSG_RETURN_BUFFER*)pvMessage;
		MPBMessageWindow::writeMessage( "DirectPlayMessageHandlerServer: DPN_MSGID_RETURN_BUFFER" );
	}
	else if (dwMsgId == DPN_MSGID_CLIENT_INFO)
	{
		DPNMSG_CLIENT_INFO* pMsg = (DPNMSG_CLIENT_INFO*)pvMessage;
		MPBMessageWindow::writeMessage( "DirectPlayMessageHandlerServer: DPN_MSGID_CLIENT_INFO" );
	}
	else if (dwMsgId == DPN_MSGID_TERMINATE_SESSION)
	{
		MPBMessageWindow::writeMessage( "DirectPlayMessageHandlerServer: DPN_MSGID_TERMINATE_SESSION" );
	}

	return DPN_OK;
}


//////////////////////////////////////////////////////////////////////
// client only
//////////////////////////////////////////////////////////////////////

void MPBDirectPlay::initDirectPlayClient( const WCHAR serverIP[], DWORD serverPort )
{
	MPBMessageWindow::writeMessage( "Starting DirectPlay client..." );

	HRESULT hr;

	// create the DP client object

	hr = CoCreateInstance(	CLSID_DirectPlay8Client, 
									NULL, 
									CLSCTX_INPROC_SERVER,
									IID_IDirectPlay8Client, 
									(LPVOID*)&m_dpClient	);

	checkHR( hr );
	MPB_ASSERT( m_dpClient );

	hr = m_dpClient->Initialize( NULL, DirectPlayMessageHandlerClient, 0 );
	checkHR( hr );

	// set client info

	DPN_PLAYER_INFO pi;
	ZeroMemory( &pi, sizeof pi );
	pi.dwSize = sizeof pi;
	pi.dwInfoFlags = DPNINFO_NAME;
	pi.pwszName = L"MyName";

	DPNHANDLE dh;

	hr = m_dpClient->SetClientInfo( &pi, NULL, &dh, 0 );
	checkHR( hr );

	// create local client address object

	hr = CoCreateInstance(	CLSID_DirectPlay8Address,
									NULL,
									CLSCTX_INPROC_SERVER,
									IID_IDirectPlay8Address,
									(LPVOID*)&m_dpClientAddress	); 

	checkHR( hr );
	MPB_ASSERT( m_dpClientAddress );

	hr = m_dpClientAddress->SetSP( &CLSID_DP8SP_TCPIP );
	checkHR( hr );

	/////////////////////////////////
	// enumerate Service Providers

/*	DWORD dwItems = 0;
	DWORD dwSize  = 0;

	// Determine the required buffer size
	hr = m_dpClient->EnumServiceProviders( NULL, NULL, NULL, &dwSize, &dwItems, DPNENUMSERVICEPROVIDERS_ALL );
//	checkHR( hr );

	DPN_SERVICE_PROVIDER_INFO* pdnSPInfo = (DPN_SERVICE_PROVIDER_INFO*)(new BYTE[dwSize]);

	//Fill the buffer with service provider information
	hr = m_dpClient->EnumServiceProviders( NULL, NULL, pdnSPInfo, &dwSize, &dwItems, DPNENUMSERVICEPROVIDERS_ALL );
	checkHR( hr );

	DPN_SERVICE_PROVIDER_INFO* pdnSPInfoEnum = pdnSPInfo;
	for (DWORD i = 0; i < dwItems; i++)
	{
		sprintf( g_logBuf, "Found service provider:  %S", pdnSPInfoEnum->pwszName );
		MPBMessageWindow::writeMessage( g_logBuf );
		pdnSPInfoEnum++;
	}
*/
	/////////////////////////////////


	// create remote address object and set port number

	m_dpRemoteServerAddress = createAddress( serverIP, serverPort );
//	m_dpRemoteServerAddress = createAddress( NULL, serverPort );


	/////////////////////////////////
	//	enumerate Hosts

	DPN_APPLICATION_DESC dad;

/*	MPBMessageWindow::writeMessage( "Enumerating hosts..." );

	ZeroMemory( &dad, sizeof(DPN_APPLICATION_DESC) );
	dad.dwSize = sizeof(DPN_APPLICATION_DESC);
//	dad.guidApplication = m_guid;

	hr = m_dpClient->EnumHosts(	&dad,
											m_dpRemoteServerAddress,
											m_dpClientAddress,
											NULL, 
											0,
											0,
											0,
											0,
											NULL,
											NULL,
											DPNENUMHOSTS_SYNC	| DPNENUMHOSTS_OKTOQUERYFORADDRESSING	);
	checkHR( hr );
*/

	/////////////////////////////////



	// connect to server

	MPBMessageWindow::writeMessage( "Connecting to server..." );

	ZeroMemory( &dad, sizeof(DPN_APPLICATION_DESC) );
	dad.dwSize = sizeof(DPN_APPLICATION_DESC);
	DPNHANDLE dpnHandle;

	hr = m_dpClient->Connect(	&dad,
										m_dpRemoteServerAddress,
										m_dpClientAddress,   
										NULL,  
										NULL,
										NULL, 
										0, 
										NULL,
										&dpnHandle, 
										0	/*DPNCONNECT_SYNC*/	);
	
	if (hr == DPNSUCCESS_PENDING)
	{
	}
	else if (FAILED( hr ))
	{
		checkHR( hr );
		MPBMessageWindow::writeMessage( "Failed to connect!" );
//		m_isDPClient = false;
	}

//	MPBMessageWindow::writeMessage( "Done." );
}

HRESULT WINAPI DirectPlayMessageHandlerClient( void *pvContext, DWORD dwMsgId, void *pvMessage )
{
//	MPBEnterCriticalSection();

	if (dwMsgId == DPN_MSGID_RECEIVE)
	{
		PDPNMSG_RECEIVE pMsg = (PDPNMSG_RECEIVE)pvMessage;
		ClientUpdate* clientUpdate = (ClientUpdate*)pMsg->pReceiveData;
		sprintf( g_logBuf, "Received from server: %d", clientUpdate->playerID );
		MPBMessageWindow::writeMessage( g_logBuf );
		MPBPerformanceMonitor::m_messagesReceived++;

//		if (GE::m_clientUpdateProcessed)
//		{
//			ClientUpdate* clientUpdate = (ClientUpdate*)pMsg->pReceiveData;
//			GE::m_clientUpdateFromServer = *clientUpdate;
//			GE::m_clientUpdateProcessed = false;
//		}
	}
	else if (dwMsgId == DPN_MSGID_ENUM_HOSTS_RESPONSE)
	{
		MPBMessageWindow::writeMessage( "DirectPlayMessageHandlerClient: DPN_MSGID_ENUM_HOSTS_RESPONSE" );
	}
	else if (dwMsgId == DPN_MSGID_CONNECT_COMPLETE)
	// when server responds to connect attempt
	{
		MPBMessageWindow::writeMessage( "DirectPlayMessageHandlerClient: DPN_MSGID_CONNECT_COMPLETE" );
	}
	else if (dwMsgId == DPN_MSGID_TERMINATE_SESSION)
	{
		MPBMessageWindow::writeMessage( "DirectPlayMessageHandlerClient: DPN_MSGID_TERMINATE_SESSION" );
		MessageBox( g_hwnd, "Lost connection to server.", "Disconnection", MB_OK );
	}
	else if (dwMsgId == DPN_MSGID_RETURN_BUFFER)
	{
		MPBMessageWindow::writeMessage( "DirectPlayMessageHandlerClient: DPN_MSGID_RETURN_BUFFER" );
	}
	else if (dwMsgId == DPN_MSGID_SEND_COMPLETE)
	{
		DPNMSG_SEND_COMPLETE* pMsg = (DPNMSG_SEND_COMPLETE*)pvMessage;
//		MPBMessageWindow::writeMessage( "DirectPlayMessageHandlerServer: DPN_MSGID_SEND_COMPLETE" );
	}

//	MPBLeaveCriticalSection();

	return DPN_OK;
}

void MPBDirectPlay::sendInputToServer()
{
	static DWORD tick;
	tick = GetTickCount();
	static DWORD lastTick = tick;

	////////////////////////////////////////////////
	// upload state of client input to server

	static ClientInput lastClientInput;

	static int x = 0;
	m_clientInput.xRotation = x++;

//	if (memcmp( &m_clientInput, &lastClientInput, sizeof( m_clientInput ) ))
	{
		DPN_BUFFER_DESC buffer;
		buffer.dwBufferSize = sizeof( m_clientInput );
		buffer.pBufferData = (BYTE*)&m_clientInput;
		DPNHANDLE dpnHandle;
		m_dpClient->Send(	&buffer,
								1,
								0,
								NULL,
								&dpnHandle,
								0	);

		sprintf( g_logBuf, "Sent to server: %d", m_clientInput.xRotation );
		MPBMessageWindow::writeMessage();

		lastClientInput = m_clientInput;
		lastTick = tick;
	}
}




