cmake_minimum_required(VERSION 2.8)

set(PROJECT final)

project (${PROJECT})

set(SOURCES master.cpp worker.cpp fd_passing.cpp http.cpp)

set(HEADERS lib.h http.h )

add_definitions(-std=c++11)
add_executable(${PROJECT} ${HEADERS} ${SOURCES})
target_link_libraries(${PROJECT} ev)
