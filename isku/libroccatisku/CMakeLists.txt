ADD_LIBRARY(libroccatisku SHARED
  isku.c
  isku_actual_profile.c
  isku_config.c
  isku_info.c
  isku_key_mask.c
  isku_keys_capslock.c
  isku_keys_easyzone.c
  isku_keys_function.c
  isku_keys_macro.c
  isku_keys_media.c
  isku_keys_thumbster.c
  isku_last_set.c
  isku_light.c
  isku_macro.c
  isku_play.c
  isku_reset.c
  isku_rkp_accessors.c
  isku_rkp_file.c
  isku_rkp_high_level.c
  isku_rkp_macro_key_info.c
  isku_talk.c
  isku_dbus_services.c
)

ADD_DEPENDENCIES(libroccatisku
  generated_headers
  isku_dbus_bindings
)

TARGET_LINK_LIBRARIES(libroccatisku
  libroccat
  libroccathelper
  ${DBUS-GLIB_LIBRARIES}
  ${GAMINGGEAR0_LIBRARY}
  ${GLIB2_LIBRARIES}
)

SET_TARGET_PROPERTIES(libroccatisku
  PROPERTIES
    VERSION ${V_MAJOR}.${V_MINOR}.${V_PATCH}
    SOVERSION ${V_MAJOR}
    OUTPUT_NAME roccatisku
    COMPILE_FLAGS "-DG_LOG_DOMAIN=\\\"Isku\\\""
)

INSTALL(TARGETS libroccatisku DESTINATION ${LIBDIR})
