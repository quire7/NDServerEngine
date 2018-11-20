@echo off
echo 清除所有obj pch idb pdb ncb opt plg res sbr ilk 文件，请稍等......
pause


del /f /s /q .\*.obj
del /f /s /q .\*.pch
del /f /s /q .\*.ipch
del /f /s /q .\*.sdf
del /f /s /q .\*.opensdf
del /f /s /q .\*.suo
del /f /s /q .\*.user
del /f /s /q .\*.tlog
del /f /s /q .\*.exp
del /f /s /q .\*.idb
del /f /s /q .\*.pdb
del /f /s /q .\*.ncb 
del /f /s /q .\*.opt 
del /f /s /q .\*.plg
del /f /s /q .\*.res
del /f /s /q .\*.sbr
del /f /s /q .\*.ilk
del /f /s /q .\*.dep
del /f /s /q .\*.manifest
del /f /s /q .\*.ib_tag
del /f /s /q .\*.ib_pdb_index
del /f /s /q buildlog.htm

rd  /s /q 	".\NDClientNetIOTest/Debug"
rd  /s /q 	".\NDDataServer/Debug"
rd  /s /q 	".\NDGameServer/Debug"
rd  /s /q 	".\NDGatewayServer/Debug"
rd  /s /q 	".\NDLoginServer/Debug"
rd  /s /q 	".\NDLogServer/Debug"
rd  /s /q 	".\NDProtocol/Debug"
rd  /s /q 	".\NDRoomServer/Debug"
rd  /s /q 	".\NDServerNetIOTest/Debug"
rd  /s /q 	".\NDServerShare/Debug"
rd  /s /q 	".\NDShareBase/Debug"
rd  /s /q 	".\NDWorldServer/Debug"
rd  /s /q 	".\Tool\ExcelToTxt/Debug"

rd  /s /q 	".\NDClientNetIOTest/UnityDebug"
rd  /s /q 	".\NDDataServer/UnityDebug"
rd  /s /q 	".\NDGameServer/UnityDebug"
rd  /s /q 	".\NDGatewayServer/UnityDebug"
rd  /s /q 	".\NDLoginServer/UnityDebug"
rd  /s /q 	".\NDLogServer/UnityDebug"
rd  /s /q 	".\NDProtocol/UnityDebug"
rd  /s /q 	".\NDRoomServer/UnityDebug"
rd  /s /q 	".\NDServerNetIOTest/UnityDebug"
rd  /s /q 	".\NDServerShare/UnityDebug"
rd  /s /q 	".\NDShareBase/UnityDebug"
rd  /s /q 	".\NDWorldServer/UnityDebug"
rd  /s /q 	".\Tool\ExcelToTxt/UnityDebug"

rd  /s /q 	".\NDClientNetIOTest/Release"
rd  /s /q 	".\NDDataServer/Release"
rd  /s /q 	".\NDGameServer/Release"
rd  /s /q 	".\NDGatewayServer/Release"
rd  /s /q 	".\NDLoginServer/Release"
rd  /s /q 	".\NDLogServer/Release"
rd  /s /q 	".\NDProtocol/Release"
rd  /s /q 	".\NDRoomServer/Release"
rd  /s /q 	".\NDServerNetIOTest/Release"
rd  /s /q 	".\NDServerShare/Release"
rd  /s /q 	".\NDShareBase/Release"
rd  /s /q 	".\NDWorldServer/Release"
rd  /s /q 	".\Tool\ExcelToTxt/Release"


del /f /s /q "DebugBin\NDDataServer.exe"
del /f /s /q "DebugBin\NDGameServer.exe"
del /f /s /q "DebugBin\NDGatewayServer.exe"
del /f /s /q "DebugBin\NDLoginServer.exe"
del /f /s /q "DebugBin\NDLogServer.exe"
del /f /s /q "DebugBin\NDRoomServer.exe"
del /f /s /q "DebugBin\NDWorldServer.exe"
del /f /s /q "DebugBin\testnetio\NDClientNetIOTest.exe"
del /f /s /q "DebugBin\testnetio\NDServerNetIOTest.exe"
del /f /s /q "Tool\DebugBin\ExcelToTxt\ExcelToTxt.exe"

del /f /s /q "DebugBin\lib\NDProtocol.lib"
del /f /s /q "DebugBin\lib\NDServerShare.lib"
del /f /s /q "DebugBin\lib\NDShareBase.lib"


del /f /s /q "UnityDebugBin\NDDataServer.exe"
del /f /s /q "UnityDebugBin\NDGameServer.exe"
del /f /s /q "UnityDebugBin\NDGatewayServer.exe"
del /f /s /q "UnityDebugBin\NDLoginServer.exe"
del /f /s /q "UnityDebugBin\NDLogServer.exe"
del /f /s /q "UnityDebugBin\NDRoomServer.exe"
del /f /s /q "UnityDebugBin\NDWorldServer.exe"
del /f /s /q "UnityDebugBin\testnetio\NDClientNetIOTest.exe"
del /f /s /q "UnityDebugBin\testnetio\NDServerNetIOTest.exe"
del /f /s /q "Tool\UnityDebugBin\ExcelToTxt\ExcelToTxt.exe"

del /f /s /q "UnityDebugBin\lib\NDProtocol.lib"
del /f /s /q "UnityDebugBin\lib\NDServerShare.lib"
del /f /s /q "UnityDebugBin\lib\NDShareBase.lib"


echo 清除文件完成！
echo. & pause