list_contains(DEVICES "konepure;konepuremilitary;konepureoptical;konextdoptical" contains)
IF(contains)
  add_dbus_bindings(konepure)

  INCLUDE_DIRECTORIES(
    ${CMAKE_SOURCE_DIR}/koneplus/libroccatkoneplus
    ${CMAKE_SOURCE_DIR}/konextd/libroccatkonextd
    ${CMAKE_SOURCE_DIR}/konepure/libroccatkonepure
    ${CMAKE_BINARY_DIR}/konepure
  )

  ADD_SUBDIRECTORY(libroccatkonepure)
  ADD_SUBDIRECTORY(libroccatkonepurewidget)
ENDIF()

list_contains(DEVICES konepure contains)
IF(contains)
  ADD_SUBDIRECTORY(roccatkonepureconfig)
  ADD_SUBDIRECTORY(roccatkonepurecontrol)
  ADD_SUBDIRECTORY(libkonepureeventhandler)
  ADD_SUBDIRECTORY(libkonepuregfxplugin)
ENDIF()
