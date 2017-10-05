cd /d %~dp0

@echo off
copy ..\\..\\..\\_Out\\Comm\\Release\\PluginLoader.exe AFLoginServer\\ /Y
copy ..\\..\\..\\_Out\\Comm\\Release\\PluginLoader.exe AFMasterServer\\ /Y
copy ..\\..\\..\\_Out\\Comm\\Release\\PluginLoader.exe AFProxyServer1\\ /Y
copy ..\\..\\..\\_Out\\Comm\\Release\\PluginLoader.exe AFWorldServer\\ /Y
copy ..\\..\\..\\_Out\\Comm\\Release\\PluginLoader.exe AFGameServer1\\ /Y

copy ..\\..\\..\\Dep\\lib\\Release\\libproto*.dll AFLoginServer\\ /Y
copy ..\\..\\..\\Dep\\lib\\Release\\libproto*.dll AFMasterServer\\ /Y
copy ..\\..\\..\\Dep\\lib\\Release\\libproto*.dll AFProxyServer\\ /Y
copy ..\\..\\..\\Dep\\lib\\Release\\libproto*.dll AFWorldServer\\ /Y
copy ..\\..\\..\\Dep\\lib\\Release\\libproto*.dll AFGameServer\\ /Y

cd  AFMasterServer

echo Starting AFMasterServer...
start "" "PluginLoader.exe"

choice /t 2 /d y /n >nul
cd  ..
cd  AFWorldServer
echo Starting AFWorldServer...
start "" "PluginLoader.exe"


choice /t 5 /d y /n >nul

cd  ..
cd  AFLoginServer
echo Starting AFLoginServer...
start "" "PluginLoader.exe"

choice /t 2 /d y /n >nul

cd  ..
cd  AFGameServer
echo Starting AFGameServer...
start "" "PluginLoader.exe"

choice /t 4 /d y /n >nul

cd  ..
cd  AFProxyServer
echo Starting AFProxyServer...
start "" "PluginLoader.exe"






