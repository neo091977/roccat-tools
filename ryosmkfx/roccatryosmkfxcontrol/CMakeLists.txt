ADD_EXECUTABLE(roccatryosmkfxcontrol ryosmkfxcontrol.c)

TARGET_LINK_LIBRARIES(roccatryosmkfxcontrol
  libroccatryosmkfx
  libroccatryosmk
  libroccat
  ${GLIB2_LIBRARIES}
)

INSTALL(TARGETS roccatryosmkfxcontrol
  DESTINATION bin
  PERMISSIONS OWNER_READ OWNER_WRITE OWNER_EXECUTE GROUP_READ GROUP_EXECUTE WORLD_READ WORLD_EXECUTE
)

ADD_SUBDIRECTORY(man)
