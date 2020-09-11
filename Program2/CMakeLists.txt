cmake_minimum_required(VERSION 3.10)
project(program2)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_FLAGS ${CMAKE_CXX_FLAGS} "-pthread")

add_executable(program2 program2.cpp local_socket.h)

