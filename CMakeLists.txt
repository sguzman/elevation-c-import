project(levitation2)
cmake_minimum_required(VERSION 2.8)

include(FindLibXml2)

include_directories(${LIBXML2_INCLUDE_DIR})

add_custom_target(versionheader ALL
    DEPENDS ${CMAKE_CURRENT_BINARY_DIR}/version.h)

add_custom_command(OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/version.h
    COMMAND ${CMAKE_COMMAND} -DSOURCE_DIR=${CMAKE_CURRENT_SOURCE_DIR}
                         -P ${CMAKE_CURRENT_SOURCE_DIR}/revinfo.cmake)


add_executable(levitation levitation.c wikiparser.c dynstring.c gitwriter.c)
target_link_libraries(levitation ${LIBXML2_LIBRARIES})
set_target_properties(levitation PROPERTIES
    COMPILE_FLAGS "-g3 -Wall -Werror"
    LINK_FLAGS -g3)

add_dependencies(levitation versionheader)
