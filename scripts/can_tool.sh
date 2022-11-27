#!/bin/sh

sudo ip addr | grep can0 | grep UP > /dev/null
if [ $? = 1 ]; then
	echo "CAN baudrate set to 1Mbps and link up can0"
	sudo ip link set can0 type can bitrate 1000000
	sudo ip link set can0 up
fi

while true
do
	# CAN ID
    #   FPGA        : 'F' (0x46)
    #   TRCH        : 'T' (0x54)
	#   CAN Analyzer: 'A' (0x41)
    echo "Wait CAN packet from CANID: 'F'"
	candump can0,041:07FF -n 1
	if [ $? = 0 ]; then
		echo "CAN Packet received"
		echo "Send CAN Packet (Data: 0x12,0x34,0x56,0x78,0x9A,0xBC,0xDE,0xF0)"
		cansend can0 046#123456789ABCDEF0
	else
		echo "candaump failed. aboerted"
		exit
	fi
done
