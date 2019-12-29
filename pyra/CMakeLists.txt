list_contains(DEVICES pyra contains)
IF(contains)
  add_dbus_bindings(pyra)

  INCLUDE_DIRECTORIES(
    ${CMAKE_SOURCE_DIR}/pyra/libroccatpyra
    ${CMAKE_BINARY_DIR}/pyra
  )

  ADD_SUBDIRECTORY(libroccatpyra)

  ADD_SUBDIRECTORY(roccatpyraconfig)
  ADD_SUBDIRECTORY(roccatpyracontrol)
  ADD_SUBDIRECTORY(libpyraeventhandler)
ENDIF()
