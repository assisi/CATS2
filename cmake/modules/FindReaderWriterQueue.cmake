# - Try to find readerwriterqueue
#  READERWRITERQUEUE_FOUND - System has readerwriterqueue
#  READERWRITERQUEUE_INCLUDE_DIRS - The readerwriterqueue include directories

find_path(READERWRITERQUEUE_INCLUDE_DIR readerwriterqueue.h
          HINTS "$ENV{CATS2_DIR}/dependencies"
          PATH_SUFFIXES readerwriterqueue)

include(FindPackageHandleStandardArgs)
# handle the QUIETLY and REQUIRED arguments and set READERWRITERQUEUE_FOUND to TRUE
# if all listed variables are TRUE
find_package_handle_standard_args(ReaderWriterQueue  DEFAULT_MSG READERWRITERQUEUE_INCLUDE_DIR)

mark_as_advanced(READERWRITERQUEUE_INCLUDE_DIR)

set(READERWRITERQUEUE_INCLUDE_DIRS ${READERWRITERQUEUE_INCLUDE_DIR} )
