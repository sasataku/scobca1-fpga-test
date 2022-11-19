# SPDX-License-Identifier: Apache-2.0

cmake_minimum_required(VERSION 3.20.0)

find_package(Zephyr REQUIRED HINTS $ENV{ZEPHYR_BASE})
project(scobca1-fpga-test)

target_sources(app PRIVATE src/main.c)
target_sources(app PRIVATE src/common.c)
target_sources(app PRIVATE src/watchdog.c)
target_sources(app PRIVATE src/i2c_test.c)
target_sources(app PRIVATE src/hrmem_test.c)
target_sources(app PRIVATE src/qspi_common.c)
target_sources(app PRIVATE src/qspi_norflash_test.c)
target_sources(app PRIVATE src/qspi_fram_test.c)
target_sources(app PRIVATE src/can_test.c)
target_sources(app PRIVATE src/bhm_test.c)
