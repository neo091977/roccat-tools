list_contains(DEVICES kova2016 contains)
IF(contains)
  add_dbus_bindings(kova2016)

  INCLUDE_DIRECTORIES(
    ${CMAKE_SOURCE_DIR}/kova2016/libroccatkova2016
    ${CMAKE_BINARY_DIR}/kova2016
  )
  
  ADD_SUBDIRECTORY(libroccatkova2016)
  ADD_SUBDIRECTORY(roccatkova2016control)
  ADD_SUBDIRECTORY(roccatkova2016config)
  ADD_SUBDIRECTORY(libkova2016eventhandler)
  ADD_SUBDIRECTORY(libkova2016gfxplugin)
ENDIF()
