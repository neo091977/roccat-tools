ADD_LIBRARY(libroccatsuora SHARED
  suora_config.c
  suora_dbus_services.c
  suora_device.c
  suora_game_mode.c
  suora_illumination.c
  suora_info.c
  suora_keys.c
  suora_key_relations.c
  suora_macro.c
  suora_profile_data.c
  suora_reset.c
  suora_rkp.c
  suora_rkp_keys.c
  suora_rkp_macro.c
)

ADD_DEPENDENCIES(libroccatsuora
  generated_headers
  suora_dbus_bindings
)

TARGET_LINK_LIBRARIES(libroccatsuora
  libroccat
  libroccathelper
  ${DBUS-GLIB_LIBRARIES}
  ${GAMINGGEAR0_LIBRARY}
  ${GLIB2_LIBRARIES}
)

SET_TARGET_PROPERTIES(libroccatsuora
  PROPERTIES
    VERSION ${V_MAJOR}.${V_MINOR}.${V_PATCH}
    SOVERSION ${V_MAJOR}
    OUTPUT_NAME roccatsuora
    COMPILE_FLAGS "-DG_LOG_DOMAIN=\\\"Suora\\\""
)

INSTALL(TARGETS libroccatsuora DESTINATION ${LIBDIR})
