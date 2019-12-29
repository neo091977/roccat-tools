ADD_LIBRARY(libiskufxgfxplugin SHARED
  iskufxgfxplugin.c
)

INCLUDE_DIRECTORIES(
  ${DBUS_INCLUDE_DIRS}
)

TARGET_LINK_LIBRARIES(libiskufxgfxplugin
  libroccat
  libroccathelper
  ${DBUS_LIBRARIES}
  ${GLIB2_LIBRARIES}
)

SET_TARGET_PROPERTIES(libiskufxgfxplugin
  PROPERTIES
    OUTPUT_NAME iskufxgfxplugin
    COMPILE_FLAGS "-DG_LOG_DOMAIN=\\\"IskuFXGfxplugin\\\" -fvisibility=hidden"
)

INSTALL(TARGETS libiskufxgfxplugin DESTINATION ${GAMINGGEAR0_FX_PLUGIN_DIR})
