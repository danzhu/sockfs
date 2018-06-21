cmake_minimum_required(VERSION 3.5)

project(sockfs CXX)

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

set(CMAKE_BUILD_TYPE Debug)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

if("${CMAKE_CXX_COMPILER_ID}" STREQUAL "Clang")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} \
        -Weverything \
        -Wno-c++98-compat \
        -Wno-c++98-compat-pedantic \
        -Wno-covered-switch-default \
        -Wno-padded \
        -Wno-sign-conversion \
        -Wno-switch-enum \
        -pedantic")

    if(CMAKE_GENERATOR STREQUAL "Ninja")
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fcolor-diagnostics")
    endif()
elseif("${CMAKE_CXX_COMPILER_ID}" STREQUAL "GNU")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} \
        -Wall \
        -Wextra \
        -pedantic")

    if(CMAKE_GENERATOR STREQUAL "Ninja")
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fdiagnostics-color=always")
    endif()
elseif("${CMAKE_CXX_COMPILER_ID}" STREQUAL "MSVC")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} \
        /W4 \
        /wd4100")
else()
    message(WARNING "Compiler not recognized, no warning flags set")
endif()

add_executable(server src/server.cc src/socket.cc src/fs.cc)

add_executable(client src/client.cc src/socket.cc src/sock.cc)
target_compile_definitions(client PUBLIC -DFUSE_USE_VERSION=29 -D_FILE_OFFSET_BITS=64)
target_link_libraries(client fuse)
