ADD_LIBRARY(libskeltrgfxplugin SHARED
  skeltrgfxplugin.c
)

INCLUDE_DIRECTORIES(
  ${DBUS_INCLUDE_DIRS}
)

TARGET_LINK_LIBRARIES(libskeltrgfxplugin
  libroccat
  libroccathelper
  ${DBUS_LIBRARIES}
  ${GLIB2_LIBRARIES}
)

SET_TARGET_PROPERTIES(libskeltrgfxplugin
  PROPERTIES
    OUTPUT_NAME skeltrgfxplugin
    COMPILE_FLAGS "-DG_LOG_DOMAIN=\\\"SkeltrGfxplugin\\\" -fvisibility=hidden"
)

INSTALL(TARGETS libskeltrgfxplugin DESTINATION ${GAMINGGEAR0_FX_PLUGIN_DIR})
