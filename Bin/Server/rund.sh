echo Copy common dlls...
cp -a ../../Bin/Comm/Debug/PluginLoader_d master/
cp -a ../../Bin/Comm/Debug/PluginLoader_d world/
cp -a ../../Bin/Comm/Debug/PluginLoader_d login/
cp -a ../../Bin/Comm/Debug/PluginLoader_d proxy/
cp -a ../../Bin/Comm/Debug/PluginLoader_d game/

cp -a ../../Dep/lib/Debug/libproto*d.so master/
cp -a ../../Dep/lib/Debug/libproto*d.so world/
cp -a ../../Dep/lib/Debug/libproto*d.so login/
cp -a ../../Dep/lib/Debug/libproto*d.so proxy/
cp -a ../../Dep/lib/Debug/libproto*d.so game/

cp -a ../../Bin/Comm/Debug/AFProto*d.so master/
cp -a ../../Bin/Comm/Debug/AFProto*d.so world/
cp -a ../../Bin/Comm/Debug/AFProto*d.so login/
cp -a ../../Bin/Comm/Debug/AFProto*d.so proxy/
cp -a ../../Bin/Comm/Debug/AFProto*d.so game/

cp -a ../../Bin/Comm/Debug/AFKernelPlugin*d.so master/
cp -a ../../Bin/Comm/Debug/AFKernelPlugin*d.so world/
cp -a ../../Bin/Comm/Debug/AFKernelPlugin*d.so login/
cp -a ../../Bin/Comm/Debug/AFKernelPlugin*d.so proxy/
cp -a ../../Bin/Comm/Debug/AFKernelPlugin*d.so game/

cp -a ../../Bin/Comm/Debug/AFUtilityPlugin*d.so master/
cp -a ../../Bin/Comm/Debug/AFUtilityPlugin*d.so world/
cp -a ../../Bin/Comm/Debug/AFUtilityPlugin*d.so login/
cp -a ../../Bin/Comm/Debug/AFUtilityPlugin*d.so proxy/
cp -a ../../Bin/Comm/Debug/AFUtilityPlugin*d.so game/

echo Copy self dlls
cp -a ../../Bin/Comm/Debug/AFLogin*d.so		master/
cp -a ../../Bin/Comm/Debug/AFMaster*d.so 	world/
cp -a ../../Bin/Comm/Debug/AFProxy*d.so		login/
cp -a ../../Bin/Comm/Debug/AFWorld*d.so		proxy/
cp -a ../../Bin/Comm/Debug/AFGame*d.so		game/

export LC_ALL="C"

cd master
echo Starting master...
./PluginLoader_d -d app_id=1 app_name=master_server cfg=Plugin.xml

cd ..
cd world
echo Starting world...
./PluginLoader_d -d app_id=50 app_name=world_server cfg=Plugin.xml


cd ..
cd login
echo Starting login...
./PluginLoader_d -d app_id=52 app_name=login_server cfg=Plugin.xml

cd ..
cd proxy
echo Starting proxy...
./PluginLoader_d -d app_id=53 app_name=proxy_server cfg=Plugin.xml

cd ..
cd game
echo Starting game...
./PluginLoader_d -d app_id=51 app_name=game_server cfg=Plugin.xml

echo All processes are launching...

ps ax | grep *_server