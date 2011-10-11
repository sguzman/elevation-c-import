project(levitation2)
cmake_minimum_required(VERSION 2.8)

include(FindLibXml2)

include_directories(${LIBXML2_INCLUDE_DIR})

add_executable(levitation levitation.c wikiparser.c dynstring.c revlist.c)
target_link_libraries(levitation ${LIBXML2_LIBRARIES})
set_target_properties(levitation PROPERTIES
    COMPILE_FLAGS "-g3 -Wall -Werror"
    LINK_FLAGS -g3)
