# Stefan Achatz 2012
#
# Tries to find libgdk and sets following variables according to found capabilities:
#
# GDK_FOUND
# GDK_INCLUDE_DIRS
# GDK_LIBRARIES

FIND_PACKAGE(PkgConfig)
PKG_CHECK_MODULES(PKG_GDK gdk-2.0)

FIND_PATH(GDK_GDK_INCLUDE_DIRS gdk/gdk.h
  HINTS
    ${PKG_GDK_INCLUDE_DIRS}
    ${PKG_GDK_INCLUDEDIR}
  PATH_SUFFIXES gtk-2.0
)

FIND_PATH(GDK_GDKCONFIG_INCLUDE_DIRS gdkconfig.h
  HINTS
    ${PKG_GDK_INCLUDE_DIRS}
    ${PKG_GDK_INCLUDEDIR}
  PATH_SUFFIXES gtk-2.0
)

FIND_LIBRARY(GDK_GDK_LIBRARIES gdk-x11-2.0
  PATHS ${PKG_GDK_LIBRARY_DIRS}
)

FIND_PACKAGE(GDK_PIXBUF2)

IF(GDK_GDK_LIBRARIES AND GDK_GDK_INCLUDE_DIRS AND GDK_GDKCONFIG_INCLUDE_DIRS AND GDK_PIXBUF2_FOUND)
  SET(GDK_INCLUDE_DIRS ${GDK_GDK_INCLUDE_DIRS} ${GDK_GDKCONFIG_INCLUDE_DIRS} ${GDK_PIXBUF2_INCLUDE_DIRS})
  SET(GDK_LIBRARIES ${GDK_GDK_LIBRARIES} ${GDK_PIXBUF2_LIBRARIES})
  SET(GDK_FOUND true)
ENDIF()

IF(GDK_FIND_REQUIRED AND NOT GDK_FOUND)
  MESSAGE(FATAL_ERROR "Could not find GDK")
ENDIF()
