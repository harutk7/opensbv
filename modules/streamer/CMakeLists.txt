cmake_minimum_required(VERSION 3.5)

project(opensbv)
set(PROJECT_NAME opensbv)
set(CMAKE_CXX_STANDARD 11)

include_directories(
        ./include
)

set(HEADER_FILES
        include/opensbv/streamer/streamerMRTP.h
        include/opensbv/streamer/streamBuffer.h
        include/opensbv/streamer/streamerBase.h
        include/opensbv/streamer/streamer.h
        )

set(SOURCE_FILES
        src/streamerMRTP.cpp
        src/streamBuffer.cpp
        src/streamerBase.cpp
        src/streamer.cpp
        )

SET(LIBRARY_OUTPUT_PATH ${CMAKE_BINARY_DIR}/libs)

# LIBRARIES

add_library(opensbv_streamer SHARED ${HEADER_FILES} ${SOURCE_FILES})

target_link_libraries(opensbv_streamer ${CMAKE_BINARY_DIR}/libs/libopensbv_helpers_general.so ${CMAKE_BINARY_DIR}/libs/libopensbv_helpers_image.so boost_system boost_thread pthread)

install(TARGETS opensbv_streamer DESTINATION lib)

install(DIRECTORY ./include/
        DESTINATION include
        FILES_MATCHING PATTERN "*.h*")