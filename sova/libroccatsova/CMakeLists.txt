ADD_LIBRARY(libroccatsova SHARED
  sova_config.c
  sova_dbus_services.c
  sova_device.c
  sova_info.c
  sova_key_mask.c
  sova_key_relations.c
  sova_keys_easyzone.c
  sova_keys_extra.c
  sova_keys_function.c
  sova_keys_primary.c
  sova_light.c
  sova_light_control.c
  sova_macro.c
  sova_profile.c
  sova_profile_data.c
  sova_reset.c
  sova_rkp.c
  sova_rkp_sound_feedback.c
  sova_talk.c
)

ADD_DEPENDENCIES(libroccatsova
  generated_headers
  sova_dbus_bindings
)

TARGET_LINK_LIBRARIES(libroccatsova
  libroccat
  libroccathelper
  ${DBUS-GLIB_LIBRARIES}
  ${GAMINGGEAR0_LIBRARY}
  ${GLIB2_LIBRARIES}
)

SET_TARGET_PROPERTIES(libroccatsova
  PROPERTIES
    VERSION ${V_MAJOR}.${V_MINOR}.${V_PATCH}
    SOVERSION ${V_MAJOR}
    OUTPUT_NAME roccatsova
    COMPILE_FLAGS "-DG_LOG_DOMAIN=\\\"Sova\\\""
)

INSTALL(TARGETS libroccatsova DESTINATION ${LIBDIR})
