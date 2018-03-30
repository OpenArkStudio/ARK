echo Copy common dlls...
cp -a ../../../Bin/Comm/Debug/PluginLoader_d AFLoginServer/
cp -a ../../../Bin/Comm/Debug/PluginLoader_d AFMasterServer/
cp -a ../../../Bin/Comm/Debug/PluginLoader_d AFProxyServer/
cp -a ../../../Bin/Comm/Debug/PluginLoader_d AFWorldServer/
cp -a ../../../Bin/Comm/Debug/PluginLoader_d AFGameServer/

cp -a ../../../Dep/lib/Debug/libproto*d.so AFLoginServer/
cp -a ../../../Dep/lib/Debug/libproto*d.so AFMasterServer/
cp -a ../../../Dep/lib/Debug/libproto*d.so AFProxyServer/
cp -a ../../../Dep/lib/Debug/libproto*d.so AFWorldServer/
cp -a ../../../Dep/lib/Debug/libproto*d.so AFGameServer/

cp -a ../../../Bin/Comm/Debug/AFProto*d.so AFLoginServer/
cp -a ../../../Bin/Comm/Debug/AFProto*d.so AFMasterServer/
cp -a ../../../Bin/Comm/Debug/AFProto*d.so AFProxyServer/
cp -a ../../../Bin/Comm/Debug/AFProto*d.so AFWorldServer/
cp -a ../../../Bin/Comm/Debug/AFProto*d.so AFGameServer/

cp -a ../../../Bin/Comm/Debug/AFKernelPlugin*d.so AFLoginServer/
cp -a ../../../Bin/Comm/Debug/AFKernelPlugin*d.so AFMasterServer/
cp -a ../../../Bin/Comm/Debug/AFKernelPlugin*d.so AFProxyServer/
cp -a ../../../Bin/Comm/Debug/AFKernelPlugin*d.so AFWorldServer/
cp -a ../../../Bin/Comm/Debug/AFKernelPlugin*d.so AFGameServer/

cp -a ../../../Bin/Comm/Debug/AFUtilityPlugin*d.so AFLoginServer/
cp -a ../../../Bin/Comm/Debug/AFUtilityPlugin*d.so AFMasterServer/
cp -a ../../../Bin/Comm/Debug/AFUtilityPlugin*d.so AFProxyServer/
cp -a ../../../Bin/Comm/Debug/AFUtilityPlugin*d.so AFWorldServer/
cp -a ../../../Bin/Comm/Debug/AFUtilityPlugin*d.so AFGameServer/

echo Copy self dlls
cp -a ../../../Bin/Comm/Debug/AFLogin*d.so AFLoginServer/
cp -a ../../../Bin/Comm/Debug/AFMaster*d.so AFMasterServer/
cp -a ../../../Bin/Comm/Debug/AFProxy*d.so AFProxyServer/
cp -a ../../../Bin/Comm/Debug/AFWorld*d.so AFWorldServer/
cp -a ../../../Bin/Comm/Debug/AFGame*d.so AFGameServer/

export LC_ALL="C"

cd AFMasterServer
echo Starting AFMasterServer...
./PluginLoader_d -d app_id=3 app_name=MasterServer cfg=Plugin.xml

cd ..
cd AFWorldServer
echo Starting AFWorldServer...
./PluginLoader_d -d app_id=7 app_name=WorldServer cfg=Plugin.xml


cd ..
cd AFLoginServer
echo Starting AFLoginServer...
./PluginLoader_d -d app_id=4 app_name=LoginServer cfg=Plugin.xml

cd ..
cd AFGameServer
echo Starting AFGameServer...
./PluginLoader_d -d app_id=6 app_name=GameServer cfg=Plugin.xml

cd ..
cd AFProxyServer
echo Starting AFProxyServer...
./PluginLoader_d -d app_id=5 app_name=ProxyServer cfg=Plugin.xml

echo All processes are launching...

ps ax | grep *Server