cmake_minimum_required(VERSION 3.10)
project(OrderBookEngine)
set(CMAKE_CXX_STANDARD 20)
include_directories(include)

add_executable(engine src/main.cpp src/orderbook.cpp)
add_executable(server src/server.cpp src/orderbook.cpp)
add_executable(client src/client.cpp)

