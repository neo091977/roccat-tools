list_contains(DEVICES suora contains)
IF(contains)
  add_dbus_bindings(suora)

  INCLUDE_DIRECTORIES(
    ${CMAKE_SOURCE_DIR}/suora/libroccatsuora
    ${CMAKE_BINARY_DIR}/suora
  )

  ADD_SUBDIRECTORY(libroccatsuora)
  
  ADD_SUBDIRECTORY(roccatsuoraconfig)
  ADD_SUBDIRECTORY(roccatsuoracontrol)
  ADD_SUBDIRECTORY(libsuoraeventhandler)
ENDIF()
