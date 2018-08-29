cd /d %~dp0

@echo off

echo Copy common dlls...

copy ..\\..\\Bin\\bin\\PluginLoader_d.exe master\\ /Y
copy ..\\..\\Bin\\bin\\PluginLoader_d.exe world\\ /Y
copy ..\\..\\Bin\\bin\\PluginLoader_d.exe login\\ /Y
copy ..\\..\\Bin\\bin\\PluginLoader_d.exe proxy\\ /Y
copy ..\\..\\Bin\\bin\\PluginLoader_d.exe game\\ /Y

copy ..\\..\\Dep\\lib\\libproto*d.dll master\\ /Y
copy ..\\..\\Dep\\lib\\libproto*d.dll world\\ /Y
copy ..\\..\\Dep\\lib\\libproto*d.dll login\\ /Y
copy ..\\..\\Dep\\lib\\libproto*d.dll proxy\\ /Y
copy ..\\..\\Dep\\lib\\libproto*d.dll game\\ /Y

copy ..\\..\\Bin\\bin\\AFProto*_d.dll master\\ /Y
copy ..\\..\\Bin\\bin\\AFProto*_d.dll world\\ /Y
copy ..\\..\\Bin\\bin\\AFProto*_d.dll login\\ /Y
copy ..\\..\\Bin\\bin\\AFProto*_d.dll proxy\\ /Y
copy ..\\..\\Bin\\bin\\AFProto*_d.dll game\\ /Y

copy ..\\..\\Bin\\bin\\AFKernelPlugin*_d.dll master\\ /Y
copy ..\\..\\Bin\\bin\\AFKernelPlugin*_d.dll world\\ /Y
copy ..\\..\\Bin\\bin\\AFKernelPlugin*_d.dll login\\ /Y
copy ..\\..\\Bin\\bin\\AFKernelPlugin*_d.dll proxy\\ /Y
copy ..\\..\\Bin\\bin\\AFKernelPlugin*_d.dll game\\ /Y

copy ..\\..\\Bin\\bin\\AFUtilityPlugin*_d.dll master\\ /Y
copy ..\\..\\Bin\\bin\\AFUtilityPlugin*_d.dll world\\ /Y
copy ..\\..\\Bin\\bin\\AFUtilityPlugin*_d.dll login\\ /Y
copy ..\\..\\Bin\\bin\\AFUtilityPlugin*_d.dll proxy\\ /Y
copy ..\\..\\Bin\\bin\\AFUtilityPlugin*_d.dll game\\ /Y

echo Copy self dlls
copy ..\\..\\Bin\\bin\\AFMaster*_d.dll	master\\ /Y
copy ..\\..\\Bin\\bin\\AFWorld*_d.dll	world\\ /Y
copy ..\\..\\Bin\\bin\\AFLogin*_d.dll	login\\ /Y
copy ..\\..\\Bin\\bin\\AFProxy*_d.dll	proxy\\ /Y
copy ..\\..\\Bin\\bin\\AFGame*_d.dll	game\\ /Y

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





