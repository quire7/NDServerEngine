#ifndef __DATASERVER_ND_DS_PALYER_H__
#define __DATASERVER_ND_DS_PALYER_H__


#include "main/local/NDPreCompiler.h"

class NDDSPlayer
{
private:

	NDUint8				m_playerState;									//DS�ϵ����״̬;

	NDUint32			m_nGSServerID;									//������Ǹ�GS��;

	NDPlayerAccount		m_playerAccount;								//����˺ű�;
	NDPlayerMain		m_playerMain;									//�����������;

	NDBool				m_tableDirtyDataFlag[ETableType_MAX];			//�жϱ��Ƿ�������Ҫ����;
	NDTime				m_tableOperateTime[ETableType_MAX];				//ÿ�ű������ϴθ��µ�ʱ��;

public:
	NDDSPlayer();
	~NDDSPlayer();

	void				clear();

	NDBool				init();
	void				release();

	//���úͻ��������Ǹ�GS��;
	void				setGSServerID( NDUint32 nGSServerID )			{ m_nGSServerID = nGSServerID; }
	NDUint32			getGSServerID() const							{ return m_nGSServerID; }
	NDBool				sendProtocolToGS( NDProtocol& refProtocol );


	NDUint64			getPlayerGUID() const	{ return m_playerAccount.m_nPlayerGUID; };

	//����˺ű�����;
	NDBool				insertUpdatePlayerAccount( const NDPlayerAccount& refPlayerAccount );
	//�����������;
	NDBool				insertUpdatePlayerMain( const NDPlayerMain& refPlayerMain );



	






	//��������������ݵ�GS;
	NDBool				sendAllPlayerDataToGS();

private:
	//���������������;
	NDBool				sendPlayerMainToGS();
};


#endif
