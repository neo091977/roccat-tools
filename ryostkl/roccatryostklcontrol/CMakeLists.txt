ADD_EXECUTABLE(roccatryostklcontrol ryostklcontrol.c)

TARGET_LINK_LIBRARIES(roccatryostklcontrol
  libroccatryostkl
  libroccatryosmk
  libroccat
  ${GOBJECT2_LIBRARIES}
  ${GLIB2_LIBRARIES}
)

INSTALL(TARGETS roccatryostklcontrol
  DESTINATION bin
  PERMISSIONS OWNER_READ OWNER_WRITE OWNER_EXECUTE GROUP_READ GROUP_EXECUTE WORLD_READ WORLD_EXECUTE
)

ADD_SUBDIRECTORY(man)
