echo Copy common dlls...
cp -a ../../../Bin/Comm/Release/PluginLoader AFLoginServer/
cp -a ../../../Bin/Comm/Release/PluginLoader AFMasterServer/
cp -a ../../../Bin/Comm/Release/PluginLoader AFProxyServer/
cp -a ../../../Bin/Comm/Release/PluginLoader AFWorldServer/
cp -a ../../../Bin/Comm/Release/PluginLoader AFGameServer/

cp -a ../../../Dep/lib/Release/libproto*.so AFLoginServer/
cp -a ../../../Dep/lib/Release/libproto*.so AFMasterServer/
cp -a ../../../Dep/lib/Release/libproto*.so AFProxyServer/
cp -a ../../../Dep/lib/Release/libproto*.so AFWorldServer/
cp -a ../../../Dep/lib/Release/libproto*.so AFGameServer/

cp -a ../../../Bin/Comm/Release/AFProto*.so AFLoginServer/
cp -a ../../../Bin/Comm/Release/AFProto*.so AFMasterServer/
cp -a ../../../Bin/Comm/Release/AFProto*.so AFProxyServer/
cp -a ../../../Bin/Comm/Release/AFProto*.so AFWorldServer/
cp -a ../../../Bin/Comm/Release/AFProto*.so AFGameServer/

cp -a ../../../Bin/Comm/Release/AFKernelPlugin*.so AFLoginServer/
cp -a ../../../Bin/Comm/Release/AFKernelPlugin*.so AFMasterServer/
cp -a ../../../Bin/Comm/Release/AFKernelPlugin*.so AFProxyServer/
cp -a ../../../Bin/Comm/Release/AFKernelPlugin*.so AFWorldServer/
cp -a ../../../Bin/Comm/Release/AFKernelPlugin*.so AFGameServer/

cp -a ../../../Bin/Comm/Release/AFUtilityPlugin*.so AFLoginServer/
cp -a ../../../Bin/Comm/Release/AFUtilityPlugin*.so AFMasterServer/
cp -a ../../../Bin/Comm/Release/AFUtilityPlugin*.so AFProxyServer/
cp -a ../../../Bin/Comm/Release/AFUtilityPlugin*.so AFWorldServer/
cp -a ../../../Bin/Comm/Release/AFUtilityPlugin*.so AFGameServer/

echo Copy self dlls
cp -a ../../../Bin/Comm/Release/AFLogin*.so AFLoginServer/
cp -a ../../../Bin/Comm/Release/AFMaster*.so AFMasterServer/
cp -a ../../../Bin/Comm/Release/AFProxy*.so AFProxyServer/
cp -a ../../../Bin/Comm/Release/AFWorld*.so AFWorldServer/
cp -a ../../../Bin/Comm/Release/AFGame*.so AFGameServer/

export LC_ALL="C"

cd AFMasterServer
echo Starting AFMasterServer...
./PluginLoader -d app_id=3 app_name=MasterServer cfg=Plugin.xml

cd ..
cd AFWorldServer
echo Starting AFWorldServer...
./PluginLoader -d app_id=7 app_name=WorldServer cfg=Plugin.xml


cd ..
cd AFLoginServer
echo Starting AFLoginServer...
./PluginLoader -d app_id=4 app_name=LoginServer cfg=Plugin.xml

cd ..
cd AFGameServer
echo Starting AFGameServer...
./PluginLoader -d app_id=6 app_name=GameServer cfg=Plugin.xml

cd ..
cd AFProxyServer
echo Starting AFProxyServer...
./PluginLoader -d app_id=5 app_name=ProxyServer cfg=Plugin.xml

echo All processes are launching...

ps ax | grep *Server