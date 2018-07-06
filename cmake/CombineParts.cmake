# MACRO FUNCTION TO FIND SUBDIRS
MACRO(SUBDIRLIST result curdir)
    FILE(GLOB children LIST_DIRECTORIES true ${curdir}/*)
    SET(dirlist "")
    FOREACH(child ${children})
        IF(IS_DIRECTORY ${child})
            LIST(APPEND dirlist ${child})
        ENDIF()
    ENDFOREACH()
    SET(${result} ${dirlist})
ENDMACRO()

# MODULES
SUBDIRLIST(helpers_subdirs helpers)
SUBDIRLIST(modules_subdirs modules)

FOREACH(subdir ${helpers_subdirs})
    include_directories(${subdir}/include)
ENDFOREACH()

FOREACH(subdir ${modules_subdirs})
    include_directories(${subdir}/include)
ENDFOREACH()

add_subdirectory(helpers)
add_subdirectory(modules)

if(CMAKE_BUILD_TYPE STREQUAL "Release")
endif()