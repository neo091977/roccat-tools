ADD_LIBRARY(libroccatkonepuremilitary SHARED
  konepuremilitary.c
  konepuremilitary_config.c
  konepuremilitary_dbus_services.c
  konepuremilitary_profile_settings.c
  konepuremilitary_rmp_accessors.c
  konepuremilitary_rmp_conversions.c
  konepuremilitary_rmp_file.c
  konepuremilitary_rmp_high_level.c
)

ADD_DEPENDENCIES(libroccatkonepuremilitary
  generated_headers
  konepuremilitary_dbus_bindings
)

TARGET_LINK_LIBRARIES(libroccatkonepuremilitary
  libroccat
  libroccathelper
  libroccatkoneplus
  libroccatkonepure
  libroccatkonextd
  ${DBUS-GLIB_LIBRARIES}
  ${GAMINGGEAR0_LIBRARY}
  ${GLIB2_LIBRARIES}
)

SET_TARGET_PROPERTIES(libroccatkonepuremilitary
  PROPERTIES
    VERSION ${V_MAJOR}.${V_MINOR}.${V_PATCH}
    SOVERSION ${V_MAJOR}
    OUTPUT_NAME roccatkonepuremilitary
    COMPILE_FLAGS "-DG_LOG_DOMAIN=\\\"KonePureMilitary\\\""
)

INSTALL(TARGETS libroccatkonepuremilitary DESTINATION ${LIBDIR})
