list_contains(DEVICES "konextd;konepure;konepuremilitary;konepureoptical;konextdoptical" contains)
IF(contains)
  add_dbus_bindings(konextd)

  INCLUDE_DIRECTORIES(
    ${CMAKE_SOURCE_DIR}/koneplus/libroccatkoneplus
    ${CMAKE_SOURCE_DIR}/konextd/libroccatkonextd
    ${CMAKE_BINARY_DIR}/konextd
  )

  ADD_SUBDIRECTORY(libroccatkonextd)
  ADD_SUBDIRECTORY(libroccatkonextdwidget)
ENDIF()

list_contains(DEVICES konextd contains)
IF(contains)
  ADD_SUBDIRECTORY(roccatkonextdconfig)
  ADD_SUBDIRECTORY(roccatkonextdcontrol)
  ADD_SUBDIRECTORY(libkonextdeventhandler)
  ADD_SUBDIRECTORY(libkonextdgfxplugin)
ENDIF()
