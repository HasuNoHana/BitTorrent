cmake_minimum_required(VERSION 3.19)
project(TIN C)

set(CMAKE_C_STANDARD 99)

add_executable(TIN main.c include/socket.h source/socket.c)