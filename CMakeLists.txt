project(elevation C)
cmake_minimum_required(VERSION 2.6)

if(not ${LIBXML2DIR})
    include(FindLibXml2)
else()
    # I had a hard time to figure out how to give the list of libraries via
    # a -DLIBXML2_LIBRARIES=... switch on the command line, so I placed the
    # needed libraries in this place. This works as long as you use the widely
    # used libraries from ftp://ftp.zlatkovic.com/libxml/, but can fail if
    # you use a libxml2 version where some parts are different compiled.
    set(LIBXML2_INCLUDE_DIR "${LIBXML2DIR}/include")
    set(LIBXML2_LIBRARIES
        "${LIBXML2DIR}/lib/zlib.lib"
        "${LIBXML2DIR}/lib/iconv.lib"
        "${LIBXML2DIR}/lib/libxml2.lib")
endif()

include_directories(${LIBXML2_INCLUDE_DIR})
include_directories(${CMAKE_CURRENT_BINARY_DIR})

macro(set_elev_props target)
set_target_properties(${target} PROPERTIES
    COMPILE_FLAGS "-g3 -Wall -Werror --std=c99"
    LINK_FLAGS -g3)
endmacro()

add_library(elev filehandling.c dynstring.c wikiparser.c gitwriter.c)
add_dependencies(elev versionheader)
set_elev_props(elev)

add_executable(elevation elevation.c)
target_link_libraries(elevation elev ${LIBXML2_LIBRARIES})
set_elev_props(elevation)

add_custom_target(versionheader ALL
    DEPENDS ${CMAKE_CURRENT_BINARY_DIR}/version.h)

# the output of non_existant_file is necessary to run this command
# with every make invocation.
add_custom_command(OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/version.h non_existant_file
    COMMAND ${CMAKE_COMMAND} -DSOURCE_DIR=${CMAKE_CURRENT_SOURCE_DIR}
                         -P ${CMAKE_CURRENT_SOURCE_DIR}/revinfo.cmake)


add_dependencies(elevation versionheader)

add_executable(elevtest elevtest.c)
set_elev_props(elevtest)
target_link_libraries(elevtest elev)
