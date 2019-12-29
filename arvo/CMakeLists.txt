list_contains(DEVICES arvo contains)
IF(contains)
  add_dbus_bindings(arvo)

  INCLUDE_DIRECTORIES(
    ${CMAKE_SOURCE_DIR}/arvo/libroccatarvo
    ${CMAKE_BINARY_DIR}/arvo
  )

  ADD_SUBDIRECTORY(libroccatarvo)

  ADD_SUBDIRECTORY(roccatarvoconfig)
  ADD_SUBDIRECTORY(roccatarvocontrol)
  ADD_SUBDIRECTORY(libarvoeventhandler)
ENDIF()
