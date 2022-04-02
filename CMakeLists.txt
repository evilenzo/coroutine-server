cmake_minimum_required(VERSION 3.12)

###
### PROJECT
### name and version
###
project(coroutine-server LANGUAGES CXX)

###
### CONFIGURATION
###
set(CMAKE_CXX_STANDARD             20)
set(CMAKE_CXX_STANDARD_REQUIRED    ON)

set(M_TARGET_NAME                  ${PROJECT_NAME})

###
### LIBRARIES
###
find_package(Threads REQUIRED)
find_package(Boost 1.78 COMPONENTS system REQUIRED)
add_subdirectory(fmt)

###
### BUILD
###
add_executable(${M_TARGET_NAME}
        main.cpp)

target_link_libraries(${M_TARGET_NAME} PRIVATE
        Threads::Threads
        Boost::system
        fmt::fmt)
