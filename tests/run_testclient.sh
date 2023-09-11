#!/bin/bash
EXIT_CODE=0

function cleanup() {
	rm -f ./scifserver.log ./scifclient.log
}

trap cleanup EXIT

../scifserver/scifserver -k ./www.example.com.key -c ./www.example.com.cert > ./scifserver.log 2>&1 &

sleep 2

../scifclient/scifclient > ./scifclient.log 2>&1
if [ $? -ne 0 ]
then
	echo "test client failed"
	EXIT_CODE=127
fi

sleep 2

kill -SIGKILL $(pidof scifserver)


echo "SCIF SERVER LOGS"
echo ========================================
cat ./scifserver.log


echo "SCIF CLIENT LOGS"
echo ========================================
cat ./scifclient.log


exit $EXIT_CODE


