list_contains(DEVICES ryostkl contains)
IF(contains)
  add_dbus_bindings(ryostkl)

  INCLUDE_DIRECTORIES(
    ${CMAKE_SOURCE_DIR}/ryosmk/libroccatryosmk
    ${CMAKE_SOURCE_DIR}/ryostkl/libroccatryostkl
    ${CMAKE_BINARY_DIR}/ryostkl
  )

  ADD_SUBDIRECTORY(libroccatryostkl)

  ADD_SUBDIRECTORY(roccatryostklconfig)
  ADD_SUBDIRECTORY(roccatryostklcontrol)
  ADD_SUBDIRECTORY(libryostkleventhandler)
ENDIF()
