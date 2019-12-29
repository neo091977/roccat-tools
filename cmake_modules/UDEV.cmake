# Stefan Achatz 2012
#
# Tries to find install dir for udev rules and stores path.
# Normally that's /lib/udev/rules.d but Fedora 17 started to move /lib to /usr/lib
# Following variables are set:
#
# UDEVDIR
#
# Does nothing if UDEVDIR is already set.

IF(NOT UDEVDIR)
  FIND_PATH(UDEVDIR rules.d
    HINTS
      /usr/lib/udev
      /lib/udev
  )

  IF(NOT UDEVDIR)
    MESSAGE(FATAL_ERROR "Don't know where to install udev rules. Please set UDEVDIR manually.")
  ENDIF()

  SET(UDEVDIR "${UDEVDIR}/rules.d" CACHE PATH "" FORCE)
ENDIF()

MESSAGE(STATUS "Installing udev rules in ${UDEVDIR}")
