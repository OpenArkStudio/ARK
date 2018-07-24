echo Copy common dlls...

cp -a ../../Bin/Comm/Release/PluginLoader master/
cp -a ../../Bin/Comm/Release/PluginLoader world/
cp -a ../../Bin/Comm/Release/PluginLoader login/
cp -a ../../Bin/Comm/Release/PluginLoader proxy/
cp -a ../../Bin/Comm/Release/PluginLoader game/

cp -a ../../Dep/lib/Release/libproto*.so master/
cp -a ../../Dep/lib/Release/libproto*.so world/
cp -a ../../Dep/lib/Release/libproto*.so login/
cp -a ../../Dep/lib/Release/libproto*.so proxy/
cp -a ../../Dep/lib/Release/libproto*.so game/

cp -a ../../Bin/Comm/Release/AFProto*.so master/
cp -a ../../Bin/Comm/Release/AFProto*.so world/
cp -a ../../Bin/Comm/Release/AFProto*.so login/
cp -a ../../Bin/Comm/Release/AFProto*.so proxy/
cp -a ../../Bin/Comm/Release/AFProto*.so game/

cp -a ../../Bin/Comm/Release/AFKernelPlugin*.so master/
cp -a ../../Bin/Comm/Release/AFKernelPlugin*.so world/
cp -a ../../Bin/Comm/Release/AFKernelPlugin*.so login/
cp -a ../../Bin/Comm/Release/AFKernelPlugin*.so proxy/
cp -a ../../Bin/Comm/Release/AFKernelPlugin*.so game/

cp -a ../../Bin/Comm/Release/AFUtilityPlugin*.so master/
cp -a ../../Bin/Comm/Release/AFUtilityPlugin*.so world/
cp -a ../../Bin/Comm/Release/AFUtilityPlugin*.so login/
cp -a ../../Bin/Comm/Release/AFUtilityPlugin*.so proxy/
cp -a ../../Bin/Comm/Release/AFUtilityPlugin*.so game/

echo Copy self dlls
cp -a ../../Bin/Comm/Release/AFMaster*.so	master/
cp -a ../../Bin/Comm/Release/AFWorld*.so	world/
cp -a ../../Bin/Comm/Release/AFLogin*.so	login/
cp -a ../../Bin/Comm/Release/AFProxy*.so	proxy/
cp -a ../../Bin/Comm/Release/AFGame*.so		game/

export LC_ALL="C"

cd master
echo Starting master...
./PluginLoader -d app_id=1 app_name=master_server cfg=Plugin.xml

cd ..
cd world
echo Starting world...
./PluginLoader -d app_id=50 app_name=world_server cfg=Plugin.xml

cd ..
cd login
echo Starting login...
./PluginLoader -d app_id=52 app_name=login_server cfg=Plugin.xml

cd ..
cd proxy
echo Starting proxy...
./PluginLoader -d app_id=51 app_name=proxy_server cfg=Plugin.xml

cd ..
cd game
echo Starting game...
./PluginLoader -d app_id=53 app_name=game_server cfg=Plugin.xml

echo All processes are launching...

ps ax | grep *Server