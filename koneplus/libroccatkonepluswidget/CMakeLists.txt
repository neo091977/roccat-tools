ADD_LIBRARY(libroccatkonepluswidget SHARED
  koneplus_buttons_frame.c
  koneplus_calibration_assistant.c
  koneplus_cpi_selector.c
  koneplus_dcu_frame.c
  koneplus_info_dialog.c
  koneplus_key_combo_box.c
  koneplus_light_effects_frame.c
  koneplus_tcu_frame.c
  koneplus_tcu_dcu_dialog.c
)

TARGET_LINK_LIBRARIES(libroccatkonepluswidget
  libroccat
  libroccatwidget
  libroccatkoneplus
  ${GTK_LIBRARIES}
  ${GAMINGGEAR0_LIBRARY}
  ${GAMINGGEAR0_WIDGET_LIBRARY}
)

SET_TARGET_PROPERTIES(libroccatkonepluswidget
  PROPERTIES
    VERSION ${V_MAJOR}.${V_MINOR}.${V_PATCH}
    SOVERSION ${V_MAJOR}
    OUTPUT_NAME roccatkonepluswidget
    COMPILE_FLAGS "-DG_LOG_DOMAIN=\\\"KoneplusWidget\\\""
)

INSTALL(TARGETS libroccatkonepluswidget DESTINATION ${LIBDIR})
