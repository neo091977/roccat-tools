list_contains(DEVICES lua contains)
IF(contains)
  add_dbus_bindings(lua)

  INCLUDE_DIRECTORIES(
    ${CMAKE_SOURCE_DIR}/lua/libroccatlua
    ${CMAKE_BINARY_DIR}/lua
  )

  ADD_SUBDIRECTORY(libroccatlua)

  ADD_SUBDIRECTORY(roccatluaconfig)
  ADD_SUBDIRECTORY(roccatluacontrol)
  ADD_SUBDIRECTORY(libluaeventhandler)
ENDIF()
