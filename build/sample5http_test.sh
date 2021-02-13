export LC_ALL="C"

# The default zone is 1001

CHANNEL_ID=1
ZONE_ID=1

echo Starting http_server...
./bin/app_d -d --busid=1.0.1.1 --name=sample5 --app_conf=conf/Sample5.app.conf

echo All processes are launching...

ps aux | grep app_d
