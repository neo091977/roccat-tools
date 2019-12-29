list_contains(DEVICES tyon contains)
IF(contains)
  add_dbus_bindings(tyon)

  INCLUDE_DIRECTORIES(
    ${CMAKE_SOURCE_DIR}/tyon/libroccattyon
    ${CMAKE_BINARY_DIR}/tyon
  )

  ADD_SUBDIRECTORY(libroccattyon)

  ADD_SUBDIRECTORY(roccattyonconfig)
  ADD_SUBDIRECTORY(roccattyoncontrol)
  ADD_SUBDIRECTORY(libtyoneventhandler)
  ADD_SUBDIRECTORY(libtyongfxplugin)
ENDIF()
