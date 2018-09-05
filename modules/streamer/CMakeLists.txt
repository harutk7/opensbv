cmake_minimum_required(VERSION 3.5)

project(opensbv)
set(PROJECT_NAME opensbv)
set(CMAKE_CXX_STANDARD 11)

find_package(OpenCV REQUIRED)
find_package(Boost REQUIRED)

include_directories(
        ./include
)

set(HEADER_FILES
        include/opensbv/streamer/streamerMRTP.h
        include/opensbv/streamer/streamBuffer.h
        include/opensbv/streamer/streamerBase.h
        include/opensbv/streamer/streamer.h
        include/opensbv/streamer/capture.h
        include/opensbv/streamer/abstractCapture.h
        include/opensbv/streamer/captureMRTP.h
        include/opensbv/streamer/udpServer.h
        include/opensbv/streamer/tcpClient.h
        include/opensbv/streamer/chunkSplitter.h
        include/opensbv/streamer/tcpServer.h
        )

set(SOURCE_FILES
        src/streamerMRTP.cpp
        src/streamBuffer.cpp
        src/streamerBase.cpp
        src/streamer.cpp
        src/capture.cpp
        src/captureMRTP.cpp
        src/udpServer.cpp
        src/tcpClient.cpp
        src/chunkSplitter.cpp
        src/tcpServer.cpp
        )

SET(LIBRARY_OUTPUT_PATH ${CMAKE_BINARY_DIR}/libs)

# LIBRARIES

add_library(opensbv_streamer SHARED ${HEADER_FILES} ${SOURCE_FILES})

target_link_libraries(opensbv_streamer
        ${OpenCV_LIBS}
        ${CMAKE_BINARY_DIR}/libs/libopensbv_helpers_general.so
        ${CMAKE_BINARY_DIR}/libs/libopensbv_helpers_image.so
        boost_system boost_thread
        pthread)

add_dependencies(opensbv_streamer opensbv_helpers_general opensbv_helpers_image)

install(TARGETS opensbv_streamer DESTINATION lib)

install(DIRECTORY ./include/
        DESTINATION include
        FILES_MATCHING PATTERN "*.h*")