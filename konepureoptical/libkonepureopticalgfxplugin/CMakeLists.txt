ADD_LIBRARY(libkonepureopticalgfxplugin SHARED
  konepureopticalgfxplugin.c
)

INCLUDE_DIRECTORIES(
  ${DBUS_INCLUDE_DIRS}
)

TARGET_LINK_LIBRARIES(libkonepureopticalgfxplugin
  libroccat
  libroccathelper
  ${DBUS_LIBRARIES}
  ${GLIB2_LIBRARIES}
)

SET_TARGET_PROPERTIES(libkonepureopticalgfxplugin
  PROPERTIES
    OUTPUT_NAME konepureopticalgfxplugin
    COMPILE_FLAGS "-DG_LOG_DOMAIN=\\\"KonepureopticalGfxplugin\\\" -fvisibility=hidden"
)

INSTALL(TARGETS libkonepureopticalgfxplugin DESTINATION ${GAMINGGEAR0_FX_PLUGIN_DIR})
