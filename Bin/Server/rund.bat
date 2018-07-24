cd /d %~dp0

@echo off

echo Copy common dlls...

copy ..\\..\\Bin\\Comm\\Debug\\PluginLoader_d.exe master\\ /Y
copy ..\\..\\Bin\\Comm\\Debug\\PluginLoader_d.exe world\\ /Y
copy ..\\..\\Bin\\Comm\\Debug\\PluginLoader_d.exe login\\ /Y
copy ..\\..\\Bin\\Comm\\Debug\\PluginLoader_d.exe proxy\\ /Y
copy ..\\..\\Bin\\Comm\\Debug\\PluginLoader_d.exe game\\ /Y

copy ..\\..\\Dep\\lib\\Debug\\libproto*d.dll master\\ /Y
copy ..\\..\\Dep\\lib\\Debug\\libproto*d.dll world\\ /Y
copy ..\\..\\Dep\\lib\\Debug\\libproto*d.dll login\\ /Y
copy ..\\..\\Dep\\lib\\Debug\\libproto*d.dll proxy\\ /Y
copy ..\\..\\Dep\\lib\\Debug\\libproto*d.dll game\\ /Y

copy ..\\..\\Bin\\Comm\\Debug\\AFProto*d.dll master\\ /Y
copy ..\\..\\Bin\\Comm\\Debug\\AFProto*d.dll world\\ /Y
copy ..\\..\\Bin\\Comm\\Debug\\AFProto*d.dll login\\ /Y
copy ..\\..\\Bin\\Comm\\Debug\\AFProto*d.dll proxy\\ /Y
copy ..\\..\\Bin\\Comm\\Debug\\AFProto*d.dll game\\ /Y

copy ..\\..\\Bin\\Comm\\Debug\\AFKernelPlugin*d.dll master\\ /Y
copy ..\\..\\Bin\\Comm\\Debug\\AFKernelPlugin*d.dll world\\ /Y
copy ..\\..\\Bin\\Comm\\Debug\\AFKernelPlugin*d.dll login\\ /Y
copy ..\\..\\Bin\\Comm\\Debug\\AFKernelPlugin*d.dll proxy\\ /Y
copy ..\\..\\Bin\\Comm\\Debug\\AFKernelPlugin*d.dll game\\ /Y

copy ..\\..\\Bin\\Comm\\Debug\\AFUtilityPlugin*d.dll master\\ /Y
copy ..\\..\\Bin\\Comm\\Debug\\AFUtilityPlugin*d.dll world\\ /Y
copy ..\\..\\Bin\\Comm\\Debug\\AFUtilityPlugin*d.dll login\\ /Y
copy ..\\..\\Bin\\Comm\\Debug\\AFUtilityPlugin*d.dll proxy\\ /Y
copy ..\\..\\Bin\\Comm\\Debug\\AFUtilityPlugin*d.dll game\\ /Y

echo Copy self dlls
copy ..\\..\\Bin\\Comm\\Debug\\AFMaster*d.dll	master\\ /Y
copy ..\\..\\Bin\\Comm\\Debug\\AFWorld*d.dll	world\\ /Y
copy ..\\..\\Bin\\Comm\\Debug\\AFLogin*d.dll	login\\ /Y
copy ..\\..\\Bin\\Comm\\Debug\\AFProxy*d.dll	proxy\\ /Y
copy ..\\..\\Bin\\Comm\\Debug\\AFGame*d.dll		game\\ /Y

cd  master
echo Starting master...
start "" PluginLoader_d.exe -x app_id=1 app_name=master_server cfg=Plugin.xml
choice /t 2 /d y /n >nul

cd  ..
cd  world
echo Starting world...
start "" PluginLoader_d.exe -x app_id=100 app_name=world_server cfg=Plugin.xml
choice /t 5 /d y /n >nul

cd  ..
cd  login
echo Starting login...
start "" PluginLoader_d.exe -x app_id=102 app_name=login_server cfg=Plugin.xml
choice /t 2 /d y /n >nul

cd  ..
cd  proxy
echo Starting proxy...
start "" PluginLoader_d.exe -x app_id=103 app_name=proxy_server cfg=Plugin.xml

cd  ..
cd  game
echo Starting game...
start "" PluginLoader_d.exe -x app_id=101 app_name=game_server cfg=Plugin.xml
choice /t 4 /d y /n >nul

echo All processes are launching...





