list_contains(DEVICES "iskufx" contains)
IF(contains)
  add_dbus_bindings(iskufx)

  INCLUDE_DIRECTORIES(
    ${CMAKE_SOURCE_DIR}/isku/libroccatisku
    ${CMAKE_SOURCE_DIR}/iskufx/libroccatiskufx
    ${CMAKE_BINARY_DIR}/iskufx
  )

  ADD_SUBDIRECTORY(libroccatiskufx)
  ADD_SUBDIRECTORY(roccatiskufxconfig)
  ADD_SUBDIRECTORY(roccatiskufxcontrol)
  ADD_SUBDIRECTORY(libiskufxeventhandler)
  ADD_SUBDIRECTORY(libiskufxgfxplugin)
ENDIF()
