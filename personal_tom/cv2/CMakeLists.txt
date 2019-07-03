cmake_minimum_required(VERSION 2.8)
project(auto_city)

# set(CMAKE_SYSTEM_NAME Generic)
# set(CMAKE_SYSTEM_PROCESSOR arm)

SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11 -pthread")

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/build)
set (CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/cmake" )

# where to look for header files
include_directories(include)

# inspect the following subdirectories
add_subdirectory(src)
add_subdirectory(lib)

file(STRINGS "cmake/VERSION" LIBPIXY_VERSION)
add_definitions(-D__LIBPIXY_VERSION__="${LIBPIXY_VERSION}")
add_definitions(-D__BOOST_ROOT__="/usr/include/boost")
add_definitions(-D__BOOST_VER__="1.62.0")