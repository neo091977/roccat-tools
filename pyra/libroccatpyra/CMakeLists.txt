ADD_LIBRARY(libroccatpyra SHARED
  pyra.c
  pyra_config.c
  pyra_file.c
  pyra_info.c
  pyra_play.c
  pyra_profile_buttons.c
  pyra_profile_settings.c
  pyra_rmp_accessors.c
  pyra_rmp_conversions.c
  pyra_rmp_file.c
  pyra_rmp_high_level.c
  pyra_rmp_macro_key_info.c
  pyra_settings.c
  pyra_dbus.c
)

ADD_DEPENDENCIES(libroccatpyra
  generated_headers
  pyra_dbus_bindings
)

TARGET_LINK_LIBRARIES(libroccatpyra
  libroccat
  libroccathelper
  ${DBUS-GLIB_LIBRARIES}
  ${GAMINGGEAR0_LIBRARY}
  ${GLIB2_LIBRARIES}
)

SET_TARGET_PROPERTIES(libroccatpyra
  PROPERTIES
    VERSION ${V_MAJOR}.${V_MINOR}.${V_PATCH}
    SOVERSION ${V_MAJOR}
    OUTPUT_NAME roccatpyra
    COMPILE_FLAGS "-DG_LOG_DOMAIN=\\\"Pyra\\\""
)

INSTALL(TARGETS libroccatpyra DESTINATION ${LIBDIR})
