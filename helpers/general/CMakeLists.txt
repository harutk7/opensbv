cmake_minimum_required(VERSION 3.5)

project(opensbv)
set(PROJECT_NAME opensbv)
set(CMAKE_CXX_STANDARD 11)

set(SOURCE_HELPERS
        src/GeneralHelper.cpp
        include/opensbv/helpers/general/GeneralHelper.h
        )

include_directories(
        include)

SET(LIBRARY_OUTPUT_PATH ${CMAKE_BINARY_DIR}/libs)

# LIBRARIES

add_library(opensbv_helpers_general SHARED ${SOURCE_HELPERS} )

target_link_libraries(opensbv_helpers_general pthread)

install(TARGETS opensbv_helpers_general DESTINATION lib)

install(DIRECTORY include/
        DESTINATION include
        FILES_MATCHING PATTERN "*.h*")