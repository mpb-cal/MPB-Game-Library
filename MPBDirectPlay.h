#ifndef __MPBDIRECTPLAY_H__
#define __MPBDIRECTPLAY_H__



const int MPB_MAX_PLAYERS = 10;

struct ClientUpdate
{  
//	unsigned char	active[MPB_MAX_PLAYERS];

//	long				x[MPB_MAX_PLAYERS];
//	long				y[MPB_MAX_PLAYERS];
//	long				z[MPB_MAX_PLAYERS];

//	MPBFLOAT 			xRot[MPB_MAX_PLAYERS];
//	MPBFLOAT 			yRot[MPB_MAX_PLAYERS];
//	MPBFLOAT 			zRot[MPB_MAX_PLAYERS];

//	long				xe[MAX_ENEMIES];
//	long				ye[MAX_ENEMIES];
//	long				ze[MAX_ENEMIES];

	int				playerID;
};


struct ClientInput
{  
//	unsigned char		keys[256];
	long					xRotation;
//	long					yRotation;
//	long					zRotation;
//	long					mouseX;
//	long					mouseY;
//	long					mouseZ;
//	unsigned char		mouseB0;
//	unsigned char		mouseB1;
};

class MPBCountdown;

class MPBDirectPlay
{
public:

	static void init();
	static void cleanUp();
	static void initDirectPlayServer( DWORD serverPort );
	static void initDirectPlayClient( const WCHAR serverIP[], DWORD serverPort );
	static void sendInputToServer();
	static void updateClient( DPNID playerID, const ClientUpdate& clientUpdate );
	static void updateClients( ClientUpdate& clientUpdate );

private:

	static GUID								m_guid;
	static IDirectPlay8Server*			m_dpServer;
	static IDirectPlay8Client*			m_dpClient;
	static IDirectPlay8Address*		m_dpClientAddress;
	static IDirectPlay8Address*		m_dpServerAddress;
	static IDirectPlay8Address*		m_dpRemoteServerAddress;
	static DPN_CONNECTION_INFO			m_dpConnectionInfo;
	static ClientInput					m_clientInput;
	static MPBCountdown*					m_connectionInfoCountdownServer;

	friend HRESULT WINAPI DirectPlayMessageHandlerServer( void *pvContext, DWORD dwMsgId, void *pvMessage );
	friend HRESULT WINAPI DirectPlayMessageHandlerClient( void *pvContext, DWORD dwMsgId, void *pvMessage );
};


#endif

