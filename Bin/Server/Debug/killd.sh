kill -9 $(pidof MasterServer)
kill -9 $(pidof WorldServer)
kill -9 $(pidof LoginServer)
kill -9 $(pidof GameServer)
kill -9 $(pidof ProxyServer)

ps ax | grep *Server