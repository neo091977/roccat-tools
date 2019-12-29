ADD_LIBRARY(libroccatkonepurewidget SHARED
  konepure_buttons_frame.c
  konepure_color_frame.c
  konepure_light_effects_frame.c
)

INCLUDE_DIRECTORIES(
  ${CMAKE_SOURCE_DIR}/koneplus/libroccatkonepluswidget
)

TARGET_LINK_LIBRARIES(libroccatkonepurewidget
  libroccatkonepure
  libroccatkoneplus
  libroccatkonextd
  libroccatkonepluswidget
  libroccatwidget
  ${GTK_LIBRARIES}
  ${GAMINGGEAR0_WIDGET_LIBRARY}
)

SET_TARGET_PROPERTIES(libroccatkonepurewidget
  PROPERTIES
    VERSION ${V_MAJOR}.${V_MINOR}.${V_PATCH}
    SOVERSION ${V_MAJOR}
    OUTPUT_NAME roccatkonepurewidget
    COMPILE_FLAGS "-DG_LOG_DOMAIN=\\\"KonepureWidget\\\""
)

INSTALL(TARGETS libroccatkonepurewidget DESTINATION ${LIBDIR})
