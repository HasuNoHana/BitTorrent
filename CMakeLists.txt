cmake_minimum_required(VERSION 3.19)
project(TIN C)

set(CMAKE_C_STANDARD 99)

add_executable(TIN main.c include/socket.h source/socekt.c include/metadata.h source/metadata.c)

find_package(OpenSSL REQUIRED)
target_link_libraries(TIN OpenSSL::SSL)

target_link_libraries(TIN -lpthread)