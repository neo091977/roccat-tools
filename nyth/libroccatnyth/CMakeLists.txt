ADD_LIBRARY(libroccatnyth SHARED
  nyth_button_set.c
  nyth_config.c
  nyth_control_unit.c
  nyth_dbus_services.c
  nyth_device.c
  nyth_device_state.c
  nyth_gfx.c
  nyth_info.c
  nyth_macro.c
  nyth_profile.c
  nyth_profile_buttons.c
  nyth_profile_data.c
  nyth_profile_data_eventhandler.c
  nyth_profile_data_hardware.c
  nyth_profile_settings.c
  nyth_rmp.c
  nyth_talk.c
)

ADD_DEPENDENCIES(libroccatnyth
  generated_headers
  nyth_dbus_bindings
)

TARGET_LINK_LIBRARIES(libroccatnyth
  libroccat
  libroccathelper
  ${DBUS-GLIB_LIBRARIES}
  ${GAMINGGEAR0_LIBRARY}
  ${GLIB2_LIBRARIES}
)

SET_TARGET_PROPERTIES(libroccatnyth
  PROPERTIES
    VERSION ${V_MAJOR}.${V_MINOR}.${V_PATCH}
    SOVERSION ${V_MAJOR}
    OUTPUT_NAME roccatnyth
    COMPILE_FLAGS "-DG_LOG_DOMAIN=\\\"Nyth\\\""
)

INSTALL(TARGETS libroccatnyth DESTINATION ${LIBDIR})
