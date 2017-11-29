cmake_minimum_required(VERSION 3.5)

project(opensbv)
set(PROJECT_NAME opensbv)
set(CMAKE_CXX_STANDARD 11)

SET(OpenCV_LIBS opencv_core opencv_videoio opencv_imgproc opencv_highgui opencv_imgcodecs opencv_tracking)

SET(OpenAL_LIBS openal)

SET(Other_LIBS jpeg serial pthread)

include_directories(
        ~/Libraries/serial/include
        ../helpers/include
        ../modules/camera/include
        ../modules/streamer/include
        ../modules/serial/include
        ../modules/audio/include
)

set(SOURCE_FILES
        ../helpers/src/images/ImageHelper.cpp
        ../helpers/src/general/GeneralHelper.cpp
        ../modules/camera/src/CameraUVC.cpp
        ../modules/audio/src/AudioCAP.cpp
        ../modules/serial/src/Serialrw.cpp
        ../modules/streamer/src/PracticalSocket.cpp
        ../modules/streamer/src/StreamerMJPG.cpp
        ../modules/streamer/src/StreamerRTSP.cpp
        ../modules/streamer/src/StreamerUDP.cpp
        ../modules/streamer/src/StreamerBase.cpp
        ../modules/streamer/src/StreamerMRTSP.cpp
        ../modules/streamer/src/Streamer.cpp
        )

set(VIDEO_TEST_FILES
        video/Commands.h
        video/Commands.cpp
        video/Config.cpp
        video/Config.h
        video/main.cpp
        video/Opencv.h
        video/Tracker.h
        video/Tracker.cpp)

set(AUDIO_TEST_FILES
        audio/main.cpp audio/AudioTest.cpp audio/AudioTest.h)

set(GENERAL_TEST_FILES
        general/main.cpp)

add_executable(opensbv_test ${SOURCE_FILES} ${VIDEO_TEST_FILES})

target_link_libraries(opensbv_test ${OpenCV_LIBS} ${Other_LIBS} ${OpenAL_LIBS})