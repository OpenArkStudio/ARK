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
start "" PluginLoader_d.exe -x --busid=1.1.1.1 --name=master_server plugin=Plugin.xml
choice /t 2 /d y /n >nul

cd  ..
cd  world
echo Starting world...
start "" PluginLoader_d.exe -x --busid=1.1.100.1 --name=world_server plugin=Plugin.xml
choice /t 5 /d y /n >nul

cd  ..
cd  login
echo Starting login...
start "" PluginLoader_d.exe -x --busid=1.1.102.1 --name=login_server plugin=Plugin.xml
choice /t 2 /d y /n >nul

cd  ..
cd  proxy
echo Starting proxy...
start "" PluginLoader_d.exe -x --busid=1.1.103.1 --name=proxy_server plugin=Plugin.xml

cd  ..
cd  game
echo Starting game...
start "" PluginLoader_d.exe -x --busid=1.1.101.1 --name=game_server plugin=Plugin.xml
choice /t 4 /d y /n >nul

echo All processes are launching...





