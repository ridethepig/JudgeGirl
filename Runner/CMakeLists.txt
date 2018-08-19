cmake_minimum_required (VERSION 2.8)
project (Runner)
add_definitions(-std=c++11)
add_executable(Runner main.cpp common.h configureparser.cpp configureparser.h sandboxer.cpp sandboxer.h)
