ADD_LIBRARY(libroccatiskufx SHARED
  iskufx.c
  iskufx_config.c
  iskufx_dbus_services.c
  iskufx_gfx.c
  iskufx_light.c
  iskufx_rkp_accessors.c
  iskufx_rkp_file.c
  iskufx_rkp_high_level.c
  iskufx_rkp_light_info.c
  iskufx_talkfx.c
)

ADD_DEPENDENCIES(libroccatiskufx
  generated_headers
  iskufx_dbus_bindings
)

TARGET_LINK_LIBRARIES(libroccatiskufx
  libroccatisku
  libroccat
  libroccathelper
  ${DBUS-GLIB_LIBRARIES}
  ${GAMINGGEAR0_LIBRARY}
  ${GLIB2_LIBRARIES}
)

SET_TARGET_PROPERTIES(libroccatiskufx
  PROPERTIES
    VERSION ${V_MAJOR}.${V_MINOR}.${V_PATCH}
    SOVERSION ${V_MAJOR}
    OUTPUT_NAME roccatiskufx
    COMPILE_FLAGS "-DG_LOG_DOMAIN=\\\"IskuFX\\\""
)

INSTALL(TARGETS libroccatiskufx DESTINATION ${LIBDIR})
