list_contains(DEVICES konextdoptical contains)
IF(contains)
  add_dbus_bindings(konextdoptical)

  INCLUDE_DIRECTORIES(
    ${CMAKE_SOURCE_DIR}/koneplus/libroccatkoneplus
    ${CMAKE_SOURCE_DIR}/konepure/libroccatkonepure
    ${CMAKE_SOURCE_DIR}/konextd/libroccatkonextd
    ${CMAKE_SOURCE_DIR}/konextdoptical/libroccatkonextdoptical
    ${CMAKE_SOURCE_DIR}/konepuremilitary/libroccatkonepuremilitary
    ${CMAKE_BINARY_DIR}/konextdoptical
  )

  ADD_SUBDIRECTORY(libroccatkonextdoptical)

  ADD_SUBDIRECTORY(roccatkonextdopticalconfig)
  ADD_SUBDIRECTORY(roccatkonextdopticalcontrol)
  ADD_SUBDIRECTORY(libkonextdopticaleventhandler)
  ADD_SUBDIRECTORY(libkonextdopticalgfxplugin)
ENDIF()
