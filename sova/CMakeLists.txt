list_contains(DEVICES sova contains)
IF(contains)
  add_dbus_bindings(sova)

  INCLUDE_DIRECTORIES(
    ${CMAKE_SOURCE_DIR}/sova/libroccatsova
    ${CMAKE_BINARY_DIR}/sova
  )

  ADD_SUBDIRECTORY(libroccatsova)
  ADD_SUBDIRECTORY(roccatsovaconfig)
  ADD_SUBDIRECTORY(roccatsovacontrol)
  ADD_SUBDIRECTORY(libsovaeventhandler)
ENDIF()
