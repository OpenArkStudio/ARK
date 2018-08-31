echo Copy common dlls...

cp -a bin/PluginLoader master/
cp -a bin/PluginLoader world/
cp -a bin/PluginLoader login/
cp -a bin/PluginLoader proxy/
cp -a bin/PluginLoader game/

export LC_ALL="C"

#The default zone is 1001

cd master
echo Starting master...
./PluginLoader -d --busid=1.0.1.1 --name=master_server --plugin=plugin.xml

cd ..
cd world
echo Starting world...
./PluginLoader -d --busid=1.1.100.1 --name=world_server --plugin=plugin.xml

cd ..
cd login
echo Starting login...
./PluginLoader -d --busid=1.1.102.1 --name=login_server --plugin=plugin.xml

cd ..
cd proxy
echo Starting proxy...
./PluginLoader -d --busid=1.1.103.1 --name=proxy_server --plugin=plugin.xml

cd ..
cd game
echo Starting game...
./PluginLoader -d --busid=1.1.101.1 --name=game_server --plugin=plugin.xml

echo All processes are launching...

ps ax | grep *_server