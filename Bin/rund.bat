cd /d %~dp0

@echo off

echo Copy common dlls...

copy /Y bin\\PluginLoader_d.exe master\\
copy /Y bin\\PluginLoader_d.exe world\\
copy /Y bin\\PluginLoader_d.exe login\\
copy /Y bin\\PluginLoader_d.exe proxy\\
copy /Y bin\\PluginLoader_d.exe game\\

copy /Y ..\\Dep\\lib\\libprotobufd.dll master\\
copy /Y ..\\Dep\\lib\\libprotobufd.dll world\\
copy /Y ..\\Dep\\lib\\libprotobufd.dll login\\
copy /Y ..\\Dep\\lib\\libprotobufd.dll proxy\\
copy /Y ..\\Dep\\lib\\libprotobufd.dll game\\

copy /Y bin\\AFProto_d.dll master\\
copy /Y bin\\AFProto_d.dll world\\
copy /Y bin\\AFProto_d.dll login\\
copy /Y bin\\AFProto_d.dll proxy\\
copy /Y bin\\AFProto_d.dll game\\

copy /Y bin\\AFNet_d.dll master\\
copy /Y bin\\AFNet_d.dll world\\
copy /Y bin\\AFNet_d.dll login\\
copy /Y bin\\AFNet_d.dll proxy\\
copy /Y bin\\AFNet_d.dll game\\

REM the default zone is 1001

cd  master
echo Starting master...
start "" PluginLoader_d.exe -x --busid=1.0.1.1 --name=master_server --plugin=plugin.xml
choice /t 2 /d y /n >nul

cd  ..
cd  world
echo Starting world...
start "" PluginLoader_d.exe -x --busid=1.1.100.1 --name=world_server --plugin=plugin.xml
choice /t 5 /d y /n >nul

cd  ..
cd  login
echo Starting login...
start "" PluginLoader_d.exe -x --busid=1.1.102.1 --name=login_server --plugin=plugin.xml
choice /t 2 /d y /n >nul

cd  ..
cd  proxy
echo Starting proxy...
start "" PluginLoader_d.exe -x --busid=1.1.103.1 --name=proxy_server --plugin=plugin.xml

cd  ..
cd  game
echo Starting game...
start "" PluginLoader_d.exe -x --busid=1.1.101.1 --name=game_server --plugin=plugin.xml
choice /t 4 /d y /n >nul

echo All processes are launching...