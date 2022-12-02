#!/bin/sh

echo "*******************************************************"
echo "* Activate CAN Interface"
echo "*******************************************************"
sudo ip addr | grep can0 | grep UP > /dev/null
if [ $? = 1 ]; then
	sudo ip link set can0 type can bitrate 1000000
	sudo ip link set can0 up
fi

sleep 3

echo "*******************************************************"
echo "* Send CAN Packet to kick TRCH power cycle"
echo "*******************************************************"
killall can_tool.sh > /dev/null 2>&1
killall candump > /dev/null 2>&1
cansend can0 054#00
