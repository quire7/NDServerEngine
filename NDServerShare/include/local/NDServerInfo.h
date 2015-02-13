#ifndef __SERVER_SHARE_ND_SERVER_INFO_H__
#define __SERVER_SHARE_ND_SERVER_INFO_H__

#include "NDTypes.h"
#include "net/socket/NDSocketAddress.h"

#include "NDProtocolBase.h"

using NDShareBase::NDSocketAddress;

struct	NDLoadFactor
{
private:
	NDUint32	m_nCurConn;
	NDUint32	m_nMaxConn;

public:
	explicit NDLoadFactor(NDUint32 nMaxConn = 1000) : m_nCurConn( 0 ), m_nMaxConn( nMaxConn )
	{}

	NDLoadFactor( const NDLoadFactor& other ) : m_nCurConn( other.m_nCurConn ), m_nMaxConn( other.m_nMaxConn )
	{}

	bool operator == (const NDLoadFactor& other)
	{
		return (m_nCurConn == other.m_nCurConn) && (m_nMaxConn == other.m_nMaxConn);
	}

	bool operator != (const NDLoadFactor& other)
	{
		return !(*this == other);
	}

	NDLoadFactor& operator = ( const NDLoadFactor& other )
	{
		if ( this == &other ) return *this;

		m_nCurConn	= other.m_nCurConn;
		m_nMaxConn	= other.m_nMaxConn;

		return *this;
	}

	NDLoadFactor& operator += ( const NDLoadFactor& other )
	{
		m_nCurConn += other.m_nCurConn;
		m_nMaxConn += other.m_nMaxConn;

		return *this;
	}

	NDLoadFactor& operator -= ( const NDLoadFactor& other )
	{
		m_nCurConn -= other.m_nCurConn;
		m_nMaxConn -= other.m_nMaxConn;

		return *this;
	}

	void	setCurConnected( NDUint32 nCurConn ) { m_nCurConn = nCurConn; }
	void	setMaxConnected( NDUint32 nMaxConn ) { m_nMaxConn = nMaxConn; }

	NDUint32 getCurConnected() const { return m_nCurConn; }
	NDUint32 getMaxConnected() const { return m_nMaxConn; }

	/// Advance (prefix only)
	NDLoadFactor& operator ++()
	{
		++m_nCurConn;
		return *this;
	}
	///reduce (prefix only)
	NDLoadFactor& operator --()
	{
		--m_nCurConn;
		return *this;
	}
	void clear()
	{
		m_nCurConn = 0;
		m_nMaxConn = 0;
	}
};

/************************************************************************/
/* class description:													*/
/*	class  NDServerInfo just save server base config information.		*/
/************************************************************************/
class NDServerInfo
{
public:
	enum eLocalRemote
	{
		eLR_Null			= 0x00,
		eLR_Local			= 0x01,
		eLR_Remote			= 0x02,
	};

	enum eConnState_Types
	{
		eConnState_Null,
		eConnState_Initial,
		eConnState_Connected,
		eConnState_DisConnected,
	};

private:

	NDSocketAddress				m_netAddress;					//server address;

	SERVERTYPE					m_byServType;					//Server type;

	eConnState_Types			m_eConnState;					//Connect server state;

	eLocalRemote				m_eLocalRemote;					//local or remote type;

	NDUint32					m_nSessionID;					//Server Session ID;

	NDUint32					m_nServerID;					//Server ID;

	char						m_szServName[MAX_SERVER_NAME];	//server name;

public:
	NDServerInfo(void);
	virtual ~NDServerInfo(void);

	NDBool						setServerName( const char* szName );
	const char*					getServerName() const;

	void						setNetAddress( const NDSocketAddress& netAddress );
	const NDSocketAddress&		getNetAddress() const;

	NDBool						setServerIP( const char* szIP );
	const char*					getServerIP() const;

	void						setServerPort(  NDUint16 usPort );
	NDUint16					getServerPort() const;

	void						setServerType( SERVERTYPE byServerType );
	SERVERTYPE					getServerType() const;

	void						setSessionID( NDUint32 nSessionID );
	NDUint32					getSessionID() const;

	void						setConnState( eConnState_Types eState );
	eConnState_Types			getConnState() const;
	
	void						setLocalRemote( eLocalRemote eLR );
	eLocalRemote				getLocalRemote() const;

	void						setServerID( NDUint32 nServerID );
	NDUint32					getServerID() const;
};


class NDLocalServerInfo : public NDServerInfo
{
private:
	char						m_szDumpPath[MAX_PATH_LEN];
	char						m_szLogPath[MAX_PATH_LEN];

public:
	NDLocalServerInfo();
	~NDLocalServerInfo();

	NDBool						setDumpPath( const char* szDumpPath );
	NDBool						setLogPath( const char* szLogPath );

	const char* 				getDumpPath() const;
	const char* 				getLogPath()  const;
};

#endif

