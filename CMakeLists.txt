cmake_minimum_required(VERSION 3.16)
project(DNet)

set(CMAKE_CXX_STANDARD 17)

add_executable(DNet main.cpp driver/driver.cpp driver/driver.h defs.h layers/Layer.h util/util.cpp util/util.h layers/L2/arp/ARP.cpp layers/L2/arp/ARP.h layers/defs.h layers/L2/L2.cpp layers/L2/L2.h DNet.cpp DNet.h layers/L3/L3.cpp layers/L3/L3.h layers/L3/icmp/ICMP.cpp layers/L3/icmp/ICMP.h layers/L4/L4.h layers/L4/L4.cpp)
target_link_libraries(DNet pthread)
#set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O2")
