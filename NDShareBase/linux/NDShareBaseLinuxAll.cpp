
////zlib file;(为什么被注册掉,单独编译呢?因为纯C的文件的命名空间中,两个C文件中可以定义相同名称的变量函数等);
//#include "zlib/adler32.c"
//#include "zlib/compress.c"
//#include "zlib/crc32.c"
//#include "zlib/deflate.c"
//#include "zlib/gzclose.c"
//#include "zlib/gzlib.c"
//#include "zlib/gzread.c"
//#include "zlib/gzwrite.c"
//#include "zlib/inflate.c"
//#include "zlib/infback.c"
//#include "zlib/inftrees.c"
//#include "zlib/inffast.c"
//#include "zlib/trees.c"
//#include "zlib/uncompr.c"
//#include "zlib/zutil.c"


//c and global function's file;
#include "NDShareBaseGlobal.cpp"


//authentic function's file;
#include "authentic/NDCrypt.cpp"


//tinyxml file;
#include "tinyxml/tinystr.cpp"
#include "tinyxml/tinyxml.cpp"
#include "tinyxml/tinyxmlerror.cpp"
#include "tinyxml/tinyxmlparser.cpp"



//thread file;(先包含实现文件,在#include包含她们的文件);

#include "thread/NDLinuxConditionImpl.cpp"


#include "thread/NDMutex.cpp"
#include "thread/NDCondition.cpp"
#include "thread/NDThread.cpp"
#include "thread/NDDBThread.cpp"





//memory file;
#include "memory/NDBufferPool.cpp"
#include "memory/NDByteBufferImpl.cpp"
#include "memory/NDByteBuffer.cpp"
#include "memory/NDByteBufferPool.cpp"
#include "memory/object/NDMemoryPoolExManager.cpp"


//sharememory file;
#include "sharememory/NDShareMemory.cpp"
#include "sharememory/NDShareMemoryAccessObject.cpp"


//function file;
#include "function/NDDistributeID.cpp"
#include "function/NDWordFilter.cpp"
#include "function/NDConsoleCMDManager.cpp"
#include "function/NDStatWatcher.cpp"


//math file;
#include "math/NDRectangle.cpp"


//file's operation file;
#include "file/NDCFileImpl.cpp"
#include "file/NDCFile.cpp"
#include "file/NDCLogFile.cpp"
#include "file/NDConsoleLog.cpp"
#include "file/NDShareMemroyLogFile.cpp"
#include "file/NDConfig.cpp"
#include "file/NDXmlConfigBase.cpp"
#include "file/NDIniFile.cpp"
#include "file/NDExcelFile.cpp"



//database operation file;
#include "database/NDField.cpp"
#include "database/NDMysqlBaseOp.cpp"
#include "database/NDMysqlBase.cpp"
#include "database/NDTableBase.cpp"
#include "database/NDSelectTableBase.cpp"


//event operation file;
#include "event/event/NDBoundSlot.cpp"
#include "event/event/NDEvent.cpp"
#include "event/event/NDEventSet.cpp"


//timer event operation file;
#include "event/timerEvent/NDTimerBoundSlot.cpp"
#include "event/timerEvent/NDTimerEventManager.cpp"


//network environment and operation file,socket file;
#include "net/socket/NDSocketEnv.cpp"
#include "net/socket/NDSockOps.cpp"
#include "net/socket/NDSocket.cpp"
#include "net/socket/NDClientSocket.cpp"
#include "net/socket/NDSocketAcceptor.cpp"
#include "net/socket/NDSelect.cpp"
#include "net/socket/NDLinuxEpoll.cpp"

//network environment and operation file,session file;
#include "net/session/NDSession.cpp"
#include "net/session/NDClientSession.cpp"
#include "net/session/NDServerSession.cpp"
#include "net/session/NDSessionManagerImpl.cpp"
#include "net/session/NDSessionManager.cpp"



//network environment and operation file,stream file;
#include "net/stream/NDIStreamImpl.cpp"
#include "net/stream/NDOStreamImpl.cpp"
#include "net/stream/NDProtocolPacket.cpp"



//network environment and operation file,process file;
#include "net/process/NDDataProcess.cpp"
#include "net/process/NDServerTask.cpp"



//network environment and operation file,netio file;
#include "net/netio/NDClientNetIO.cpp"
#include "net/netio/NDServerNetIO.cpp"






