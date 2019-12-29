ADD_LIBRARY(libroccatskeltr SHARED
  skeltr_bluetooth.c
  skeltr_config.c
  skeltr_dbus_services.c
  skeltr_device.c
  skeltr_gfx.c
  skeltr_info.c
  skeltr_key_mask.c
  skeltr_key_relations.c
  skeltr_keys_easyzone.c
  skeltr_keys_extra.c
  skeltr_keys_function.c
  skeltr_keys_macro.c
  skeltr_keys_primary.c
  skeltr_keys_thumbster.c
  skeltr_light.c
  skeltr_light_control.c
  skeltr_light_custom.c
  skeltr_macro.c
  skeltr_profile.c
  skeltr_profile_data.c
  skeltr_reset.c
  skeltr_rkp.c
  skeltr_rkp_keys_easyzone.c
  skeltr_rkp_sound_feedback.c
  skeltr_talk.c
)

ADD_DEPENDENCIES(libroccatskeltr
  generated_headers
  skeltr_dbus_bindings
)

TARGET_LINK_LIBRARIES(libroccatskeltr
  libroccat
  libroccathelper
  ${DBUS-GLIB_LIBRARIES}
  ${GAMINGGEAR0_LIBRARY}
  ${GLIB2_LIBRARIES}
)

SET_TARGET_PROPERTIES(libroccatskeltr
  PROPERTIES
    VERSION ${V_MAJOR}.${V_MINOR}.${V_PATCH}
    SOVERSION ${V_MAJOR}
    OUTPUT_NAME roccatskeltr
    COMPILE_FLAGS "-DG_LOG_DOMAIN=\\\"Skeltr\\\""
)

INSTALL(TARGETS libroccatskeltr DESTINATION ${LIBDIR})
