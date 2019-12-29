ADD_LIBRARY(libroccatkone SHARED
  kone_config.c
  kone_device_scanner.c
  kone_file.c
  kone_profile.c
  kone_rmp_accessors.c
  kone_rmp_button_info.c
  kone_rmp_conversions.c
  kone_rmp_file.c
  kone_rmp_high_level.c
  kone_rmp_light_info.c
  kone_settings.c
  kone_dbus.c
)

ADD_DEPENDENCIES(libroccatkone
  generated_headers
  kone_dbus_bindings
)

TARGET_LINK_LIBRARIES(libroccatkone
  libroccat
  libroccathelper
  ${DBUS-GLIB_LIBRARIES}
  ${GAMINGGEAR0_LIBRARY}
  ${GUDEV_LIBRARIES}
  ${GLIB2_LIBRARIES}
)

SET_TARGET_PROPERTIES(libroccatkone
  PROPERTIES
    VERSION ${V_MAJOR}.${V_MINOR}.${V_PATCH}
    SOVERSION ${V_MAJOR}
    OUTPUT_NAME roccatkone
    COMPILE_FLAGS "-DG_LOG_DOMAIN=\\\"Kone\\\""
)

INSTALL(TARGETS libroccatkone DESTINATION ${LIBDIR})
