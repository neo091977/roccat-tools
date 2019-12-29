ADD_LIBRARY(libroccatarvo SHARED
  arvo.c
  arvo_button.c
  arvo_config.c
  arvo_file.c
  arvo_info.c
  arvo_play.c
  arvo_rkp_accessors.c
  arvo_rkp_button_info.c
  arvo_rkp_file.c
  arvo_rkp_high_level.c
  arvo_dbus.c
)

ADD_DEPENDENCIES(libroccatarvo
  generated_headers
  arvo_dbus_bindings
)

TARGET_LINK_LIBRARIES(libroccatarvo
  libroccat
  libroccathelper
  ${DBUS-GLIB_LIBRARIES}
  ${GAMINGGEAR0_LIBRARY}
  ${GLIB2_LIBRARIES}
)

SET_TARGET_PROPERTIES(libroccatarvo
  PROPERTIES
    VERSION ${V_MAJOR}.${V_MINOR}.${V_PATCH}
    SOVERSION ${V_MAJOR}
    OUTPUT_NAME roccatarvo
    COMPILE_FLAGS "-DG_LOG_DOMAIN=\\\"Arvo\\\""
)

INSTALL(TARGETS libroccatarvo DESTINATION ${LIBDIR})
