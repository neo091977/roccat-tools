ADD_EXECUTABLE(roccatkonepuremilitarycontrol konepuremilitarycontrol.c)

TARGET_LINK_LIBRARIES(roccatkonepuremilitarycontrol
  libroccatkonepuremilitary
  libroccatkoneplus
  libroccat
  ${GLIB2_LIBRARIES}
)

INSTALL(TARGETS roccatkonepuremilitarycontrol
  DESTINATION bin
  PERMISSIONS OWNER_READ OWNER_WRITE OWNER_EXECUTE GROUP_READ GROUP_EXECUTE WORLD_READ WORLD_EXECUTE
)

ADD_SUBDIRECTORY(man)
