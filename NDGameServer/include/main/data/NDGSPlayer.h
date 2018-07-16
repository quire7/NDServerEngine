#ifndef __GAMESERVER_ND_GS_PALYER_H__
#define __GAMESERVER_ND_GS_PALYER_H__

#include "main/local/NDPreCompiler.h"

class NDServerInfo;
class NDGSPlayer
{
private:
	char						m_szSessionKey[ND_SHA1_LEN];					//client��¼��֤��(WS����);

	NDBool						m_bRecvAllDSPlayerData;							//����DS���͹����������Ƿ�����;
	NDServerInfo*				m_pDSServerInfo;								//��ҷ������Ǹ�DS��;
	NDUint32					m_nGTWSServerID;								//��ҷ������Ǹ�GatewayServer��;

	NDPlayerMain				m_NDPlayerMain;									//�����������;

public:
	NDGSPlayer();
	~NDGSPlayer();

	void						clear();
	void						release();

	NDBool						init();

	//���������Ϣ-start;
	void						setPlayerMain( const NDPlayerMain& refData );
	
	NDBool						setPlayerName( const char* szPlayerName )					{ return m_NDPlayerMain.setPlayerName(szPlayerName); }
	const char*					getPlayerName() const										{ return m_NDPlayerMain.m_szPlayerName; }

	void						setPlayerGUID( NDUint64 nPlayerGUID )						{ m_NDPlayerMain.m_nPlayerGUID = nPlayerGUID; }
	NDUint64					getPlayerGUID() const										{ return m_NDPlayerMain.m_nPlayerGUID; }
	
	//���������Ϣ-end;

	//���úͻ��������Ǹ�GTWS��;
	void						setGTWSServerID( NDUint32 nServerID )						{ m_nGTWSServerID = nServerID; }
	NDUint32					getGTWSServerID() const										{ return m_nGTWSServerID; }

	//���úͻ�ô�DS���յ���������Ƿ�����;
	void						setRecvAllDSPlayerData( NDBool bOK )						{ m_bRecvAllDSPlayerData = bOK; }
	NDBool						getRecvAllDSPlayerData() const								{ return m_bRecvAllDSPlayerData; }
	//���úͻ�ȡDS��Ϣ;
	void						setDSServerInfo( const NDServerInfo* pDSServerInfo )		{ m_pDSServerInfo = (NDServerInfo*)pDSServerInfo; }
	NDUint32					getDSServerID() const;
	NDUint32					getDSSessionID() const;

	//�������ݵ�DS;
	NDBool						sendProtocolToDS( NDProtocol& refProtocol );				//��DS������Ϣ�ӿ�;
	NDBool						sendPlayerLoginToDS();										//֪ͨDS��¼,��������;
	NDBool						sendPlayerMainToDS();										//��������������ݵ�DS;


	//WS;
	NDBool						sendPlayerLoginToWS();										//���͵�¼��Ϣ��WS;


	//�������ݵ�GTWS(Ҳ����Ҫ���͵��ͻ���);
	NDBool						sendProtocolToGTWS( NDProtocol& refProtocol );				//������Ϣ��GTWS�ϵĽӿ�;
	NDBool						sendPlayerLoginDataSuccessToGTWS();							//֪ͨGTWS,GS���������,�ͻ��˿��Ե�¼;
};


#endif
