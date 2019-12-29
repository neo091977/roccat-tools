list_contains(DEVICES ryosmkfx contains)
IF(contains)
  add_dbus_bindings(ryosmkfx)

  INCLUDE_DIRECTORIES(
    ${CMAKE_SOURCE_DIR}/ryosmk/libroccatryosmk
    ${CMAKE_SOURCE_DIR}/ryosmkfx/libroccatryosmkfx
    ${CMAKE_BINARY_DIR}/ryosmkfx
  )

  ADD_SUBDIRECTORY(libroccatryosmkfx)

  ADD_SUBDIRECTORY(roccatryosmkfxconfig)
  ADD_SUBDIRECTORY(roccatryosmkfxcontrol)
  ADD_SUBDIRECTORY(libryosmkfxeventhandler)
  ADD_SUBDIRECTORY(libryosmkfxgfxplugin)
ENDIF()
