cmake_minimum_required(VERSION 3.5)

project(opensbv)
set(PROJECT_NAME opensbv)
set(CMAKE_CXX_STANDARD 11)

set (CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake")

#CALL CMAKE SCRIPTS
include(CombineParts)

if (WITH_TEST)
    add_subdirectory(test)
endif(WITH_TEST)