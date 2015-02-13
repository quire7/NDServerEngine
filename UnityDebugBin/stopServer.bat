@echo off
echo kill server ...

taskkill /f /IM NDLoginServer.exe /T &
@ping localhost -n 3 >nul
taskkill /f /IM NDWorldServer.exe /T &
@ping localhost -n 5 >nul
taskkill /f /IM NDLogServer.exe /T &