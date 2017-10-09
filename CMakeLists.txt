cmake_minimum_required(VERSION 3.5)

project(opensbv)
set(PROJECT_NAME opensbv)
set(CMAKE_CXX_STANDARD 11)

#option(WITH_H/ELPERS "WITH_HELPERS" TRUE)
#if (WITH_HELPERS)
#    include_directories(helpers/include)
#    add_subdirectory(helpers)
#endif(WITH_HELPERS)
#add_subdirectory(modules)
add_subdirectory(test)