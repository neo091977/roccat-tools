ADD_EXECUTABLE(roccatnythcontrol nythcontrol.c)

TARGET_LINK_LIBRARIES(roccatnythcontrol
  libroccatnyth
  libroccat
  ${GLIB2_LIBRARIES}
)

INSTALL(TARGETS roccatnythcontrol
  DESTINATION bin
  PERMISSIONS OWNER_READ OWNER_WRITE OWNER_EXECUTE GROUP_READ GROUP_EXECUTE WORLD_READ WORLD_EXECUTE
)

ADD_SUBDIRECTORY(man)
