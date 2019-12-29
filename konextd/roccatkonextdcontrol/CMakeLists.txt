ADD_EXECUTABLE(roccatkonextdcontrol konextdcontrol.c)

TARGET_LINK_LIBRARIES(roccatkonextdcontrol
  libroccatkonextd
  libroccatkoneplus
  libroccat
  ${GLIB2_LIBRARIES}
)

INSTALL(TARGETS roccatkonextdcontrol
  DESTINATION bin
  PERMISSIONS OWNER_READ OWNER_WRITE OWNER_EXECUTE GROUP_READ GROUP_EXECUTE WORLD_READ WORLD_EXECUTE
)

ADD_SUBDIRECTORY(man)
