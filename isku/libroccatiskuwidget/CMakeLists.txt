ADD_LIBRARY(libroccatiskuwidget SHARED
  isku_capslock_frame.c
  isku_easyzone_frame.c
  isku_function_frame.c
  isku_key_combo_box.c
  isku_key_mask_selector.c
  isku_macro_frame.c
  isku_media_frame.c
  isku_thumbster_frame.c
)

TARGET_LINK_LIBRARIES(libroccatiskuwidget
  libroccat
  libroccatwidget
  libroccatisku
  ${GTK_LIBRARIES}
  ${GAMINGGEAR0_LIBRARY}
  ${GAMINGGEAR0_WIDGET_LIBRARY}
)

SET_TARGET_PROPERTIES(libroccatiskuwidget
  PROPERTIES
    VERSION ${V_MAJOR}.${V_MINOR}.${V_PATCH}
    SOVERSION ${V_MAJOR}
    OUTPUT_NAME roccatiskuwidget
    COMPILE_FLAGS "-DG_LOG_DOMAIN=\\\"IskuWidget\\\""
)

INSTALL(TARGETS libroccatiskuwidget DESTINATION ${LIBDIR})
