ADD_EXECUTABLE(roccatiskufxcontrol iskufxcontrol.c)

TARGET_LINK_LIBRARIES(roccatiskufxcontrol
  libroccatiskufx
  libroccatisku
  libroccat
  ${GLIB2_LIBRARIES}
)

INSTALL(TARGETS roccatiskufxcontrol
  DESTINATION bin
  PERMISSIONS OWNER_READ OWNER_WRITE OWNER_EXECUTE GROUP_READ GROUP_EXECUTE WORLD_READ WORLD_EXECUTE
)

ADD_SUBDIRECTORY(man)
