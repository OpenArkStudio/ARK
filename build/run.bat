cd /d %~dp0

@echo off

REM the default zone is 1001

set CHANNEL_ID=1
set ZONE_ID=1

cd Release

echo Starting master...
start "master" bin/app.exe -x --busid=1.0.1.1 --name=master_server --app_conf=plugin_conf/master.app.conf
choice /t 5 /d y /n >nul

echo Starting world...
start "world" bin/app.exe -x --busid=1.1.30.1 --name=world_server --app_conf=plugin_conf/world.app.conf
choice /t 5 /d y /n >nul

echo Starting login...
start "login" bin/app.exe -x --busid=1.1.31.1 --name=login_server --app_conf=plugin_conf/login.app.conf
choice /t 5 /d y /n >nul

echo Starting proxy...
start "proxy" bin/app.exe -x --busid=1.1.32.1 --name=proxy_server --app_conf=plugin_conf/proxy.app.conf
choice /t 5 /d y /n >nul

echo Starting game...
start "game" bin/app.exe -x --busid=1.1.33.1 --name=game_server --app_conf=plugin_conf/game.app.conf
choice /t 5 /d y /n >nul

echo All processes are launching...

cd 