cd /d %~dp0

@echo off

echo Copy common dlls...
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

copy ..\\..\\..\\Bin\\Comm\\Debug\\AFProto*d.dll AFLoginServer\\ /Y
copy ..\\..\\..\\Bin\\Comm\\Debug\\AFProto*d.dll AFMasterServer\\ /Y
copy ..\\..\\..\\Bin\\Comm\\Debug\\AFProto*d.dll AFProxyServer\\ /Y
copy ..\\..\\..\\Bin\\Comm\\Debug\\AFProto*d.dll AFWorldServer\\ /Y
copy ..\\..\\..\\Bin\\Comm\\Debug\\AFProto*d.dll AFGameServer\\ /Y

copy ..\\..\\..\\Bin\\Comm\\Debug\\AFKernelPlugin*d.dll AFLoginServer\\ /Y
copy ..\\..\\..\\Bin\\Comm\\Debug\\AFKernelPlugin*d.dll AFMasterServer\\ /Y
copy ..\\..\\..\\Bin\\Comm\\Debug\\AFKernelPlugin*d.dll AFProxyServer\\ /Y
copy ..\\..\\..\\Bin\\Comm\\Debug\\AFKernelPlugin*d.dll AFWorldServer\\ /Y
copy ..\\..\\..\\Bin\\Comm\\Debug\\AFKernelPlugin*d.dll AFGameServer\\ /Y

copy ..\\..\\..\\Bin\\Comm\\Debug\\AFUtilityPlugin*d.dll AFLoginServer\\ /Y
copy ..\\..\\..\\Bin\\Comm\\Debug\\AFUtilityPlugin*d.dll AFMasterServer\\ /Y
copy ..\\..\\..\\Bin\\Comm\\Debug\\AFUtilityPlugin*d.dll AFProxyServer\\ /Y
copy ..\\..\\..\\Bin\\Comm\\Debug\\AFUtilityPlugin*d.dll AFWorldServer\\ /Y
copy ..\\..\\..\\Bin\\Comm\\Debug\\AFUtilityPlugin*d.dll AFGameServer\\ /Y

echo Copy self dlls
copy ..\\..\\..\\Bin\\Comm\\Debug\\AFLogin*d.dll AFLoginServer\\ /Y
copy ..\\..\\..\\Bin\\Comm\\Debug\\AFMaster*d.dll AFMasterServer\\ /Y
copy ..\\..\\..\\Bin\\Comm\\Debug\\AFProxy*d.dll AFProxyServer\\ /Y
copy ..\\..\\..\\Bin\\Comm\\Debug\\AFWorld*d.dll AFWorldServer\\ /Y
copy ..\\..\\..\\Bin\\Comm\\Debug\\AFGame*d.dll AFGameServer\\ /Y

cd  AFMasterServer
echo Starting AFMasterServer...
start "" "PluginLoader_d.exe -x app_id=3 app_name=MasterServer cfg=Plugin.xml"
choice /t 2 /d y /n >nul

cd  ..
cd  AFWorldServer
echo Starting AFWorldServer...
start "" "PluginLoader_d.exe -x app_id=7 app_name=WorldServer cfg=Plugin.xml"
choice /t 5 /d y /n >nul

cd  ..
cd  AFLoginServer
echo Starting AFLoginServer...
start "" "PluginLoader_d.exe -x app_id=4 app_name=LoginServer cfg=Plugin.xml"
choice /t 2 /d y /n >nul

cd  ..
cd  AFGameServer
echo Starting AFGameServer...
start "" "PluginLoader_d.exe -x app_id=6 app_name=GameServer cfg=Plugin.xml"
choice /t 4 /d y /n >nul

cd  ..
cd  AFProxyServer
echo Starting AFProxyServer...
start "" "PluginLoader_d.exe -x app_id=5 app_name=ProxyServer cfg=Plugin.xml"


echo All processes are launching...





