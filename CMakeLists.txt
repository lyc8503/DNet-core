cmake_minimum_required(VERSION 3.22.1)
project(DNet)

set(CMAKE_CXX_STANDARD 14)

add_executable(DNet main.cpp driver/driver.cpp driver/driver.h defs.h ethernet/EthernetLayer.cpp ethernet/EthernetLayer.h Layer.h)
