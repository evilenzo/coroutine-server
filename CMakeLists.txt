cmake_minimum_required(VERSION 3.21)

###
### PROJECT
### name and version
###
project(coroutine-server VERSION 1.0 LANGUAGES CXX)

###
### CONFIGURATION
###
set(CMAKE_CXX_STANDARD              20)
set(CMAKE_CXX_STANDARD_REQUIRED     ON)
set(CMAKE_CXX_FLAGS_DEBUG           "${CMAKE_CXX_FLAGS_DEBUG} -fno-omit-frame-pointer -fsanitize=address")
set(CMAKE_LINKER_FLAGS_DEBUG        "${CMAKE_LINKER_FLAGS_DEBUG} -fno-omit-frame-pointer -fsanitize=address")

set(TARGET_NAME            ${PROJECT_NAME})
set(INCLUDE_DIR            "include/${PROJECT_NAME}")

###
### LIBRARIES
###
find_package(Threads REQUIRED)
find_package(Boost 1.78 COMPONENTS system REQUIRED)
add_subdirectory(fmt)

###
### BUILD
###
add_executable(${TARGET_NAME}
        main.cpp
        server.cpp
        server.hpp)

target_link_libraries(${TARGET_NAME} PRIVATE
        Threads::Threads
        Boost::system
        fmt::fmt)
