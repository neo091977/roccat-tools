list_contains(DEVICES "koneplus;konepure;konepuremilitary;konepureoptical;konextd;konextdoptical" contains)
IF(contains)
  add_dbus_bindings(koneplus)

  INCLUDE_DIRECTORIES(
    ${CMAKE_SOURCE_DIR}/koneplus/libroccatkoneplus
    ${CMAKE_BINARY_DIR}/koneplus
  )

  ADD_SUBDIRECTORY(libroccatkoneplus)
  ADD_SUBDIRECTORY(libroccatkonepluswidget)
ENDIF()

list_contains(DEVICES koneplus contains)
IF(contains)
  ADD_SUBDIRECTORY(roccatkoneplusconfig)
  ADD_SUBDIRECTORY(roccatkonepluscontrol)
  ADD_SUBDIRECTORY(libkonepluseventhandler)
  ADD_SUBDIRECTORY(libkoneplusgfxplugin)
ENDIF()
