cmake_minimum_required(VERSION 3.0)
project(sysyc)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Werror -std=c++11")

set(SYSYC_SOURCE_DIR ${PROJECT_SOURCE_DIR}/src/)
set(SYSYC_3RDPARTY_DIR ${PROJECT_SOURCE_DIR}/3rdparty/)

include_directories(${SYSYC_SOURCE_DIR})
include_directories(${SYSYC_3RDPARTY_DIR})

add_subdirectory(${SYSYC_3RDPARTY_DIR})
set(SYSYC_LIBRARY optparse)

file(GLOB_RECURSE SYSYC_SOURCE "${SYSYC_SOURCE_DIR}/*/*.cc")
add_library(sysyc_static STATIC ${SYSYC_SOURCE})
target_link_libraries(sysyc_static ${SYSYC_LIBRARY})

add_executable(sysyc ${SYSYC_SOURCE_DIR}/main.cc)
target_link_libraries(sysyc sysyc_static)

include(CTest)
enable_testing()
add_subdirectory(test)
