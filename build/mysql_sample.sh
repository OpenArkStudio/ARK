export LC_ALL="C"

# The default zone is 1001

CHANNEL_ID=1
ZONE_ID=1

echo Starting http_server...
./bin/app_d --busid=1.0.1.1 --name=mysql_sample --app_conf=conf/mysql_sample.app.conf

echo All processes are launching...

ps aux | grep app_d
