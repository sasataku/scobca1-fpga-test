#!/bin/sh

wait_finish_pdi() {
	while read i
	do
		echo "$i" | grep -e "Abort Pre Delivery Inspection" -e "Finish Pre Delivery Inspection"
		if [ $? = "0" ];then
			echo "Pre Delivery Inspection is finished, please check the result"
			killall tail
			exit
		 fi
	done
}

cur=$(pwd)

echo "*******************************************************"
echo "* Start Pre Delivery Inspection"
echo "*******************************************************"
sleep 1

echo "*******************************************************"
echo "* Activate CAN Interface"
echo "*******************************************************"
sudo ip addr | grep can0 | grep UP > /dev/null
if [ $? = 1 ]; then
	sudo ip link set can0 type can bitrate 1000000
	sudo ip link set can0 up
fi

echo "*******************************************************"
echo "* Send CAN Packet to kick TRCH power cycle"
echo "*******************************************************"
killall can_tool.sh > /dev/null 2>&1
killall candump > /dev/null 2>&1
cansend can0 054#00
sleep 10

echo "*******************************************************"
echo "* Bootup Can tool for background"
echo "*******************************************************"
./can_tool.sh &
sleep 1

echo "*******************************************************"
echo "* Build"
echo "*******************************************************"
cd ../..
rm -rf build
west build -b scobc_module1 scobca1-fpga-test -- -DCONFIG_AUTO_RUN=y -DCONFIG_AUTO_RUN_TEST_NUMBER=1 -DCONFIG_DEBUG_PRINT=y scobca1-fpga-test/

echo "*******************************************************"
echo "* Flash"
echo "*******************************************************"
west flash
# First flash is failed, need to retry
west flash

echo "*******************************************************"
echo "* Wait to finish Pre Delivery Inspection"
echo "*******************************************************"
tail -n 0 -f ./pdi.log | wait_finish_pdi

echo "*******************************************************"
echo "* Kill CAN tool"
echo "*******************************************************"
killall can_tool.sh
killall candump
