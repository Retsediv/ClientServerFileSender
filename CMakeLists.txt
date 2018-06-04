cmake_minimum_required(VERSION 3.10)
project(EzloTestTask)

find_package (Threads)

find_package(Boost 1.40 REQUIRED COMPONENTS thread system filesystem program_options)
include_directories(${Boost_INCLUDE_DIRS})

set(CMAKE_CXX_STANDARD 11)

#add_executable(EzloTestTask main.cpp)
#target_link_libraries(EzloTestTask ${Boost_LIBRARIES})

add_executable(server session.cpp server.cpp exceptions.h session.h)
target_link_libraries(server ${Boost_LIBRARIES} ${CMAKE_THREAD_LIBS_INIT})

add_executable(client client.cpp)
target_link_libraries(client ${Boost_LIBRARIES} ${CMAKE_THREAD_LIBS_INIT})