cmake_minimum_required (VERSION 3.8)
project(SeK)
set (CMAKE_CXX_STANDARD 20)
set (CMAKE_CXX_STANDARD_REQUIRED True)

set (ENDIAN_INCLUDE_DIR "EndianStream/include")
set (LIB_MCC_COMPRESS_INCLUDE_DIR "libMccCompress/include")
set (LIB_SABER_INCLUDE_DIR "libSaber/include")

set (ENDIAN_INCLUDES ${ENDIAN_INCLUDE_DIR}/EStream.h 
             ${ENDIAN_INCLUDE_DIR}/EndianStream/endian_reader.h 
             ${ENDIAN_INCLUDE_DIR}/EndianStream/endian_writer.h 
             ${ENDIAN_INCLUDE_DIR}/EndianStream/sys_io.h
             ${ENDIAN_INCLUDE_DIR}/EndianStream/byte_reader.h
             )

set(ENDIAN_SOURCES EndianStream/endian_reader.cpp 
            EndianStream/endian_writer.cpp 
            EndianStream/byte_reader.cpp
            EndianStream/byte_writer.cpp
            EndianStream/sys_io.cpp 
            )

set (LIB_SABER_INCLUDES ${LIB_SABER_INCLUDE_DIR}/libSaber.h 
             ${LIB_SABER_INCLUDE_DIR}/libSaber/definitions/texture_entry.h 
             ${LIB_SABER_INCLUDE_DIR}/libSaber/common.h 
             ${LIB_SABER_INCLUDE_DIR}/libSaber/saber_file.h 
             ${LIB_SABER_INCLUDE_DIR}/libSaber/saber_generic_entry.h 
             ${LIB_SABER_INCLUDE_DIR}/libSaber/s3dpak.h
             ${LIB_SABER_INCLUDE_DIR}/libSaber/s3dpak_entry.h
             ${LIB_SABER_INCLUDE_DIR}/libSaber/imeta.h
             ${LIB_SABER_INCLUDE_DIR}/libSaber/imeta_entry.h
             ${LIB_SABER_INCLUDE_DIR}/libSaber/ipak.h
             ${LIB_SABER_INCLUDE_DIR}/libSaber/ipak_entry.h
             )

set(LIB_SABER_SOURCES 
            libSaber/saber_file.cpp
            libSaber/imeta.cpp
            libSaber/imeta_entry.cpp
            )


if(CMAKE_BUILD_TYPE STREQUAL "Debug")
	set(ZLIB_LIBRARY "zlibstaticd")

    set(NAME_ENDIAN_STREAM "libEndianStreamd")
    set(NAME_LIB_MCC_COMPRESS "libMccCompressd")
    set(NAME_LIB_SABER "libSaberd")
else()
	set(ZLIB_LIBRARY "zlibstatic")

    set(NAME_ENDIAN_STREAM "libEndianStream")
    set(NAME_LIB_MCC_COMPRESS "libMccCompress")
    set(NAME_LIB_SABER "libSaber")
endif()

include_directories(${ENDIAN_INCLUDE_DIR} ${LIB_MCC_COMPRESS_INCLUDE_DIR} ${LIB_SABER_INCLUDE_DIR})
link_directories("${PROJECT_SOURCE_DIR}/lib")

# Endian Stream
add_library(${NAME_ENDIAN_STREAM} ${ENDIAN_SOURCES})

# Lib Mcc Compress
add_library(${NAME_LIB_MCC_COMPRESS} INTERFACE)
target_include_directories(${NAME_LIB_MCC_COMPRESS} INTERFACE ${LIB_MCC_COMPRESS_INCLUDE_DIR})
add_dependencies(${NAME_LIB_MCC_COMPRESS} ${NAME_ENDIAN_STREAM})

# Lib Saber
add_library(${NAME_LIB_SABER} ${LIB_SABER_SOURCES})
target_link_libraries(${NAME_LIB_SABER} ${NAME_LIB_MCC_COMPRESS} ${NAME_ENDIAN_STREAM})
target_link_libraries(${NAME_LIB_SABER} "${ZLIB_LIBRARY}")
add_dependencies(${NAME_LIB_SABER} ${NAME_LIB_MCC_COMPRESS} ${NAME_ENDIAN_STREAM})


add_executable(Scratch libSaber.cpp)
add_dependencies(Scratch ${NAME_LIB_SABER} ${NAME_LIB_MCC_COMPRESS} ${NAME_ENDIAN_STREAM})
target_link_libraries(Scratch ${NAME_LIB_SABER})