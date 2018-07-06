cd /d %~dp0

@echo off

echo Copy common dlls...
copy ..\\..\\..\\Bin\\Comm\\Release\\PluginLoader.exe master\\ /Y
copy ..\\..\\..\\Bin\\Comm\\Release\\PluginLoader.exe world\\ /Y
copy ..\\..\\..\\Bin\\Comm\\Release\\PluginLoader.exe login\\ /Y
copy ..\\..\\..\\Bin\\Comm\\Release\\PluginLoader.exe proxy\\ /Y
copy ..\\..\\..\\Bin\\Comm\\Release\\PluginLoader.exe game\\ /Y

copy ..\\..\\..\\Dep\\lib\\Release\\libproto*.dll master\\ /Y
copy ..\\..\\..\\Dep\\lib\\Release\\libproto*.dll world\\ /Y
copy ..\\..\\..\\Dep\\lib\\Release\\libproto*.dll login\\ /Y
copy ..\\..\\..\\Dep\\lib\\Release\\libproto*.dll proxy\\ /Y
copy ..\\..\\..\\Dep\\lib\\Release\\libproto*.dll game\\ /Y

copy ..\\..\\..\\Bin\\Comm\\Release\\AFProto*.dll master\\ /Y
copy ..\\..\\..\\Bin\\Comm\\Release\\AFProto*.dll world\\ /Y
copy ..\\..\\..\\Bin\\Comm\\Release\\AFProto*.dll login\\ /Y
copy ..\\..\\..\\Bin\\Comm\\Release\\AFProto*.dll proxy\\ /Y
copy ..\\..\\..\\Bin\\Comm\\Release\\AFProto*.dll game\\ /Y

copy ..\\..\\..\\Bin\\Comm\\Release\\AFKernelPlugin*.dll master\\ /Y
copy ..\\..\\..\\Bin\\Comm\\Release\\AFKernelPlugin*.dll world\\ /Y
copy ..\\..\\..\\Bin\\Comm\\Release\\AFKernelPlugin*.dll login\\ /Y
copy ..\\..\\..\\Bin\\Comm\\Release\\AFKernelPlugin*.dll proxy\\ /Y
copy ..\\..\\..\\Bin\\Comm\\Release\\AFKernelPlugin*.dll game\\ /Y

copy ..\\..\\..\\Bin\\Comm\\Release\\AFUtilityPlugin*.dll master\\ /Y
copy ..\\..\\..\\Bin\\Comm\\Release\\AFUtilityPlugin*.dll world\\ /Y
copy ..\\..\\..\\Bin\\Comm\\Release\\AFUtilityPlugin*.dll login\\ /Y
copy ..\\..\\..\\Bin\\Comm\\Release\\AFUtilityPlugin*.dll proxy\\ /Y
copy ..\\..\\..\\Bin\\Comm\\Release\\AFUtilityPlugin*.dll game\\ /Y

echo Copy self dlls
copy ..\\..\\..\\Bin\\Comm\\Release\\AFMaster*.dll	master\\ /Y
copy ..\\..\\..\\Bin\\Comm\\Release\\AFWorld*.dll	world\\ /Y
copy ..\\..\\..\\Bin\\Comm\\Release\\AFLogin*.dll	login\\ /Y
copy ..\\..\\..\\Bin\\Comm\\Release\\AFProxy*.dll	proxy\\ /Y
copy ..\\..\\..\\Bin\\Comm\\Release\\AFGame*.dll	game\\ /Y


cd  master
echo Starting master...
start "" PluginLoader.exe -x app_id=1 app_name=master_server cfg=Plugin.xml
choice /t 2 /d y /n >nul

cd  ..
cd  world
echo Starting world...
start "" PluginLoader.exe -x app_id=50 app_name=world_server cfg=Plugin.xml
choice /t 5 /d y /n >nul

cd  ..
cd  login
echo Starting login...
start "" PluginLoader.exe -x app_id=52 app_name=login_server cfg=Plugin.xml
choice /t 2 /d y /n >nul

cd  ..
cd  proxy
echo Starting proxy...
start "" PluginLoader.exe -x app_id=53 app_name=proxy_server cfg=Plugin.xml

cd  ..
cd  game
echo Starting game...
start "" PluginLoader.exe -x app_id=51 app_name=game_server cfg=Plugin.xml
choice /t 4 /d y /n >nul

echo All processes are launching...