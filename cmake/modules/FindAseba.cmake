# - Try to find readerwriterqueue
#  Aseba_FOUND - System has Aseba
#  Aseba_INCLUDE_DIRS - The Aseba include directories

set(default_path "usr/local/Aseba")

find_path(Aseba_INCLUDE_DIR common/types.h
          HINTS ${default_path} 
          PATH_SUFFIXES bin)

if (UNIX)
    SET(CMAKE_FIND_LIBRARY_PREFIXES "lib")
    SET(CMAKE_FIND_LIBRARY_SUFFIXES ".so" ".a")
endif (UNIX)

find_library(Aseba_COMMON_LIBRARY
             NAMES libAsebacommon
             HINTS ${default_path}/Aseba/lib)
set(Aseba_LIBRARIES ${Aseba_COMMON_LIBRARY})
find_library(Aseba_COMPILER_LIBRARY
             NAMES libAsebacompiler
             HINTS ${default_path}/Aseba/lib)
set(Aseba_LIBRARIES ${Aseba_LIBRARIES} ${Aseba_COMPILER_LIBRARY})

include(FindPackageHandleStandardArgs)
# handle the QUIETLY and REQUIRED arguments and set Aseba_FOUND to TRUE
# if all listed variables are TRUE
find_package_handle_standard_args(Aseba DEFAULT_MSG Aseba_LIBRARIES Aseba_INCLUDE_DIR)

mark_as_advanced(Aseba_INCLUDE_DIR Aseba_LIBRARIES)

set(Aseba_INCLUDE_DIRS  ${Aseba_INCLUDE_DIR}
                        ${Aseba_INCLUDE_DIR}/common
                        ${Aseba_INCLUDE_DIR}/compiler)
