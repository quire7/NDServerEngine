@echo off
echo kill server ...

taskkill /f /IM NDGatewayServer.exe /T &
@ping localhost -n 3 >nul
taskkill /f /IM NDRoomServer.exe /T &
@ping localhost -n 3 >nul
taskkill /f /IM NDGameServer.exe /T &
@ping localhost -n 3 >nul
taskkill /f /IM NDLoginServer.exe /T &
@ping localhost -n 3 >nul
taskkill /f /IM NDWorldServer.exe /T &
@ping localhost -n 10 >nul
taskkill /f /IM NDDataServer.exe /T &
@ping localhost -n 190 >nul
taskkill /f /IM NDLogServer.exe /T &