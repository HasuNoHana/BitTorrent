cmake_minimum_required(VERSION 3.16)
project(TIN C)

set(CMAKE_C_STANDARD 99)

add_executable(TIN main.c include/socket.h source/socket.c include/user.h source/user.c )

target_link_libraries(TIN -lpthread)

find_package(OpenSSL REQUIRED)
target_link_libraries(TIN OpenSSL::SSL)
