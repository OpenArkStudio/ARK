cd /d %~dp0

@echo off

echo Copy common dlls...
copy bin\\PluginLoader.exe master\\ /Y
copy bin\\PluginLoader.exe world\\ /Y
copy bin\\PluginLoader.exe login\\ /Y
copy bin\\PluginLoader.exe proxy\\ /Y
copy bin\\PluginLoader.exe game\\ /Y

copy ..\\Dep\\lib\\libprotobuf.dll master\\ /Y
copy ..\\Dep\\lib\\libprotobuf.dll world\\ /Y
copy ..\\Dep\\lib\\libprotobuf.dll login\\ /Y
copy ..\\Dep\\lib\\libprotobuf.dll proxy\\ /Y
copy ..\\Dep\\lib\\libprotobuf.dll game\\ /Y

copy bin\\AFProto.dll master\\ /Y
copy bin\\AFProto.dll world\\ /Y
copy bin\\AFProto.dll login\\ /Y
copy bin\\AFProto.dll proxy\\ /Y
copy bin\\AFProto.dll game\\ /Y

copy /Y bin\\AFNet.dll master\\
copy /Y bin\\AFNet.dll world\\
copy /Y bin\\AFNet.dll login\\
copy /Y bin\\AFNet.dll proxy\\
copy /Y bin\\AFNet.dll game\\

REM the default zone is 1001

cd  master
echo Starting master...
start "" PluginLoader.exe -x --busid=1.0.1.1 --name=master_server --plugin=plugin.xml
choice /t 2 /d y /n >nul

cd  ..
cd  world
echo Starting world...
start "" PluginLoader.exe -x --busid=1.1.100.1 --name=world_server --plugin=plugin.xml
choice /t 5 /d y /n >nul

cd  ..
cd  login
echo Starting login...
start "" PluginLoader.exe -x --busid=1.1.102.1 --name=login_server --plugin=plugin.xml
choice /t 2 /d y /n >nul

cd  ..
cd  proxy
echo Starting proxy...
start "" PluginLoader.exe -x --busid=1.1.103.1 --name=proxy_server --plugin=plugin.xml

cd  ..
cd  game
echo Starting game...
start "" PluginLoader.exe -x --busid=1.1.101.1 --name=game_server --plugin=plugin.xml
choice /t 4 /d y /n >nul

echo All processes are launching...