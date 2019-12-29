ADD_LIBRARY(libroccatkovaplus SHARED
  kovaplus.c
  kovaplus_actual_profile.c
  kovaplus_config.c
  kovaplus_info.c
  kovaplus_play.c
  kovaplus_profile_buttons.c
  kovaplus_profile_settings.c
  kovaplus_rmp_accessors.c
  kovaplus_rmp_conversions.c
  kovaplus_rmp_file.c
  kovaplus_rmp_high_level.c
  kovaplus_rmp_macro_key_info.c
  kovaplus_dbus.c
)

ADD_DEPENDENCIES(libroccatkovaplus
  generated_headers
  kovaplus_dbus_bindings
)

TARGET_LINK_LIBRARIES(libroccatkovaplus
  libroccat
  libroccathelper
  ${DBUS-GLIB_LIBRARIES}
  ${GAMINGGEAR0_LIBRARY}
  ${GLIB2_LIBRARIES}
)

SET_TARGET_PROPERTIES(libroccatkovaplus
  PROPERTIES
    VERSION ${V_MAJOR}.${V_MINOR}.${V_PATCH}
    SOVERSION ${V_MAJOR}
    OUTPUT_NAME roccatkovaplus
    COMPILE_FLAGS "-DG_LOG_DOMAIN=\\\"Kova[+]\\\""
)

INSTALL(TARGETS libroccatkovaplus DESTINATION ${LIBDIR})
