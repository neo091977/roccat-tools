ADD_LIBRARY(libroccatkonextdwidget SHARED
  konextd_color_frame.c
  konextd_cpi_selector.c
)

TARGET_LINK_LIBRARIES(libroccatkonextdwidget
  libroccatkoneplus
  libroccatkonextd
  libroccatwidget
  ${GTK_LIBRARIES}
)

SET_TARGET_PROPERTIES(libroccatkonextdwidget
  PROPERTIES
    VERSION ${V_MAJOR}.${V_MINOR}.${V_PATCH}
    SOVERSION ${V_MAJOR}
    OUTPUT_NAME roccatkonextdwidget
    COMPILE_FLAGS "-DG_LOG_DOMAIN=\\\"KonextdWidget\\\""
)

INSTALL(TARGETS libroccatkonextdwidget DESTINATION ${LIBDIR})
