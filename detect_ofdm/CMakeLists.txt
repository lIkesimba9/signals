project(detect_ofdm LANGUAGES CXX)
cmake_minimum_required(VERSION 3.16)


set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED YES)

set(SOURCE main.cpp)

add_executable(detect_ofdm ${SOURCE})