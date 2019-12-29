ADD_EXECUTABLE(roccatkonextdopticalcontrol konextdopticalcontrol.c)

TARGET_LINK_LIBRARIES(roccatkonextdopticalcontrol
  libroccatkonextdoptical
  libroccatkonepuremilitary
  libroccatkoneplus
  libroccat
  ${GLIB2_LIBRARIES}
)

INSTALL(TARGETS roccatkonextdopticalcontrol
  DESTINATION bin
  PERMISSIONS OWNER_READ OWNER_WRITE OWNER_EXECUTE GROUP_READ GROUP_EXECUTE WORLD_READ WORLD_EXECUTE
)

ADD_SUBDIRECTORY(man)
