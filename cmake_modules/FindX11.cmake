# Stefan Achatz 2013
#
# Tries to find libX11 and sets following variables according to found capabilities:
#
# X11_FOUND
# X11_INCLUDE_DIRS
# X11_LIBRARIES

FIND_PACKAGE(PkgConfig)
PKG_CHECK_MODULES(PKG_X11 x11)

FIND_PATH(X11_INCLUDE_DIRS X11/Xlib.h
  HINTS
    ${PKG_X11_INCLUDE_DIRS}
    ${PKG_X11_INCLUDEDIR}
)

FIND_LIBRARY(X11_LIBRARIES X11
  HINTS ${PKG_X11_LIBRARY_DIRS}
)

IF(X11_LIBRARIES AND X11_INCLUDE_DIRS)
  SET(X11_FOUND true)
ENDIF()

IF(X11_FIND_REQUIRED AND NOT X11_FOUND)
  MESSAGE(FATAL_ERROR "Could not find X11")
ENDIF()
