# Stefan Achatz 2014
#
# Tries to find libm and sets following variables according to found capabilities:
#
# M_FOUND
# M_INCLUDE_DIR
# M_LIBRARY

FIND_LIBRARY(M_LIBRARY m)
FIND_PATH(M_INCLUDE_DIR math.h)

IF(M_LIBRARY AND M_INCLUDE_DIR)
  SET(M_FOUND true)
ENDIF()

IF(M_FIND_REQUIRED AND NOT M_FOUND)
  MESSAGE(FATAL_ERROR "Could not find M")
ENDIF()
