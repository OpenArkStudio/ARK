cd bin

export LC_ALL="C"

#The default zone is 1001

channel_id=1
zone_id=1

echo Starting master...
./Launcher -d --busid=${channel_id}.0.1.1 --name=master_server --plugin=../plugin_conf/master.plugin --logpath=../binlog

echo Starting world...
./Launcher -d --busid=${channel_id}.${zone_id}.100.1 --name=world_server --plugin=../plugin_conf/world.plugin --logpath=../binlog

echo Starting login...
./Launcher -d --busid=${channel_id}.${zone_id}.102.1 --name=login_server --plugin=../plugin_conf/login.plugin --logpath=../binlog

echo Starting proxy...
./Launcher -d --busid=${channel_id}.${zone_id}.103.1 --name=proxy_server --plugin=../plugin_conf/proxy.plugin --logpath=../binlog

echo Starting game...
./Launcher -d --busid=${channel_id}.${zone_id}.101.1 --name=game_server --plugin=../plugin_conf/game.plugin --logpath=../binlog

echo All processes are launching...

ps aux | grep *_server

cd ../