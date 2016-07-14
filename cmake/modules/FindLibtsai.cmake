# - Try to find readerwriterqueue
#  LIBTSAI_FOUND - System has libtsai
#  LIBTSAI_INCLUDE_DIRS - The libtsai include directories

find_path(LIBTSAI_INCLUDE_DIR libtsai.h
          HINTS "$ENV{CATS2_DIR}/dependencies/libtsai/include" 
          PATH_SUFFIXES libtsai)

if (UNIX)
	SET(CMAKE_FIND_LIBRARY_PREFIXES "lib")
	SET(CMAKE_FIND_LIBRARY_SUFFIXES ".so" ".a")
endif (UNIX)

find_library(LIBTSAI_LIBRARY
             NAMES tsai
             HINTS "$ENV{CATS2_DIR}/dependencies/libtsai/")

include(FindPackageHandleStandardArgs)
# handle the QUIETLY and REQUIRED arguments and set LIBTSAI_FOUND to TRUE
# if all listed variables are TRUE
find_package_handle_standard_args(Libtsai  DEFAULT_MSG LIBTSAI_LIBRARY LIBTSAI_INCLUDE_DIR)

mark_as_advanced(LIBTSAI_INCLUDE_DIR LIBTSAI_LIBRARY)

set(LIBTSAI_INCLUDE_DIRS ${LIBTSAI_INCLUDE_DIR} )
