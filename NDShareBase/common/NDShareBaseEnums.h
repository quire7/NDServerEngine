#ifndef __SHARE_BASE_ND_SHARE_BASE_ENUMS_H__
#define __SHARE_BASE_ND_SHARE_BASE_ENUMS_H__



enum EThreadStatus
{
	EThreadStatus_invalid,		//��Ч;
	EThreadStatus_ready,		//��ǰ�̴߳���׼��״̬;
	EThreadStatus_running,		//��ǰ�̴߳�������״̬;
	EThreadStatus_exiting,		//��ǰ�̴߳��������˳�״̬;
	EThreadStatus_exit,			//��ǰ�̴߳����˳�״̬;
};


enum EIOEventType
{
	EIOEventType_Invalid	= 0x00,		//��Ч;
	EIOEventType_Read		= 0x01,		//���¼�;
	EIOEventType_Write		= 0x02,		//д�¼�;
	EIOEventType_Close		= 0x04,		//�ر��¼�;
};

enum E_NDSlotFunctorBase
{
	E_NDSlotFunctorBase_RUN		= 1,
	E_NDSlotFunctorBase_SUSPEND	= 2,
	E_NDSlotFunctorBase_DELETE	= 4,
};

#endif

