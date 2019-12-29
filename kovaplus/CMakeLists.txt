list_contains(DEVICES kovaplus contains)
IF(contains)
  add_dbus_bindings(kovaplus)

  INCLUDE_DIRECTORIES(
    ${CMAKE_SOURCE_DIR}/kovaplus/libroccatkovaplus
    ${CMAKE_BINARY_DIR}/kovaplus
  )

  ADD_SUBDIRECTORY(libroccatkovaplus)

  ADD_SUBDIRECTORY(roccatkovaplusconfig)
  ADD_SUBDIRECTORY(roccatkovapluscontrol)
  ADD_SUBDIRECTORY(libkovapluseventhandler)
ENDIF()
