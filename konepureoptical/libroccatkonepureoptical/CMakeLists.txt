ADD_LIBRARY(libroccatkonepureoptical SHARED
  konepureoptical.c
  konepureoptical_config.c
  konepureoptical_dbus_services.c
  konepureoptical_profile_settings.c
  konepureoptical_rmp_conversions.c
  konepureoptical_rmp_file.c
  konepureoptical_rmp_high_level.c
)

ADD_DEPENDENCIES(libroccatkonepureoptical
  generated_headers
  konepureoptical_dbus_bindings
)

TARGET_LINK_LIBRARIES(libroccatkonepureoptical
  libroccat
  libroccathelper
  libroccatkoneplus
  libroccatkonepure
  libroccatkonextd
  ${DBUS-GLIB_LIBRARIES}
  ${GAMINGGEAR0_LIBRARY}
  ${GLIB2_LIBRARIES}
)

SET_TARGET_PROPERTIES(libroccatkonepureoptical
  PROPERTIES
    VERSION ${V_MAJOR}.${V_MINOR}.${V_PATCH}
    SOVERSION ${V_MAJOR}
    OUTPUT_NAME roccatkonepureoptical
    COMPILE_FLAGS "-DG_LOG_DOMAIN=\\\"KonePureOptical\\\""
)

INSTALL(TARGETS libroccatkonepureoptical DESTINATION ${LIBDIR})
