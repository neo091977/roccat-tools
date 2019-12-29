# Stefan Achatz 2012
#
# Tries to find out if the target is a 64bit machine and sets paths and naming
# conventions according to target.
# following variables are set:
#
# LIBDIR
#
# Does nothing if LIBDIR is already set.

IF(NOT LIBDIR)
  FIND_PACKAGE(PkgConfig)
  PKG_CHECK_MODULES(PKG_TEST glib-2.0)
  IF(PKG_TEST_LIBDIR)
    SET(LIBDIR ${PKG_TEST_LIBDIR})
  ENDIF()
ENDIF()

IF(NOT LIBDIR)
  EXECUTE_PROCESS(COMMAND ${CMAKE_C_COMPILER} -dumpmachine
    OUTPUT_VARIABLE MACHINE
    OUTPUT_STRIP_TRAILING_WHITESPACE
  )

  MESSAGE(STATUS "Building for target ${MACHINE}")

  STRING(REGEX MATCH
    "(x86_64-*)|(X86_64-*)|(AMD64-*)|(amd64-*)"
    _machine_x86_64
    "${MACHINE}"
  )

  IF(_machine_x86_64)
    SET(LIBDIR /lib64)
  ELSE()
    SET(LIBDIR /lib)
  ENDIF()

ENDIF()

MESSAGE(STATUS "Installing libraries in ${LIBDIR}")
