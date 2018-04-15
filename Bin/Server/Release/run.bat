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

echo Copy common dlls...
copy ..\\..\\..\\Bin\\Comm\\Release\\PluginLoader.exe AFLoginServer\\ /Y
copy ..\\..\\..\\Bin\\Comm\\Release\\PluginLoader.exe AFMasterServer\\ /Y
copy ..\\..\\..\\Bin\\Comm\\Release\\PluginLoader.exe AFProxyServer\\ /Y
copy ..\\..\\..\\Bin\\Comm\\Release\\PluginLoader.exe AFWorldServer\\ /Y
copy ..\\..\\..\\Bin\\Comm\\Release\\PluginLoader.exe AFGameServer\\ /Y

copy ..\\..\\..\\Dep\\lib\\Release\\libproto*.dll AFLoginServer\\ /Y
copy ..\\..\\..\\Dep\\lib\\Release\\libproto*.dll AFMasterServer\\ /Y
copy ..\\..\\..\\Dep\\lib\\Release\\libproto*.dll AFProxyServer\\ /Y
copy ..\\..\\..\\Dep\\lib\\Release\\libproto*.dll AFWorldServer\\ /Y
copy ..\\..\\..\\Dep\\lib\\Release\\libproto*.dll AFGameServer\\ /Y

copy ..\\..\\..\\Bin\\Comm\\Release\\AFProto*.dll AFLoginServer\\ /Y
copy ..\\..\\..\\Bin\\Comm\\Release\\AFProto*.dll AFMasterServer\\ /Y
copy ..\\..\\..\\Bin\\Comm\\Release\\AFProto*.dll AFProxyServer\\ /Y
copy ..\\..\\..\\Bin\\Comm\\Release\\AFProto*.dll AFWorldServer\\ /Y
copy ..\\..\\..\\Bin\\Comm\\Release\\AFProto*.dll AFGameServer\\ /Y

copy ..\\..\\..\\Bin\\Comm\\Release\\AFKernelPlugin*.dll AFLoginServer\\ /Y
copy ..\\..\\..\\Bin\\Comm\\Release\\AFKernelPlugin*.dll AFMasterServer\\ /Y
copy ..\\..\\..\\Bin\\Comm\\Release\\AFKernelPlugin*.dll AFProxyServer\\ /Y
copy ..\\..\\..\\Bin\\Comm\\Release\\AFKernelPlugin*.dll AFWorldServer\\ /Y
copy ..\\..\\..\\Bin\\Comm\\Release\\AFKernelPlugin*.dll AFGameServer\\ /Y

copy ..\\..\\..\\Bin\\Comm\\Release\\AFUtilityPlugin*.dll AFLoginServer\\ /Y
copy ..\\..\\..\\Bin\\Comm\\Release\\AFUtilityPlugin*.dll AFMasterServer\\ /Y
copy ..\\..\\..\\Bin\\Comm\\Release\\AFUtilityPlugin*.dll AFProxyServer\\ /Y
copy ..\\..\\..\\Bin\\Comm\\Release\\AFUtilityPlugin*.dll AFWorldServer\\ /Y
copy ..\\..\\..\\Bin\\Comm\\Release\\AFUtilityPlugin*.dll AFGameServer\\ /Y

echo Copy self dlls
copy ..\\..\\..\\Bin\\Comm\\Release\\AFLogin*.dll AFLoginServer\\ /Y
copy ..\\..\\..\\Bin\\Comm\\Release\\AFMaster*.dll AFMasterServer\\ /Y
copy ..\\..\\..\\Bin\\Comm\\Release\\AFProxy*.dll AFProxyServer\\ /Y
copy ..\\..\\..\\Bin\\Comm\\Release\\AFWorld*.dll AFWorldServer\\ /Y
copy ..\\..\\..\\Bin\\Comm\\Release\\AFGame*.dll AFGameServer\\ /Y


cd  AFMasterServer
echo Starting AFMasterServer...
start "" PluginLoader.exe -x app_id=3 app_name=MasterServer cfg=Plugin.xml
choice /t 2 /d y /n >nul

cd  ..
cd  AFWorldServer
echo Starting AFWorldServer...
start "" PluginLoader.exe -x app_id=7 app_name=WorldServer cfg=Plugin.xml
choice /t 5 /d y /n >nul

cd  ..
cd  AFLoginServer
echo Starting AFLoginServer...
start "" PluginLoader.exe -x app_id=4 app_name=LoginServer cfg=Plugin.xml
choice /t 2 /d y /n >nul

cd  ..
cd  AFGameServer
echo Starting AFGameServer...
start "" PluginLoader.exe -x app_id=6 app_name=GameServer cfg=Plugin.xml
choice /t 4 /d y /n >nul

cd  ..
cd  AFProxyServer
echo Starting AFProxyServer...
start "" PluginLoader.exe -x app_id=5 app_name=ProxyServer cfg=Plugin.xml


echo All processes are launching...