cmake_minimum_required(VERSION 3.7.2)
project(epoll-echo-server C)

set(CMAKE_C_STANDARD 99)

add_executable(epoll-echo-server server.c)
add_executable(epoll-echo-client client.c)