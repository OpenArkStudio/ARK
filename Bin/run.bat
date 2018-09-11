cd /d %~dp0

@echo off

REM the default zone is 1001

set CHANNEL_ID=1
set ZONE_ID=1

set PATH=..\lib\

cd bin

echo Starting master...
start "master" Launcher.exe -x --busid=%CHANNEL_ID%.0.1.1 --name=master_server --plugin=../plugin_conf/master.plugin --logpath=../binlog
choice /t 5 /d y /n >nul

echo Starting world...
start "world" Launcher.exe -x --busid=%CHANNEL_ID%.%ZONE_ID%.100.1 --name=world_server --plugin=../plugin_conf/world.plugin --logpath=../binlog
choice /t 5 /d y /n >nul

echo Starting login...
start "login" Launcher.exe -x --busid=%CHANNEL_ID%.%ZONE_ID%.102.1 --name=login_server --plugin=../plugin_conf/login.plugin --logpath=../binlog
choice /t 5 /d y /n >nul

echo Starting proxy...
start "proxy" Launcher.exe -x --busid=%CHANNEL_ID%.%ZONE_ID%.103.1 --name=proxy_server --plugin=../plugin_conf/proxy.plugin --logpath=../binlog
choice /t 5 /d y /n >nul

echo Starting game...
start "game" Launcher.exe -x --busid=%CHANNEL_ID%.%ZONE_ID%.101.1 --name=game_server --plugin=../plugin_conf/game.plugin --logpath=../binlog
choice /t 5 /d y /n >nul

echo All processes are launching...

cd ../