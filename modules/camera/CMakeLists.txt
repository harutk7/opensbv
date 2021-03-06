cmake_minimum_required(VERSION 3.5)

project(opensbv)
set(PROJECT_NAME opensbv)
set(CMAKE_CXX_STANDARD 11)

find_package(OpenCV REQUIRED)
find_package(Boost REQUIRED)

include_directories(
        ./include
)

set(SOURCE_FILES
        src/CameraUVC.cpp
        src/SbvVideoCapture.cpp
        )

SET(LIBRARY_OUTPUT_PATH ${CMAKE_BINARY_DIR}/libs)

# LIBRARIES

add_library(opensbv_camera SHARED ${SOURCE_FILES})

target_link_libraries(opensbv_camera ${OpenCV_LIBS}
        ${CMAKE_BINARY_DIR}/libs/libopensbv_helpers_general.so
        ${CMAKE_BINARY_DIR}/libs/libopensbv_helpers_image.so
        boost_system boost_thread
        pthread)

add_dependencies(opensbv_camera opensbv_helpers_general opensbv_helpers_image)

install(TARGETS opensbv_camera DESTINATION lib)

install(DIRECTORY ./include/
        DESTINATION include
        FILES_MATCHING PATTERN "*.h*")