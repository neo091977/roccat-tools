ADD_LIBRARY(libroccatkonextd SHARED
  konextd.c
  konextd_config.c
  konextd_dbus_services.c
  konextd_gfx.c
  konextd_rmp_accessors.c
  konextd_rmp_conversions.c
  konextd_rmp_file.c
  konextd_rmp_high_level.c
)

ADD_DEPENDENCIES(libroccatkonextd
  generated_headers
  konextd_dbus_bindings
)

TARGET_LINK_LIBRARIES(libroccatkonextd
  libroccat
  libroccatkoneplus
  libroccathelper
  ${DBUS-GLIB_LIBRARIES}
  ${GAMINGGEAR0_LIBRARY}
  ${GLIB2_LIBRARIES}
)

SET_TARGET_PROPERTIES(libroccatkonextd
  PROPERTIES
    VERSION ${V_MAJOR}.${V_MINOR}.${V_PATCH}
    SOVERSION ${V_MAJOR}
    OUTPUT_NAME roccatkonextd
    COMPILE_FLAGS "-DG_LOG_DOMAIN=\\\"KoneXTD\\\""
)

INSTALL(TARGETS libroccatkonextd DESTINATION ${LIBDIR})
