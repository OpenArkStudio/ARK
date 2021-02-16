export LC_ALL="C"

# The default zone is 1001

CHANNEL_ID=1
ZONE_ID=1

echo Starting sample1...
./bin/app_d --busid=1.0.1.1 --name=sample1 --app_conf=conf/sample1.app.conf

echo All processes are launching...

ps aux | grep app_d
