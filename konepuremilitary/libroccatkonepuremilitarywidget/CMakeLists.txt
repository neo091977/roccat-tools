ADD_LIBRARY(libroccatkonepuremilitarywidget SHARED
  konepuremilitary_anglesnap_frame.c
  konepuremilitary_cpi_selector.c
  konepuremilitary_dcu_dialog.c
  konepuremilitary_move_dialog.c
  konepuremilitary_sensor_alignment_frame.c
  konepuremilitary_single_cpi_selector.c
)

INCLUDE_DIRECTORIES(
  ${CMAKE_SOURCE_DIR}/koneplus/libroccatkonepluswidget
)

TARGET_LINK_LIBRARIES(libroccatkonepuremilitarywidget
  libroccatkonepuremilitary
  libroccatkoneplus
  libroccatkonepluswidget
  libroccatwidget
  ${GAMINGGEAR0_WIDGET_LIBRARY}
  ${GTK_LIBRARIES}
)

SET_TARGET_PROPERTIES(libroccatkonepuremilitarywidget
  PROPERTIES
    VERSION ${V_MAJOR}.${V_MINOR}.${V_PATCH}
    SOVERSION ${V_MAJOR}
    OUTPUT_NAME roccatkonepuremilitarywidget
    COMPILE_FLAGS "-DG_LOG_DOMAIN=\\\"KonepuremilitaryWidget\\\""
)

INSTALL(TARGETS libroccatkonepuremilitarywidget DESTINATION ${LIBDIR})
