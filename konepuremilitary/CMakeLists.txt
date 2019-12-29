list_contains(DEVICES "konepuremilitary;konextdoptical" contains)
IF(contains)
  add_dbus_bindings(konepuremilitary)

  INCLUDE_DIRECTORIES(
    ${CMAKE_SOURCE_DIR}/koneplus/libroccatkoneplus
    ${CMAKE_SOURCE_DIR}/konextd/libroccatkonextd
    ${CMAKE_SOURCE_DIR}/konepure/libroccatkonepure
    ${CMAKE_SOURCE_DIR}/konepuremilitary/libroccatkonepuremilitary
    ${CMAKE_BINARY_DIR}/konepuremilitary
  )

  ADD_SUBDIRECTORY(libroccatkonepuremilitary)
  ADD_SUBDIRECTORY(libroccatkonepuremilitarywidget)
ENDIF()

list_contains(DEVICES konepuremilitary contains)
IF(contains)
  ADD_SUBDIRECTORY(roccatkonepuremilitaryconfig)
  ADD_SUBDIRECTORY(roccatkonepuremilitarycontrol)
  ADD_SUBDIRECTORY(libkonepuremilitaryeventhandler)
  ADD_SUBDIRECTORY(libkonepuremilitarygfxplugin)
ENDIF()
