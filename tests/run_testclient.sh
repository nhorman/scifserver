#!/bin/bash
set -x

../scifserver/scifserver -k ./www.example.com.key -c ./www.example.com.cert > ./scifserver.log 2>&1 &

sleep 2

../testclient/scifclient > ./scifclient.log 2>&1


kill -SIGINT $(pidof scifserver)


