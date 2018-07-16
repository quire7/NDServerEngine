
#include "main/local/NDPreCompiler.cpp"

//remote file;

//DB Thread file;
#include "main/thread/NDUpdateDBThread.cpp"
#include "main/thread/NDSelectDBThread.cpp"
#include "main/thread/NDDBThreadManager.cpp"

//table file;
#include "main/table/NDPlayerAccountTable.cpp"
#include "main/table/NDPlayerMainTable.cpp"

//data file;
#include "main/data/NDDSPlayer.cpp"
#include "main/data/NDDSPlayerManager.cpp"
#include "main/data/NDDSDataManager.cpp"

//message file;
#include "message/NDS2SRegisterCallBack.cpp"
#include "message/NDGS2DSCallBack.cpp"


//Main file;
#include "main/local/NDDataCallBack.cpp"
#include "main/local/NDDataServer.cpp"
