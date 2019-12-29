list_contains(DEVICES "isku;iskufx" contains)
IF(contains)
  add_dbus_bindings(isku)

  INCLUDE_DIRECTORIES(
    ${CMAKE_SOURCE_DIR}/isku/libroccatisku
    ${CMAKE_BINARY_DIR}/isku
  )

  ADD_SUBDIRECTORY(libroccatisku)
  ADD_SUBDIRECTORY(libroccatiskuwidget)
ENDIF()

list_contains(DEVICES isku contains)
IF(contains)
  ADD_SUBDIRECTORY(roccatiskuconfig)
  ADD_SUBDIRECTORY(roccatiskucontrol)
  ADD_SUBDIRECTORY(libiskueventhandler)
ENDIF()
