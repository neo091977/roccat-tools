ADD_LIBRARY(libroccatryosmkfx SHARED
  ryosmkfx.c
  ryosmkfx_config.c
  ryosmkfx_custom_lights.c
  ryosmkfx_dbus_services.c
  ryosmkfx_device.c
  ryosmkfx_effect_device.c
  ryosmkfx_gfx.c
  ryosmkfx_key_relations.c
  ryosmkfx_led_macro_thread.c
  ryosmkfx_light.c
  ryosmkfx_light_control.c
  ryosmkfx_light_layer.c
  ryosmkfx_macro.c
  ryosmkfx_profile_data.c
  ryosmkfx_profile_data_eventhandler.c
  ryosmkfx_profile_data_hardware.c
  ryosmkfx_rkp.c
  ryosmkfx_stored_lights.c
)

ADD_DEPENDENCIES(libroccatryosmkfx
  generated_headers
  ryosmkfx_dbus_bindings
)

TARGET_LINK_LIBRARIES(libroccatryosmkfx
  libroccatryosmk
  libroccat
  libroccathelper
  ${DBUS-GLIB_LIBRARIES}
  ${GAMINGGEAR0_LIBRARY}
  ${GLIB2_LIBRARIES}
)

SET_TARGET_PROPERTIES(libroccatryosmkfx
  PROPERTIES
    VERSION ${V_MAJOR}.${V_MINOR}.${V_PATCH}
    SOVERSION ${V_MAJOR}
    OUTPUT_NAME roccatryosmkfx
    COMPILE_FLAGS "-DG_LOG_DOMAIN=\\\"RyosMKFX\\\""
)

INSTALL(TARGETS libroccatryosmkfx DESTINATION ${LIBDIR})
