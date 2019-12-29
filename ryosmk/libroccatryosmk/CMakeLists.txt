SET(SOURCES
  ryos_config.c
  ryos_custom_lights.c
  ryos_dbus_services.c
  ryos_default_rkp.c
  ryos_device.c
  ryos_device_state.c
  ryos_effect_client.c
  ryos_effect_device.c
  ryos_info.c
  ryos_key.c
  ryos_key_mask.c
  ryos_key_relations.c
  ryos_keys_easyzone.c
  ryos_keys_extra.c
  ryos_keys_function.c
  ryos_keys_macro.c
  ryos_keys_primary.c
  ryos_keys_thumbster.c
  ryos_led_macro.c
  ryos_led_macro_thread.c
  ryos_light.c
  ryos_light_control.c
  ryos_macro.c
  ryos_profile.c
  ryos_profile_data.c
  ryos_profile_data_eventhandler.c
  ryos_profile_data_hardware.c
  ryos_reset.c
  ryos_rkp.c
  ryos_rkp_accessors.c
  ryos_rkp_quicklaunch.c
  ryos_rkp_talk.c
  ryos_rkp_timer.c
  ryos_sdk.c
  ryos_stored_lights.c
  ryos_talk.c
)

SET(LIBS
  libroccat
  libroccathelper
  ${DBUS-GLIB_LIBRARIES}
  ${GAMINGGEAR0_LIBRARY}
  ${GLIB2_LIBRARIES}
)

IF(LUA_FOUND)
  LIST(APPEND LIBS ${LUA_LIBRARY})
  LIST(APPEND SOURCES ryos_effect_lua.c)
  INCLUDE_DIRECTORIES(${LUA_INCLUDE_DIR})
ENDIF()

ADD_LIBRARY(libroccatryosmk SHARED ${SOURCES})

ADD_DEPENDENCIES(libroccatryosmk
  generated_headers
  ryosmk_dbus_bindings
)

TARGET_LINK_LIBRARIES(libroccatryosmk ${LIBS})

SET_TARGET_PROPERTIES(libroccatryosmk
  PROPERTIES
    VERSION ${V_MAJOR}.${V_MINOR}.${V_PATCH}
    SOVERSION ${V_MAJOR}
    OUTPUT_NAME roccatryosmk
    COMPILE_FLAGS "-DG_LOG_DOMAIN=\\\"RyosMK\\\""
)

INSTALL(TARGETS libroccatryosmk DESTINATION ${LIBDIR})
