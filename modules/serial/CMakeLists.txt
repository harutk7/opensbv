cmake_minimum_required(VERSION 3.5)

project(opensbv)
set(PROJECT_NAME opensbv)
set(CMAKE_CXX_STANDARD 11)

include_directories(
        ./include
        ~/Libraries/serial/include)

set(SOURCE_FILES
        src/Serialrw.cpp
        include/opensbv/serial/Serialrw.h)

SET(LIBRARY_OUTPUT_PATH ${CMAKE_BINARY_DIR}/libs)

# LIBRARIES

add_library(opensbv_serial SHARED ${SOURCE_FILES})

target_link_libraries(opensbv_serial serial)

install(TARGETS opensbv_serial DESTINATION lib)

install(DIRECTORY ./include/
        DESTINATION include
        FILES_MATCHING PATTERN "*.h*")