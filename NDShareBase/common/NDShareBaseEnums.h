#ifndef __SHARE_BASE_ND_SHARE_BASE_ENUMS_H__
#define __SHARE_BASE_ND_SHARE_BASE_ENUMS_H__



enum EThreadStatus
{
	EThreadStatus_invalid,		//无效;
	EThreadStatus_ready,		//当前线程处于准备状态;
	EThreadStatus_running,		//当前线程处于运行状态;
	EThreadStatus_exiting,		//当前线程处于正在退出状态;
	EThreadStatus_exit,			//当前线程处于退出状态;
};


enum EIOEventType
{
	EIOEventType_Invalid	= 0x00,		//无效;
	EIOEventType_Read		= 0x01,		//读事件;
	EIOEventType_Write		= 0x02,		//写事件;
	EIOEventType_Close		= 0x04,		//关闭事件;
};

enum E_NDSlotFunctorBase
{
	E_NDSlotFunctorBase_RUN		= 1,
	E_NDSlotFunctorBase_SUSPEND	= 2,
	E_NDSlotFunctorBase_DELETE	= 4,
};

#endif

