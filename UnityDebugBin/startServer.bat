@echo off
echo start server ...

start NDLogServer.exe &
@ping localhost -n 5 >nul
start NDLoginServer.exe 1 &
@ping localhost -n 3 >nul
start NDWorldServer.exe 1 &
@ping localhost -n 3 >nul
start NDDataServer.exe 1 &
@ping localhost -n 3 >nul
start NDGameServer.exe 1 &
@ping localhost -n 3 >nul
start NDRoomServer.exe 1 &
@ping localhost -n 3 >nul
start NDGatewayServer.exe 1 &
@ping localhost -n 3 >nul