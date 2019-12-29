list_contains(DEVICES konepureoptical contains)
IF(contains)
  add_dbus_bindings(konepureoptical)

  INCLUDE_DIRECTORIES(
    ${CMAKE_SOURCE_DIR}/koneplus/libroccatkoneplus
    ${CMAKE_SOURCE_DIR}/konextd/libroccatkonextd
    ${CMAKE_SOURCE_DIR}/konepure/libroccatkonepure
    ${CMAKE_SOURCE_DIR}/konepureoptical/libroccatkonepureoptical
    ${CMAKE_BINARY_DIR}/konepureoptical
  )

  ADD_SUBDIRECTORY(libroccatkonepureoptical)

  ADD_SUBDIRECTORY(roccatkonepureopticalconfig)
  ADD_SUBDIRECTORY(roccatkonepureopticalcontrol)
  ADD_SUBDIRECTORY(libkonepureopticaleventhandler)
  ADD_SUBDIRECTORY(libkonepureopticalgfxplugin)
ENDIF()
