list_contains(DEVICES savu contains)
IF(contains)
  add_dbus_bindings(savu)

  INCLUDE_DIRECTORIES(
    ${CMAKE_SOURCE_DIR}/savu/libroccatsavu
    ${CMAKE_BINARY_DIR}/savu
  )

  ADD_SUBDIRECTORY(libroccatsavu)

  ADD_SUBDIRECTORY(roccatsavuconfig)
  ADD_SUBDIRECTORY(roccatsavucontrol)
  ADD_SUBDIRECTORY(libsavueventhandler)
ENDIF()
