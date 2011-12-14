project(levitation2 C)
cmake_minimum_required(VERSION 2.6)

include(FindLibXml2)

include_directories(${LIBXML2_INCLUDE_DIR})
include_directories(${CMAKE_CURRENT_BINARY_DIR})

macro(set_lev_props target)
set_target_properties(${target} PROPERTIES
    COMPILE_FLAGS "-g3 -Wall -Werror --std=c99"
    LINK_FLAGS -g3)
endmacro()

add_library(lev filehandling.c dynstring.c wikiparser.c gitwriter.c)
add_dependencies(lev versionheader)
set_lev_props(lev)

add_executable(levitation levitation.c)
target_link_libraries(levitation lev ${LIBXML2_LIBRARIES})
set_lev_props(levitation)

add_custom_target(versionheader ALL
    DEPENDS ${CMAKE_CURRENT_BINARY_DIR}/version.h)

# the output of non_existant_file is necessary to run this command
# with every make invocation.
add_custom_command(OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/version.h non_existant_file
    COMMAND ${CMAKE_COMMAND} -DSOURCE_DIR=${CMAKE_CURRENT_SOURCE_DIR}
                         -P ${CMAKE_CURRENT_SOURCE_DIR}/revinfo.cmake)


add_dependencies(levitation versionheader)

add_executable(levtest levtest.c)
set_lev_props(levtest)
target_link_libraries(levtest lev)
