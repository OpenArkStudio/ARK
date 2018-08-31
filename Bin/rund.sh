echo Copy common dlls...
cp -a bin/PluginLoader_d master/
cp -a bin/PluginLoader_d world/
cp -a bin/PluginLoader_d login/
cp -a bin/PluginLoader_d proxy/
cp -a bin/PluginLoader_d game/

export LC_ALL="C"

#The default zone is 1001

cd master
echo Starting master...
./PluginLoader_d -d --busid=1.0.1.1 --name=master_server --plugin=plugin.xml

cd ..
cd world
echo Starting world...
./PluginLoader_d -d --busid=1.1.100.1 --name=world_server --plugin=plugin.xml

cd ..
cd login
echo Starting login...
./PluginLoader_d -d --busid=1.1.102.1 --name=login_server --plugin=plugin.xml

cd ..
cd proxy
echo Starting proxy...
./PluginLoader_d -d --busid=1.1.103.1 --name=proxy_server --plugin=plugin.xml

cd ..
cd game
echo Starting game...
./PluginLoader_d -d --busid=1.1.101.1 --name=game_server --plugin=plugin.xml

echo All processes are launching...

ps ax | grep *_server