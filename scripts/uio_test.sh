#!/bin/sh

echo "*******************************************************"
echo "* Start User IO Inspection"
echo "*******************************************************"
sleep 1

echo "*******************************************************"
echo "* Build"
echo "*******************************************************"
cd ../..
rm -rf build
west build -b scobc_module1 scobca1-fpga-test -- -DCONFIG_AUTO_RUN=y -DCONFIG_AUTO_RUN_TEST_NUMBER=2 -DCONFIG_DEBUG_PRINT=y scobca1-fpga-test/

echo "*******************************************************"
echo "* Flash"
echo "*******************************************************"
west flash

echo "*******************************************************"
echo "* Wait to finish Pre Delivery Inspection"
echo "*******************************************************"
sleep 3
echo "User IO Inspection is finished, please check the result"
