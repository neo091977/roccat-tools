ADD_LIBRARY(libkoneplusgfxplugin SHARED
  koneplusgfxplugin.c
)

INCLUDE_DIRECTORIES(
  ${DBUS_INCLUDE_DIRS}
)

TARGET_LINK_LIBRARIES(libkoneplusgfxplugin
  libroccat
  libroccathelper
  ${DBUS_LIBRARIES}
  ${GLIB2_LIBRARIES}
)

SET_TARGET_PROPERTIES(libkoneplusgfxplugin
  PROPERTIES
    OUTPUT_NAME koneplusgfxplugin
    COMPILE_FLAGS "-DG_LOG_DOMAIN=\\\"KoneplusGfxplugin\\\" -fvisibility=hidden"
)

INSTALL(TARGETS libkoneplusgfxplugin DESTINATION ${GAMINGGEAR0_FX_PLUGIN_DIR})
