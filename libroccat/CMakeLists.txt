INCLUDE_DIRECTORIES(
  ${CMAKE_SOURCE_DIR}/arvo/libroccatarvo
  ${CMAKE_SOURCE_DIR}/isku/libroccatisku
  ${CMAKE_SOURCE_DIR}/iskufx/libroccatiskufx
  ${CMAKE_SOURCE_DIR}/kiro/libroccatkiro
  ${CMAKE_SOURCE_DIR}/kone/libroccatkone
  ${CMAKE_SOURCE_DIR}/koneplus/libroccatkoneplus
  ${CMAKE_SOURCE_DIR}/konepure/libroccatkonepure
  ${CMAKE_SOURCE_DIR}/konepuremilitary/libroccatkonepuremilitary
  ${CMAKE_SOURCE_DIR}/konepureoptical/libroccatkonepureoptical
  ${CMAKE_SOURCE_DIR}/konextd/libroccatkonextd
  ${CMAKE_SOURCE_DIR}/konextdoptical/libroccatkonextdoptical
  ${CMAKE_SOURCE_DIR}/kova2016/libroccatkova2016
  ${CMAKE_SOURCE_DIR}/kovaplus/libroccatkovaplus
  ${CMAKE_SOURCE_DIR}/lua/libroccatlua
  ${CMAKE_SOURCE_DIR}/nyth/libroccatnyth
  ${CMAKE_SOURCE_DIR}/pyra/libroccatpyra
  ${CMAKE_SOURCE_DIR}/ryosmk/libroccatryosmk
  ${CMAKE_SOURCE_DIR}/ryosmkfx/libroccatryosmkfx
  ${CMAKE_SOURCE_DIR}/ryostkl/libroccatryostkl
  ${CMAKE_SOURCE_DIR}/savu/libroccatsavu
  ${CMAKE_SOURCE_DIR}/skeltr/libroccatskeltr
  ${CMAKE_SOURCE_DIR}/sova/libroccatsova
  ${CMAKE_SOURCE_DIR}/suora/libroccatsuora
  ${CMAKE_SOURCE_DIR}/tyon/libroccattyon
)

SET(SOURCES
  roccat_button.c
  roccat_config.c
  roccat_control.c
  roccat_device.c
  roccat_device_scanner_interface.c
  roccat_eventhandler_host.c
  roccat_eventhandler_plugin.c
  roccat_firmware.c
  roccat_gfx.c
  roccat_helper.c
  roccat_key_file.c
  roccat_keystroke.c
  roccat_light.c
  roccat_notification_cpi.c
  roccat_notification_live_recording.c
  roccat_notification_profile.c
  roccat_notification_sensitivity.c
  roccat_notification_timer.c
  roccat_notificator.c
  roccat_orig_timer.c
  roccat_process_helper.c
  roccat_secure.c
  roccat_srom.c
  roccat_strings.c
  roccat_swarm_rmp.c
  roccat_talk.c
  roccat_talkfx.c
  roccat_timer.c
  roccat_timers.c
)

SET(LIBRARIES
  libroccathelper
  ${DBUS_LIBRARIES}
  ${GAMINGGEAR0_LIBRARY}
  ${GLIB2_LIBRARIES}
)

IF(${CMAKE_SYSTEM_NAME} STREQUAL "Linux")
  LIST(APPEND SOURCES
    roccat_device_hidraw.c
    roccat_device_sysfs.c
    roccat_device_scanner_udev.c
  )
  LIST(APPEND LIBRARIES ${GUDEV_LIBRARIES})
ENDIF()

ADD_LIBRARY(libroccat SHARED ${SOURCES})

TARGET_LINK_LIBRARIES(libroccat ${LIBRARIES})

SET_TARGET_PROPERTIES(libroccat
  PROPERTIES
    VERSION ${V_MAJOR}.${V_MINOR}.${V_PATCH}
    SOVERSION ${V_MAJOR}
    OUTPUT_NAME roccat
    COMPILE_FLAGS "-DG_LOG_DOMAIN=\\\"Roccat\\\""
)

INSTALL(TARGETS libroccat DESTINATION ${LIBDIR})
