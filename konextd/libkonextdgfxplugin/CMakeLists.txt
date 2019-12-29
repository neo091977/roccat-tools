ADD_LIBRARY(libkonextdgfxplugin SHARED
  konextdgfxplugin.c
)

INCLUDE_DIRECTORIES(
  ${DBUS_INCLUDE_DIRS}
)

TARGET_LINK_LIBRARIES(libkonextdgfxplugin
  libroccat
  libroccathelper
  ${DBUS_LIBRARIES}
  ${GLIB2_LIBRARIES}
)

SET_TARGET_PROPERTIES(libkonextdgfxplugin
  PROPERTIES
    OUTPUT_NAME konextdgfxplugin
    COMPILE_FLAGS "-DG_LOG_DOMAIN=\\\"KoneXTDGfxplugin\\\" -fvisibility=hidden"
)

INSTALL(TARGETS libkonextdgfxplugin DESTINATION ${GAMINGGEAR0_FX_PLUGIN_DIR})
