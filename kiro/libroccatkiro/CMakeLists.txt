ADD_LIBRARY(libroccatkiro SHARED
  kiro_config.c
  kiro_dbus_services.c
  kiro_dcu.c
  kiro_device.c
  kiro_device_state.c
  kiro_gfx.c
  kiro_info.c
  kiro_profile.c
  kiro_profile_data.c
  kiro_profile_data_eventhandler.c
  kiro_profile_data_hardware.c
  kiro_rmp.c
  kiro_talkfx.c
)

ADD_DEPENDENCIES(libroccatkiro
  generated_headers
  kiro_dbus_bindings
)

TARGET_LINK_LIBRARIES(libroccatkiro
  libroccat
  libroccathelper
  ${DBUS-GLIB_LIBRARIES}
  ${GAMINGGEAR0_LIBRARY}
  ${GLIB2_LIBRARIES}
)

SET_TARGET_PROPERTIES(libroccatkiro
  PROPERTIES
    VERSION ${V_MAJOR}.${V_MINOR}.${V_PATCH}
    SOVERSION ${V_MAJOR}
    OUTPUT_NAME roccatkiro
    COMPILE_FLAGS "-DG_LOG_DOMAIN=\\\"Kiro\\\""
)

INSTALL(TARGETS libroccatkiro DESTINATION ${LIBDIR})
