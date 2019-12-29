list_contains(DEVICES kiro contains)
IF(contains)
  add_dbus_bindings(kiro)

  INCLUDE_DIRECTORIES(
    ${CMAKE_SOURCE_DIR}/kiro/libroccatkiro
    ${CMAKE_BINARY_DIR}/kiro
  )
  
  ADD_SUBDIRECTORY(libroccatkiro)
  ADD_SUBDIRECTORY(roccatkirocontrol)
  ADD_SUBDIRECTORY(roccatkiroconfig)
  ADD_SUBDIRECTORY(libkiroeventhandler)
  ADD_SUBDIRECTORY(libkirogfxplugin)
ENDIF()
