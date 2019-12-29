list_contains(DEVICES kone contains)
IF(contains)
  add_dbus_bindings(kone)

  INCLUDE_DIRECTORIES(
    ${CMAKE_SOURCE_DIR}/kone/libroccatkone
    ${CMAKE_BINARY_DIR}/kone
  )

  ADD_SUBDIRECTORY(libroccatkone)

  ADD_SUBDIRECTORY(roccatkoneconfig)
  ADD_SUBDIRECTORY(roccatkonecontrol)
  ADD_SUBDIRECTORY(libkoneeventhandler)
ENDIF()
