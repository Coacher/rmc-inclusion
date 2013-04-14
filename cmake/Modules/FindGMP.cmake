# Try to find GMP library
# If found the following variables are defined:
#
#   GMP_FOUND - system has GMP
#   GMP_INCLUDE_DIRS - GMP include directory
#   GMP_LIBRARIES - link these to use GMP
#   GMP_DEFINITIONS - compiler switches required for GMP
#
# Based on CMake module (part of libssh) by Andreas Schneider <asn@cynapses.org>
#
# Redistribution and use is allowed according to the terms of
# the New BSD license.
# For details see the accompanying COPYING-CMAKE-SCRIPTS file.
#

if (GMP_LIBRARIES AND GMP_INCLUDE_DIRS)
  set(GMP_FOUND TRUE) # in cache already
else (GMP_LIBRARIES AND GMP_INCLUDE_DIRS)

  find_path(GMP_INCLUDE_DIR
    NAMES
      gmp.h
    PATHS
      /usr/include
      /usr/local/include
  )

  find_library(GMP_LIBRARY
    NAMES
      gmp
    PATHS
      /usr/lib
      /usr/local/lib
  )

  set(GMP_INCLUDE_DIRS
    ${GMP_INCLUDE_DIR}
  )

  if (GMP_LIBRARY)
    set(GMP_LIBRARIES
        ${GMP_LIBRARIES}
        ${GMP_LIBRARY}
    )
  endif (GMP_LIBRARY)

  include(FindPackageHandleStandardArgs)
  find_package_handle_standard_args(GMP DEFAULT_MSG GMP_LIBRARIES GMP_INCLUDE_DIRS)

  # show the GMP_INCLUDE_DIRS and GMP_LIBRARIES variables only in the advanced view
  mark_as_advanced(GMP_INCLUDE_DIRS GMP_LIBRARIES)

endif (GMP_LIBRARIES AND GMP_INCLUDE_DIRS)
