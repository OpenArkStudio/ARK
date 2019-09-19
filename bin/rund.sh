cd debug

export LC_ALL="C"

#The default zone is 1001

CHANNEL_ID=1
ZONE_ID=1

echo Starting master...
./bin/app_d -d --busid=1.0.1.1 --name=master_server --plugin=plugin_conf/master.plugin --logpath=binlog

echo Starting world...
./bin/app_d -d --busid=1.1.30.1 --name=world_server --plugin=plugin_conf/world.plugin --logpath=binlog

echo Starting login...
./bin/app_d -d --busid=1.1.31.1 --name=login_server --plugin=plugin_conf/login.plugin --logpath=binlog

echo Starting proxy...
./bin/app_d -d --busid=1.1.32.1 --name=proxy_server --plugin=plugin_conf/proxy.plugin --logpath=binlog

echo Starting game...
./bin/app_d -d --busid=1.1.33.1 --name=game_server --plugin=plugin_conf/game.plugin --logpath=binlog

echo All processes are launching...

ps aux | grep app_d

cd ../