list_contains(DEVICES "ryosmk;ryostkl;ryosmkfx" contains)
IF(contains)
  add_dbus_bindings(ryosmk)

  INCLUDE_DIRECTORIES(
    ${CMAKE_SOURCE_DIR}/ryosmk/libroccatryosmk
    ${CMAKE_BINARY_DIR}/ryosmk
  )

  ADD_SUBDIRECTORY(libroccatryosmk)
  ADD_SUBDIRECTORY(libroccatryosmkwidget)

  IF(LUA_FOUND)
    ADD_SUBDIRECTORY(effect_modules)
  ENDIF()
ENDIF()

list_contains(DEVICES ryosmk contains)
IF(contains)
  ADD_SUBDIRECTORY(roccatryosmkconfig)
  ADD_SUBDIRECTORY(roccatryosmkcontrol)
  ADD_SUBDIRECTORY(libryosmkeventhandler)
ENDIF()
