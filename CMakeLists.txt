project(levitation2)
cmake_minimum_required(VERSION 2.8)

include(FindLibXml2)

include_directories(${LIBXML2_INCLUDE_DIR})

add_executable(levitation levitation.c)
target_link_libraries(levitation ${LIBXML2_LIBRARIES})
