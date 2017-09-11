cd /d %~dp0

@echo off
copy ..\\..\\..\\_Out\\Comm\\Debug\\NFPluginLoader_d.exe AFLoginServer\\ /Y
copy ..\\..\\..\\_Out\\Comm\\Debug\\AFPluginLoader_d.exe AFMasterServer\\ /Y
copy ..\\..\\..\\_Out\\Comm\\Debug\\AFPluginLoader_d.exe AFProxyServer1\\ /Y
copy ..\\..\\..\\_Out\\Comm\\Debug\\AFPluginLoader_d.exe AFWorldServer\\ /Y
copy ..\\..\\..\\_Out\\Comm\\Debug\\AFPluginLoader_d.exe AFGameServer1\\ /Y

copy ..\\..\\..\\Dependencies\\lib\\Debug\\libmysql.dll AFGameServer1\\ /Y
copy ..\\..\\..\\Dependencies\\lib\\Debug\\libmysql.dll AFLoginServer\\ /Y
copy ..\\..\\..\\Dependencies\\lib\\Debug\\libmysql.dll AFMasterServer\\ /Y
copy ..\\..\\..\\Dependencies\\lib\\Debug\\libmysql.dll AFProxyServer1\\ /Y
copy ..\\..\\..\\Dependencies\\lib\\Debug\\libmysql.dll AFWorldServer\\ /Y

copy ..\\..\\..\\Dependencies\\lib\\Debug\\mysqlpp_d.dll AFGameServer1\\ /Y
copy ..\\..\\..\\Dependencies\\lib\\Debug\\mysqlpp_d.dll AFLoginServer\\ /Y
copy ..\\..\\..\\Dependencies\\lib\\Debug\\mysqlpp_d.dll AFMasterServer\\ /Y
copy ..\\..\\..\\Dependencies\\lib\\Debug\\mysqlpp_d.dll AFProxyServer1\\ /Y
copy ..\\..\\..\\Dependencies\\lib\\Debug\\mysqlpp_d.dll AFWorldServer\\ /Y

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






