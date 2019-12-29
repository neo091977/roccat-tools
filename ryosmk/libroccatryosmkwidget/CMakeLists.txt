ADD_LIBRARY(libroccatryosmkwidget SHARED
  ryos_capslock_selector.c
  ryos_configuration_dialog.c
  ryos_fn_selector.c
  ryos_gui_key_relations.c
  ryos_key_combo_box.c
  ryos_key_mask_selector.c
  ryos_layer_mode_selector.c
  ryos_layer_selector.c
  ryos_layout_selector.c
  ryos_notification_frame.c
  ryos_standard_selector.c
)

TARGET_LINK_LIBRARIES(libroccatryosmkwidget
  libroccat
  libroccatwidget
  libroccatryosmk
  ${GTK_LIBRARIES}
  ${GAMINGGEAR0_LIBRARY}
  ${GAMINGGEAR0_WIDGET_LIBRARY}
)

SET_TARGET_PROPERTIES(libroccatryosmkwidget
  PROPERTIES
    VERSION ${V_MAJOR}.${V_MINOR}.${V_PATCH}
    SOVERSION ${V_MAJOR}
    OUTPUT_NAME roccatryosmkwidget
    COMPILE_FLAGS "-DG_LOG_DOMAIN=\\\"RyosMKWidget\\\""
)

INSTALL(TARGETS libroccatryosmkwidget DESTINATION ${LIBDIR})
