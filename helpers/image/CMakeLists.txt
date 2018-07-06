cmake_minimum_required(VERSION 3.5)

project(opensbv)
set(PROJECT_NAME opensbv)
set(CMAKE_CXX_STANDARD 11)

set(SOURCE_HELPERS
        src/ImageHelper.cpp
        include/opensbv/helpers/image/ImageHelper.h
        )

set(FFMPEG_LIBS avutil avcodec swscale)

include_directories(
        include
)

SET(LIBRARY_OUTPUT_PATH ${CMAKE_BINARY_DIR}/libs)

# LIBRARIES

add_library(opensbv_helpers_image SHARED ${SOURCE_HELPERS} )

SET(JPEG_TURBO_LIB libturbojpeg.so)

target_link_libraries(opensbv_helpers_image ${JPEG_TURBO_LIB} pthread)

install(TARGETS opensbv_helpers_image DESTINATION lib)

install(DIRECTORY include/
        DESTINATION include
        FILES_MATCHING PATTERN "*.h*")