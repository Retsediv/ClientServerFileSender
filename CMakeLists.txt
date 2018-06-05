cmake_minimum_required(VERSION 3.10)
project(EzloTestTask)

find_package(Threads)

#find_package(Boost REQUIRED COMPONENTS thread system filesystem)
find_package(Boost REQUIRED COMPONENTS thread system filesystem)
include_directories(${Boost_INCLUDE_DIRS})

set(CMAKE_CXX_STANDARD 14)

file(GLOB SERVER_SOURCE_FILES
        server/*
        server_main.cpp
        server_main.cpp
        helpers.h
        )

add_executable(server ${SERVER_SOURCE_FILES})
target_link_libraries(server ${Boost_LIBRARIES} ${CMAKE_THREAD_LIBS_INIT})


file(GLOB CLIENT_SOURCE_FILES
        client/*
        client_main.cpp
        helpers.h
        )

add_executable(client ${CLIENT_SOURCE_FILES})
target_link_libraries(client ${Boost_LIBRARIES} ${CMAKE_THREAD_LIBS_INIT})