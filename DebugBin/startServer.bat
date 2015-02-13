@echo off
echo start server ...

start NDLogServer.exe &
@ping localhost -n 5 >nul
start NDLoginServer.exe 1 &
@ping localhost -n 3 >nul
start NDWorldServer.exe 1 &
@ping localhost -n 3 >nul