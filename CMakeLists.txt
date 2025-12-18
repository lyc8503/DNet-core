cmake_minimum_required(VERSION 3.16)
project(DNet)

set(CMAKE_CXX_STANDARD 17)

add_executable(DNet main.cpp driver/driver.cpp util/util.cpp layers/L2/arp/ARP.cpp layers/L2/L2.cpp DNet.cpp layers/L3/L3.cpp layers/L3/icmp/ICMP.cpp layers/L4/L4.cpp layers/L4/udp/UDP.cpp)
target_link_libraries(DNet pthread)
#set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O2")
