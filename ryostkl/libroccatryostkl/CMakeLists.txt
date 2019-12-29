ADD_LIBRARY(libroccatryostkl SHARED
  ryostkl_config.c
  ryostkl_dbus_services.c
  ryostkl_device.c
  ryostkl_led_macro_thread.c
  ryostkl_light_layer.c
  ryostkl_profile_data.c
  ryostkl_profile_data_eventhandler.c
  ryostkl_profile_data_hardware.c
  ryostkl_rkp.c
)

ADD_DEPENDENCIES(libroccatryostkl
  generated_headers
  ryostkl_dbus_bindings
)

TARGET_LINK_LIBRARIES(libroccatryostkl
  libroccatryosmk
  libroccat
  libroccathelper
  ${DBUS-GLIB_LIBRARIES}
  ${GAMINGGEAR0_LIBRARY}
  ${GLIB2_LIBRARIES}
)

SET_TARGET_PROPERTIES(libroccatryostkl
  PROPERTIES
    VERSION ${V_MAJOR}.${V_MINOR}.${V_PATCH}
    SOVERSION ${V_MAJOR}
    OUTPUT_NAME roccatryostkl
    COMPILE_FLAGS "-DG_LOG_DOMAIN=\\\"RyosTKL\\\""
)

INSTALL(TARGETS libroccatryostkl DESTINATION ${LIBDIR})
