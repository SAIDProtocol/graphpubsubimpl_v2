cmake_minimum_required(VERSION 3.5)
project(graphpubsub_pcap)

set(CMAKE_CXX_STANDARD 11)

link_libraries(pcap)

add_executable(graphpubsub_pcap main.cpp pcap_help.cpp pcap_help.hpp)