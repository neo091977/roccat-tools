INCLUDE_DIRECTORIES(
  ${CMAKE_SOURCE_DIR}/isku/libroccatisku
  ${CMAKE_SOURCE_DIR}/iskufx/libroccatiskufx
  ${CMAKE_SOURCE_DIR}/kiro/libroccatkiro
  ${CMAKE_SOURCE_DIR}/koneplus/libroccatkoneplus
  ${CMAKE_SOURCE_DIR}/konepure/libroccatkonepure
  ${CMAKE_SOURCE_DIR}/konepuremilitary/libroccatkonepuremilitary
  ${CMAKE_SOURCE_DIR}/konepureoptical/libroccatkonepureoptical
  ${CMAKE_SOURCE_DIR}/konextd/libroccatkonextd
  ${CMAKE_SOURCE_DIR}/konextdoptical/libroccatkonextdoptical
  ${CMAKE_SOURCE_DIR}/kova2016/libroccatkova2016
  ${CMAKE_SOURCE_DIR}/ryosmk/libroccatryosmk
  ${CMAKE_SOURCE_DIR}/ryosmkfx/libroccatryosmkfx
  ${CMAKE_SOURCE_DIR}/ryostkl/libroccatryostkl
  ${CMAKE_SOURCE_DIR}/nyth/libroccatnyth
  ${CMAKE_SOURCE_DIR}/skeltr/libroccatskeltr
  ${CMAKE_SOURCE_DIR}/sova/libroccatsova
  ${CMAKE_SOURCE_DIR}/tyon/libroccattyon
)

ADD_LIBRARY(libroccatwidget SHARED
  roccat_about_dialog.c
  roccat_actual_profile_selector.c
  roccat_color_button.c
  roccat_color_palette_selector.c
  roccat_color_selection_button.c
  roccat_color_selection_dialog.c
  roccat_config_window.c
  roccat_config_window_list.c
  roccat_config_window_pages.c
  roccat_continue_dialog.c
  roccat_cpi_fixed_selector.c
  roccat_cpi_selector.c
  roccat_default_profile_combo_box.c
  roccat_file_entry.c
  roccat_gamefile_selector.c
  roccat_handedness_selector.c
  roccat_info_dialog.c
  roccat_key_button.c
  roccat_key_combo_box.c
  roccat_key_dialog.c
  roccat_key_mask_selector.c
  roccat_light_selector.c
  roccat_multiwidget.c
  roccat_notification_type_combo_box.c
  roccat_polling_rate_frame.c
  roccat_polling_rate_selector.c
  roccat_profile_page.c
  roccat_profile_page_tab_label.c
  roccat_save_dialog.c
  roccat_sensitivity_frame.c
  roccat_sensor_register_dialog.c
  roccat_shortcut_dialog.c
  roccat_single_cpi_selector.c
  roccat_swarm_color_selection_button.c
  roccat_talk_target_combo_box.c
  roccat_talk_target_dialog.c
  roccat_talkfx_selector.c
  roccat_timer_combo_box.c
  roccat_timer_dialog.c
  roccat_timer_editor_dialog.c
  roccat_timer_editor_dialog_menu_bar.c
  roccat_timer_list_store.c
  roccat_timer_tree_view.c
  roccat_update_assistant.c
  roccat_volume_scale.c
  roccat_warning_dialog.c
)

TARGET_LINK_LIBRARIES(libroccatwidget
  libroccat
  libroccathelper
  ${GAMINGGEAR0_LIBRARY}
  ${GAMINGGEAR0_WIDGET_LIBRARY}
  ${GTK_LIBRARIES}
  ${M_LIBRARY}
)

SET_TARGET_PROPERTIES(libroccatwidget
  PROPERTIES
    VERSION ${V_MAJOR}.${V_MINOR}.${V_PATCH}
    SOVERSION ${V_MAJOR}
    OUTPUT_NAME roccatwidget
    COMPILE_FLAGS "-DG_LOG_DOMAIN=\\\"RoccatWidget\\\""
)

INSTALL(TARGETS libroccatwidget DESTINATION ${LIBDIR})
