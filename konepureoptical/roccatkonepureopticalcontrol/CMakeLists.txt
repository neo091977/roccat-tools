ADD_EXECUTABLE(roccatkonepureopticalcontrol konepureopticalcontrol.c)

TARGET_LINK_LIBRARIES(roccatkonepureopticalcontrol
  libroccatkonepureoptical
  libroccatkoneplus
  libroccat
  ${GLIB2_LIBRARIES}
)

INSTALL(TARGETS roccatkonepureopticalcontrol
  DESTINATION bin
  PERMISSIONS OWNER_READ OWNER_WRITE OWNER_EXECUTE GROUP_READ GROUP_EXECUTE WORLD_READ WORLD_EXECUTE
)

ADD_SUBDIRECTORY(man)
