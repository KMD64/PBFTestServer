cmake_minimum_required(VERSION 3.5)

project(PBF_Test_Server LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(Threads REQUIRED)
add_executable(PBF_Test_Server main.cpp tcpserver.h tcpserver.cpp)

target_link_libraries(${CMAKE_PROJECT_NAME} Threads::Threads)
