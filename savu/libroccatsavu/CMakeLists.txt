ADD_LIBRARY(libroccatsavu SHARED
  savu.c
  savu_buttons.c
  savu_config.c
  savu_dbus_services.c
  savu_general.c
  savu_info.c
  savu_macro.c
  savu_play.c
  savu_profile.c
  savu_rmp_accessors.c
  savu_rmp_file.c
  savu_rmp_high_level.c
  savu_sensor.c
  savu_timer.c
)

ADD_DEPENDENCIES(libroccatsavu
  generated_headers
  savu_dbus_bindings
)

TARGET_LINK_LIBRARIES(libroccatsavu
  libroccat
  libroccathelper
  ${DBUS-GLIB_LIBRARIES}
  ${GAMINGGEAR0_LIBRARY}
  ${GLIB2_LIBRARIES}
)

SET_TARGET_PROPERTIES(libroccatsavu
  PROPERTIES
    VERSION ${V_MAJOR}.${V_MINOR}.${V_PATCH}
    SOVERSION ${V_MAJOR}
    OUTPUT_NAME roccatsavu
    COMPILE_FLAGS "-DG_LOG_DOMAIN=\\\"Savu\\\""
)

INSTALL(TARGETS libroccatsavu DESTINATION ${LIBDIR})
