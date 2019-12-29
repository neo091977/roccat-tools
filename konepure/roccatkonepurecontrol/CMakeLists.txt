ADD_EXECUTABLE(roccatkonepurecontrol konepurecontrol.c)

TARGET_LINK_LIBRARIES(roccatkonepurecontrol
  libroccatkonepure
  libroccatkoneplus
  libroccat
  ${GLIB2_LIBRARIES}
)

INSTALL(TARGETS roccatkonepurecontrol
  DESTINATION bin
  PERMISSIONS OWNER_READ OWNER_WRITE OWNER_EXECUTE GROUP_READ GROUP_EXECUTE WORLD_READ WORLD_EXECUTE
)

ADD_SUBDIRECTORY(man)
