cmake_minimum_required(VERSION 3.5)

project(opensbv)
set(PROJECT_NAME opensbv)
set(CMAKE_CXX_STANDARD 11)

include_directories(
        ./include
)

set(SOURCE_FILES
        src/CameraUVC.cpp
        )

SET(LIBRARY_OUTPUT_PATH ${CMAKE_BINARY_DIR}/libs)

# LIBRARIES

add_library(opensbv_camera SHARED ${SOURCE_FILES})

target_link_libraries(opensbv_camera pthread)

install(TARGETS opensbv_camera DESTINATION lib)

install(DIRECTORY ./include/
        DESTINATION include
        FILES_MATCHING PATTERN "*.h*")