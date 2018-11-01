cd bin

export LC_ALL="C"

#The default zone is 1001

CHANNEL_ID=1
ZONE_ID=1

echo Starting master...
./Launcher_d -d --busid=${CHANNEL_ID}.0.1.1 --name=master_server --plugin=../plugin_conf/master.plugin --logpath=../binlog

echo Starting world...
./Launcher_d -d --busid=${CHANNEL_ID}.${ZONE_ID}.100.1 --name=world_server --plugin=../plugin_conf/world.plugin --logpath=../binlog

echo Starting login...
./Launcher_d -d --busid=${CHANNEL_ID}.${ZONE_ID}.102.1 --name=login_server --plugin=../plugin_conf/login.plugin --logpath=../binlog

echo Starting proxy...
./Launcher_d -d --busid=${CHANNEL_ID}.${ZONE_ID}.103.1 --name=proxy_server --plugin=../plugin_conf/proxy.plugin --logpath=../binlog

echo Starting game...
./Launcher_d -d --busid=${CHANNEL_ID}.${ZONE_ID}.101.1 --name=game_server --plugin=../plugin_conf/game.plugin --logpath=../binlog

echo All processes are launching...

ps aux | grep *_server

cd ../