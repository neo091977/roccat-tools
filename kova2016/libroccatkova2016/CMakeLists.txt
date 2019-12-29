ADD_LIBRARY(libroccatkova2016 SHARED
  kova2016_config.c
  kova2016_control_unit.c
  kova2016_dbus_services.c
  kova2016_device.c
  kova2016_device_state.c
  kova2016_gfx.c
  kova2016_info.c
  kova2016_macro.c
  kova2016_profile.c
  kova2016_profile_buttons.c
  kova2016_profile_data.c
  kova2016_profile_data_eventhandler.c
  kova2016_profile_data_hardware.c
  kova2016_profile_settings.c
  kova2016_rmp.c
  kova2016_sensor.c
  kova2016_talk.c
)

ADD_DEPENDENCIES(libroccatkova2016
  generated_headers
  kova2016_dbus_bindings
)

TARGET_LINK_LIBRARIES(libroccatkova2016
  libroccat
  libroccathelper
  ${DBUS-GLIB_LIBRARIES}
  ${GAMINGGEAR0_LIBRARY}
  ${GLIB2_LIBRARIES}
)

SET_TARGET_PROPERTIES(libroccatkova2016
  PROPERTIES
    VERSION ${V_MAJOR}.${V_MINOR}.${V_PATCH}
    SOVERSION ${V_MAJOR}
    OUTPUT_NAME roccatkova2016
    COMPILE_FLAGS "-DG_LOG_DOMAIN=\\\"Kova2016\\\""
)

INSTALL(TARGETS libroccatkova2016 DESTINATION ${LIBDIR})
