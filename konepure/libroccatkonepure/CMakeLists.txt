ADD_LIBRARY(libroccatkonepure SHARED
  konepure.c
  konepure_config.c
  konepure_dbus_services.c
  konepure_profile_buttons.c
  konepure_profile_settings.c
  konepure_rmp_conversions.c
  konepure_rmp_file.c
  konepure_rmp_high_level.c
  konepure_rmp_light_info.c
)

ADD_DEPENDENCIES(libroccatkonepure
  generated_headers
  konepure_dbus_bindings
)

TARGET_LINK_LIBRARIES(libroccatkonepure
  libroccat
  libroccatkoneplus
  libroccatkonextd
  libroccathelper
  ${DBUS-GLIB_LIBRARIES}
  ${GAMINGGEAR0_LIBRARY}
  ${GLIB2_LIBRARIES}
)

SET_TARGET_PROPERTIES(libroccatkonepure
  PROPERTIES
    VERSION ${V_MAJOR}.${V_MINOR}.${V_PATCH}
    SOVERSION ${V_MAJOR}
    OUTPUT_NAME roccatkonepure
    COMPILE_FLAGS "-DG_LOG_DOMAIN=\\\"KonePure\\\""
)

INSTALL(TARGETS libroccatkonepure DESTINATION ${LIBDIR})
