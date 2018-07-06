cmake_minimum_required(VERSION 3.5)

project(opensbv)
set(PROJECT_NAME opensbv)
set(CMAKE_CXX_STANDARD 11)

option(WITH_HELPERS_GENERAL "WITH_HELPERS_GENERAL" TRUE)
option(WITH_HELPERS_IMAGE "WITH_HELPERS_IMAGE" TRUE)

if (WITH_HELPERS_GENERAL)
    add_subdirectory(general)
endif(WITH_HELPERS_GENERAL)

if (WITH_HELPERS_IMAGE)
    add_subdirectory(image)
endif(WITH_HELPERS_IMAGE)