cmake_minimum_required(VERSION 3.5)

project(opensbv)
set(PROJECT_NAME opensbv)
set(CMAKE_CXX_STANDARD 11)

set(SOURCE_HELPERS
        src/general/GeneralHelper.cpp
        src/images/ImageHelper.cpp
        include/opensbv/helpers/general/GeneralHelper.h
        include/opensbv/helpers/images/ImageHelper.h)

set(FFMPEG_LIBS avutil avcodec swscale)

include_directories(
        ./include)

SET(LIBRARY_OUTPUT_PATH ${CMAKE_BINARY_DIR}/libs)

# LIBRARIES

add_library(opensbv_helpers SHARED ${SOURCE_HELPERS} )

target_link_libraries(opensbv_helpers jpeg pthread)

install(TARGETS opensbv_helpers DESTINATION lib)

install(DIRECTORY ./include/
        DESTINATION include
        FILES_MATCHING PATTERN "*.h*")