cd /d %~dp0

@echo off
copy ..\\..\\..\\Bin\\Comm\\Debug\\PluginLoader_d.exe AFLoginServer\\ /Y
copy ..\\..\\..\\Bin\\Comm\\Debug\\PluginLoader_d.exe AFMasterServer\\ /Y
copy ..\\..\\..\\Bin\\Comm\\Debug\\PluginLoader_d.exe AFProxyServer\\ /Y
copy ..\\..\\..\\Bin\\Comm\\Debug\\PluginLoader_d.exe AFWorldServer\\ /Y
copy ..\\..\\..\\Bin\\Comm\\Debug\\PluginLoader_d.exe AFGameServer\\ /Y

copy ..\\..\\..\\Dep\\lib\\Debug\\libproto*d.dll AFLoginServer\\ /Y
copy ..\\..\\..\\Dep\\lib\\Debug\\libproto*d.dll AFMasterServer\\ /Y
copy ..\\..\\..\\Dep\\lib\\Debug\\libproto*d.dll AFProxyServer\\ /Y
copy ..\\..\\..\\Dep\\lib\\Debug\\libproto*d.dll AFWorldServer\\ /Y
copy ..\\..\\..\\Dep\\lib\\Debug\\libproto*d.dll AFGameServer\\ /Y

cd  AFMasterServer

echo Starting AFMasterServer...
start "" "PluginLoader_d.exe"

choice /t 2 /d y /n >nul
cd  ..
cd  AFWorldServer
echo Starting AFWorldServer...
start "" "PluginLoader_d.exe"


choice /t 5 /d y /n >nul

cd  ..
cd  AFLoginServer
echo Starting AFLoginServer...
start "" "PluginLoader_d.exe"

choice /t 2 /d y /n >nul

cd  ..
cd  AFGameServer
echo Starting AFGameServer...
start "" "PluginLoader_d.exe"

choice /t 4 /d y /n >nul

cd  ..
cd  AFProxyServer
echo Starting AFProxyServer...
start "" "PluginLoader_d.exe"






