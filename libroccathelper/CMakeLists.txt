ADD_CUSTOM_COMMAND(
  OUTPUT
    ${CMAKE_CURRENT_BINARY_DIR}/g_cclosure_roccat_marshaller.h
    ${CMAKE_CURRENT_BINARY_DIR}/g_cclosure_roccat_marshaller.c
  COMMAND glib-genmarshal --header --prefix=g_cclosure_roccat_marshal ${CMAKE_CURRENT_SOURCE_DIR}/g_roccat_marshaller >${CMAKE_CURRENT_BINARY_DIR}/g_cclosure_roccat_marshaller.h
  COMMAND glib-genmarshal --body --prefix=g_cclosure_roccat_marshal ${CMAKE_CURRENT_SOURCE_DIR}/g_roccat_marshaller >${CMAKE_CURRENT_BINARY_DIR}/g_cclosure_roccat_marshaller.c
  DEPENDS
    ${CMAKE_CURRENT_SOURCE_DIR}/g_roccat_marshaller
)

ADD_LIBRARY(libroccathelper STATIC
  dbus_roccat_helper.c
  g_cclosure_roccat_marshaller.c
  g_dbus_roccat_helper.c
  g_roccat_helper.c
  g_udev_roccat_helper.c
  gtk_roccat_helper.c
)

SET_TARGET_PROPERTIES(libroccathelper
  PROPERTIES
    OUTPUT_NAME roccathelper
    COMPILE_FLAGS "-ffunction-sections -fPIC"
)
