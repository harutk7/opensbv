cmake_minimum_required(VERSION 3.5)

project(opensbv)
set(PROJECT_NAME opensbv)
set(CMAKE_CXX_STANDARD 11)

include_directories(
        ./include)

set(SOURCE_FILES
        src/Streamer.cpp
        src/StreamerUDP.cpp
        src/StreamerRTSP.cpp
        src/StreamerMJPG.cpp
        src/StreamerBase.cpp
        src/StreamerMRTSP.cpp
        src/PracticalSocket.cpp
        include/opensbv/streamer/Streamer.h
        include/opensbv/streamer/StreamerUDP.h
        include/opensbv/streamer/StreamerRTSP.h
        include/opensbv/streamer/StreamerMJPG.h
        include/opensbv/streamer/StreamerBase.h
        include/opensbv/streamer/StreamerMRTSP.h
        include/opensbv/streamer/PracticalSocket.h
        )

SET(LIBRARY_OUTPUT_PATH ${CMAKE_BINARY_DIR}/libs)

# LIBRARIES

add_library(opensbv_streamer SHARED ${SOURCE_FILES})

target_link_libraries(opensbv_streamer pthread)

install(TARGETS opensbv_streamer DESTINATION lib)

install(DIRECTORY ./include/
        DESTINATION include
        FILES_MATCHING PATTERN "*.h*")