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


REM choice /t 5 /d y /n >nul

REM cd  ..
REM cd  AFLoginServer
REM echo Starting AFLoginServer...
REM start "" "PluginLoader_d.exe"

REM choice /t 2 /d y /n >nul

REM cd  ..
REM cd  AFGameServer
REM echo Starting AFGameServer...
REM start "" "PluginLoader_d.exe"

REM choice /t 4 /d y /n >nul

REM cd  ..
REM cd  AFProxyServer
REM echo Starting AFProxyServer...
REM start "" "PluginLoader_d.exe"






