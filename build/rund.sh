cd debug

export LC_ALL="C"

#The default zone is 1001

CHANNEL_ID=1
ZONE_ID=1

echo Starting master...
./bin/app_d -d --busid=1.0.1.1 --name=master_server --app_conf=plugin_conf/master.app.conf

echo Starting world...
./bin/app_d -d --busid=1.1.30.1 --name=world_server --app_conf=plugin_conf/world.app.conf

echo Starting login...
./bin/app_d -d --busid=1.1.31.1 --name=login_server --app_conf=plugin_conf/login.app.conf

echo Starting proxy...
./bin/app_d -d --busid=1.1.32.1 --name=proxy_server --app_conf=plugin_conf/proxy.app.conf

echo Starting game...
./bin/app_d -d --busid=1.1.33.1 --name=game_server --app_conf=plugin_conf/game.app.conf

echo All processes are launching...

ps aux | grep app_d

cd ../