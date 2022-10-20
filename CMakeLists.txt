cmake_minimum_required(VERSION 3.22.1)
project(DNet)

set(CMAKE_CXX_STANDARD 14)

add_executable(DNet main.cpp driver/driver.cpp driver/driver.h defs.h layers/L2/ethernet/EthernetLayer.cpp layers/L2/ethernet/EthernetLayer.h layers/Layer.h util/util.cpp util/util.h layers/L2/arp/ARP.cpp layers/L2/arp/ARP.h layers/defs.h layers/L2/L2.cpp layers/L2/L2.h)
