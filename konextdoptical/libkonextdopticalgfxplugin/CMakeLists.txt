ADD_LIBRARY(libkonextdopticalgfxplugin SHARED
  konextdopticalgfxplugin.c
)

INCLUDE_DIRECTORIES(
  ${DBUS_INCLUDE_DIRS}
)

TARGET_LINK_LIBRARIES(libkonextdopticalgfxplugin
  libroccat
  libroccathelper
  ${DBUS_LIBRARIES}
  ${GLIB2_LIBRARIES}
)

SET_TARGET_PROPERTIES(libkonextdopticalgfxplugin
  PROPERTIES
    OUTPUT_NAME konextdopticalgfxplugin
    COMPILE_FLAGS "-DG_LOG_DOMAIN=\\\"KonextdopticalGfxplugin\\\" -fvisibility=hidden"
)

INSTALL(TARGETS libkonextdopticalgfxplugin DESTINATION ${GAMINGGEAR0_FX_PLUGIN_DIR})
