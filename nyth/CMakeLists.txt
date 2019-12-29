list_contains(DEVICES nyth contains)
IF(contains)
  add_dbus_bindings(nyth)

  INCLUDE_DIRECTORIES(
    ${CMAKE_SOURCE_DIR}/nyth/libroccatnyth
    ${CMAKE_BINARY_DIR}/nyth
  )
  
  ADD_SUBDIRECTORY(libroccatnyth)
  ADD_SUBDIRECTORY(roccatnythcontrol)
  ADD_SUBDIRECTORY(roccatnythconfig)
  ADD_SUBDIRECTORY(libnytheventhandler)
  ADD_SUBDIRECTORY(libnythgfxplugin)
ENDIF()
