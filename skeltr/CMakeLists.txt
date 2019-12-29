list_contains(DEVICES skeltr contains)
IF(contains)
  add_dbus_bindings(skeltr)

  INCLUDE_DIRECTORIES(
    ${CMAKE_SOURCE_DIR}/skeltr/libroccatskeltr
    ${CMAKE_BINARY_DIR}/skeltr
  )

  ADD_SUBDIRECTORY(libroccatskeltr)
  ADD_SUBDIRECTORY(roccatskeltrconfig)
  ADD_SUBDIRECTORY(roccatskeltrcontrol)
  ADD_SUBDIRECTORY(libskeltreventhandler)
  ADD_SUBDIRECTORY(libskeltrgfxplugin)
ENDIF()
