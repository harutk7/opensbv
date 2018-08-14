cmake_minimum_required(VERSION 3.5)

project(opensbv)
set(PROJECT_NAME opensbv)
set(CMAKE_CXX_STANDARD 11)

find_package(OpenCV)

SET(Native_LIBS ${CMAKE_BINARY_DIR}/libs/libopensbv_camera.so ${CMAKE_BINARY_DIR}/libs/libopensbv_helpers_image.so ${CMAKE_BINARY_DIR}/libs/libopensbv_helpers_general.so ${CMAKE_BINARY_DIR}/libs/libopensbv_streamer.so)

set(VIDEO_TEST_FILES
        video/main.cpp
        )
set(VIDEO_CLIENT_TEST_FILES
        videoClient/main.cpp
        )

set(STREAM_TEST_FILES
        stream/main.cpp
        )

set(AUDIO_TEST_FILES
        audio/main.cpp audio/AudioTest.cpp audio/AudioTest.h)

set(GENERAL_TEST_FILES
        general/main.cpp)

set(SERIAL_TEST_FILES
        serial/main.cpp)

add_executable(opensbv_test ${VIDEO_TEST_FILES})


add_dependencies(opensbv_streamer opensbv_camera opensbv_helpers_general opensbv_helpers_image opensbv_streamer )

SET(JPEG_TURBO_LIB libturbojpeg.so)

target_link_libraries(opensbv_test ${OpenCV_LIBS} ${Native_LIBS} ${JPEG_TURBO_LIB} boost_system boost_thread pthread)