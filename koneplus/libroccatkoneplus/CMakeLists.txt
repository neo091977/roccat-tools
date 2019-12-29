ADD_LIBRARY(libroccatkoneplus SHARED
  koneplus.c
  koneplus_actual_profile.c
  koneplus_button_play.c
  koneplus_config.c
  koneplus_control_unit.c
  koneplus_gfx.c
  koneplus_info.c
  koneplus_macro.c
  koneplus_profile_buttons.c
  koneplus_profile_settings.c
  koneplus_rmp_accessors.c
  koneplus_rmp_conversions.c
  koneplus_rmp_file.c
  koneplus_rmp_high_level.c
  koneplus_rmp_light_info.c
  koneplus_rmp_macro_key_info.c
  koneplus_sensor.c
  koneplus_talk.c
  koneplus_dbus_services.c
)

ADD_DEPENDENCIES(libroccatkoneplus
  generated_headers
  koneplus_dbus_bindings
)

TARGET_LINK_LIBRARIES(libroccatkoneplus
  libroccat
  libroccathelper
  ${DBUS-GLIB_LIBRARIES}
  ${GAMINGGEAR0_LIBRARY}
  ${GLIB2_LIBRARIES}
)

SET_TARGET_PROPERTIES(libroccatkoneplus
  PROPERTIES
    VERSION ${V_MAJOR}.${V_MINOR}.${V_PATCH}
    SOVERSION ${V_MAJOR}
    OUTPUT_NAME roccatkoneplus
    COMPILE_FLAGS "-DG_LOG_DOMAIN=\\\"Kone[+]\\\""
)

INSTALL(TARGETS libroccatkoneplus DESTINATION ${LIBDIR})
