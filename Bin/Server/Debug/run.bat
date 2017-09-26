cd /d %~dp0

@echo off
copy ..\\..\\..\\Bin\\Comm\\Debug\\PluginLoader_d.exe AFLoginServer\\ /Y
copy ..\\..\\..\\Bin\\Comm\\Debug\\PluginLoader_d.exe AFMasterServer\\ /Y
copy ..\\..\\..\\Bin\\Comm\\Debug\\PluginLoader_d.exe AFProxyServer\\ /Y
copy ..\\..\\..\\Bin\\Comm\\Debug\\PluginLoader_d.exe AFWorldServer\\ /Y
copy ..\\..\\..\\Bin\\Comm\\Debug\\PluginLoader_d.exe AFGameServer\\ /Y

rem copy ..\\..\\..\\Dep\\lib\\Debug\\libmysql.dll AFGameServer\\ /Y
rem copy ..\\..\\..\\Dep\\lib\\Debug\\libmysql.dll AFLoginServer\\ /Y
rem copy ..\\..\\..\\Dep\\lib\\Debug\\libmysql.dll AFMasterServer\\ /Y
rem copy ..\\..\\..\\Dep\\lib\\Debug\\libmysql.dll AFProxyServer\\ /Y
rem copy ..\\..\\..\\Dep\\lib\\Debug\\libmysql.dll AFWorldServer\\ /Y

rem copy ..\\..\\..\\Dep\\lib\\Debug\\mysqlpp_d.dll AFGameServer\\ /Y
rem copy ..\\..\\..\\Dep\\lib\\Debug\\mysqlpp_d.dll AFLoginServer\\ /Y
rem copy ..\\..\\..\\Dep\\lib\\Debug\\mysqlpp_d.dll AFMasterServer\\ /Y
rem copy ..\\..\\..\\Dep\\lib\\Debug\\mysqlpp_d.dll AFProxyServer\\ /Y
rem copy ..\\..\\..\\Dep\\lib\\Debug\\mysqlpp_d.dll AFWorldServer\\ /Y

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






