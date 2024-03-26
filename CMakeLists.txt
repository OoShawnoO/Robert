cmake_minimum_required(VERSION 3.20)
project(Robert)
set(CMAKE_CXX_STANDARD 11)
# Module Utils
add_subdirectory(Utils)
# Module Service
add_subdirectory(Service)
# Module Terminal
add_subdirectory(Terminal)
# test
#add_subdirectory(test)