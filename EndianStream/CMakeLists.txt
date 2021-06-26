# CMakeList.txt : CMake project for EndianStream
cmake_minimum_required (VERSION 3.8)
project(EndianStream)
set (CMAKE_CXX_STANDARD 20)
set (CMAKE_CXX_STANDARD_REQUIRED True)

set(SOURCES endian_reader.cpp 
            endian_writer.cpp 
            sys_io.cpp 
            include/EStream.h 
            include/EndianStream/endian_reader.h 
            include/EndianStream/endian_writer.h 
            include/EndianStream/sys_io.h)

add_library (EndianStream ${SOURCES})

add_executable (Scratch "scratch.cpp")

target_link_libraries(Scratch EndianStream ${CMAKE_CURRENT_SOURCE_DIR}/lib/zlibstaticd.lib)

target_include_directories(Scratch PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)
