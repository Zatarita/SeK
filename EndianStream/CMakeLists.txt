# CMakeList.txt : CMake project for EndianStream
cmake_minimum_required (VERSION 3.8)
set (CMAKE_CXX_STANDARD 17)
set (CMAKE_CXX_STANDARD_REQUIRED True)

set(SOURCES endian_reader.cpp 
            endian_writer.cpp 
            sys_io.cpp 
            include/EStream.h 
            include/EndianStream/endian_reader.h 
            include/EndianStream/endian_writer.h 
            include/EndianStream/sys_io.h)

add_library (EndianStream ${SOURCES})
