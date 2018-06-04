cmake_minimum_required(VERSION 3.10)
project(EzloTestTask)

find_package (Threads)

find_package(Boost REQUIRED COMPONENTS thread system filesystem program_options)
include_directories(${Boost_INCLUDE_DIRS})

set(CMAKE_CXX_STANDARD 11)

add_executable(server server.cpp session.h session.cpp client.h)
target_link_libraries(server ${Boost_LIBRARIES} ${CMAKE_THREAD_LIBS_INIT})

add_executable(client client.cpp)
target_link_libraries(client ${Boost_LIBRARIES} ${CMAKE_THREAD_LIBS_INIT})