cmake_minimum_required(VERSION 3.5)

project(opensbv)
set(PROJECT_NAME opensbv)
set(CMAKE_CXX_STANDARD 11)

option(WITH_STREAMER "WITH_STREAMER" TRUE)
option(WITH_SERIAL "WITH_SERIAL" TRUE)
option(WITH_CAMERA "WITH_CAMERA" TRUE)
option(WITH_AUDIO "WITH_AUDIO" TRUE)

if (WITH_STREAMER)
    add_subdirectory(streamer)
endif(WITH_STREAMER)

if (WITH_SERIAL)
    add_subdirectory(serial)
endif(WITH_SERIAL)

if (WITH_CAMERA)
    add_subdirectory(camera)
endif(WITH_CAMERA)

if (WITH_AUDIO)
    add_subdirectory(audio)
endif(WITH_AUDIO)