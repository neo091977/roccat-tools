ADD_LIBRARY(libroccattyon SHARED
  tyon_config.c
  tyon_control_unit.c
  tyon_dbus_services.c
  tyon_device.c
  tyon_device_state.c
  tyon_gfx.c
  tyon_info.c
  tyon_macro.c
  tyon_profile_buttons.c
  tyon_profile.c
  tyon_profile_settings.c
  tyon_rmp.c
  tyon_rmp_accessors.c
  tyon_rmp_conversions.c
  tyon_rmp_high_level.c
  tyon_rmp_light_info.c
  tyon_rmp_macro_key_info.c
  tyon_sensor.c
  tyon_talk.c
  tyon_xcelerator.c
)

ADD_DEPENDENCIES(libroccattyon
  generated_headers
  tyon_dbus_bindings
)

TARGET_LINK_LIBRARIES(libroccattyon
  libroccat
  libroccathelper
  ${DBUS-GLIB_LIBRARIES}
  ${USB1_LIBRARIES}
  ${GAMINGGEAR0_LIBRARY}
  ${GLIB2_LIBRARIES}
)

SET_TARGET_PROPERTIES(libroccattyon
  PROPERTIES
    VERSION ${V_MAJOR}.${V_MINOR}.${V_PATCH}
    SOVERSION ${V_MAJOR}
    OUTPUT_NAME roccattyon
    COMPILE_FLAGS "-DG_LOG_DOMAIN=\\\"Tyon\\\""
)

INSTALL(TARGETS libroccattyon DESTINATION ${LIBDIR})
