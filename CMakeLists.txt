cmake_minimum_required(VERSION 3.22)
project(loglib)

set(CMAKE_CXX_STANDARD_REQUIRED true)
set(CMAKE_CXX_STANDARD 20)

add_executable(testrunner tests/main.cpp
                        tests/Construction.cpp
                        tests/Tags.cpp)
target_include_directories(testrunner PUBLIC ${loglib_SOURCE_DIR})