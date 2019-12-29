ADD_LIBRARY(libroccatkonextdoptical SHARED
  konextdoptical.c
  konextdoptical_config.c
  konextdoptical_dbus_services.c
  konextdoptical_profile_settings.c
  konextdoptical_rmp_conversions.c
  konextdoptical_rmp_file.c
  konextdoptical_rmp_high_level.c
)

ADD_DEPENDENCIES(libroccatkonextdoptical
  generated_headers
  konextdoptical_dbus_bindings
)

TARGET_LINK_LIBRARIES(libroccatkonextdoptical
  libroccat
  libroccatkonextd
  libroccatkoneplus
  libroccatkonepuremilitary
  libroccathelper
  ${DBUS-GLIB_LIBRARIES}
  ${GAMINGGEAR0_LIBRARY}
  ${GLIB2_LIBRARIES}
)

SET_TARGET_PROPERTIES(libroccatkonextdoptical
  PROPERTIES
    VERSION ${V_MAJOR}.${V_MINOR}.${V_PATCH}
    SOVERSION ${V_MAJOR}
    OUTPUT_NAME roccatkonextdoptical
    COMPILE_FLAGS "-DG_LOG_DOMAIN=\\\"KoneXTDOptical\\\""
)

INSTALL(TARGETS libroccatkonextdoptical DESTINATION ${LIBDIR})
