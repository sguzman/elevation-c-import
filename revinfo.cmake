find_program(GIT git)

execute_process(COMMAND ${GIT} describe --long --dirty
  OUTPUT_VARIABLE GIT_VERSION
  WORKING_DIRECTORY ${SOURCE_DIR})

string(REPLACE "\n" "" GIT_VERSION ${GIT_VERSION})

file(WRITE version.h.git "#define VERSION \"${GIT_VERSION}\"\n")
execute_process(COMMAND ${CMAKE_COMMAND} -E copy_if_different
  version.h.git version.h)

