CMAKE_MINIMUM_REQUIRED(VERSION 2.8.12)

PROJECT(roccat-tools C)

FUNCTION(list_contains list values result)
  SET(${result} FALSE PARENT_SCOPE)
  FOREACH(value ${values})
    LIST(FIND ${list} ${value} ${result})
    IF(NOT ${${result}} EQUAL -1)
      SET(${result} TRUE PARENT_SCOPE)
      BREAK()
    ENDIF()
  ENDFOREACH()
ENDFUNCTION()

IF(NOT CMAKE_BUILD_TYPE)
  SET(CMAKE_BUILD_TYPE "Release"
    CACHE STRING "Choose the type of build, options are: None(CMAKE_CXX_FLAGS or CMAKE_C_FLAGS used) Debug Release RelWithDebInfo MinSizeRel."
    FORCE
  )
ENDIF()

IF(CMAKE_COMPILER_IS_GNUCC)
  # * Force gcc to generate single line error messages (used for eclipse cdt)
  # * Play it save with lots of warning messages
  # * Define GNU source for some extras
  SET(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -fmessage-length=0 -Wall -Wextra -Wno-unused-parameter -Wno-missing-field-initializers -Wno-type-limits -D_GNU_SOURCE")
  SET(CMAKE_SHARED_LINKER_FLAGS "${CMAKE_SHARED_LINKER_FLAGS} -Wl,--no-undefined")
  SET(CMAKE_SHARED_LINKER_FLAGS_RELEASE "${CMAKE_SHARED_LINKER_FLAGS_RELEASE} -Wl,--as-needed")
ENDIF()
SET(CMAKE_C_FLAGS_DEBUG "${CMAKE_C_FLAGS_DEBUG} -DGTK_DISABLE_SINGLE_INCLUDES -DGDK_DISABLE_DEPRECATED -DGTK_DISABLE_DEPRECATED -DGSEAL_ENABLE -DG_ERRORCHECK_MUTEXES -DG_ENABLE_DIAGNOSTIC")
SET(CMAKE_C_FLAGS_RELEASE "${CMAKE_C_FLAGS_RELEASE} -DG_DISABLE_CAST_CHECKS -DG_DISABLE_ASSERT")

SET(TRANSLATION_LANGUAGES de en)

SET(ALL_DEVICES
  arvo
  isku iskufx
  kiro
  koneplus
  konepure konepuremilitary konepureoptical
  konextd konextdoptical
  kovaplus kova2016
  lua nyth pyra
  ryosmk ryosmkfx ryostkl
  savu skeltr sova suora tyon
)

IF(${CMAKE_SYSTEM_NAME} STREQUAL "Linux")
  LIST(APPEND ALL_DEVICES kone)
ENDIF()

IF(NOT DEVICES)
  SET(DEVICES ${ALL_DEVICES})
ENDIF()
SET(DEVICES ${DEVICES} CACHE STRING "Create for given devices. Subset of ${ALL_DEVICES}")
MESSAGE(STATUS "compiling for devices ${DEVICES}")

LIST(APPEND CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/cmake_modules")

INCLUDE(X86)
INCLUDE(UDEV)

IF(NOT EVENTHANDLER_PLUGIN_DIR)
  SET(EVENTHANDLER_PLUGIN_DIR ${LIBDIR}/roccat)
ENDIF()
MESSAGE(STATUS "Installing eventhandler plugins in ${EVENTHANDLER_PLUGIN_DIR}")

INCLUDE(TEST_ATTRIBUTE_PACK)
IF(NOT HAVE_WORKING_ATTRIBUTE_PACK)
  MESSAGE(FATAL_ERROR "Compiler does not support attribute pack")
ENDIF()

FIND_PACKAGE(GTHREAD2)
FIND_PACKAGE(GTK REQUIRED)
FIND_PACKAGE(DBUS REQUIRED)
FIND_PACKAGE(DBUS-GLIB REQUIRED)
FIND_PACKAGE(GMODULE2 REQUIRED)
FIND_PACKAGE(GUDEV REQUIRED)
FIND_PACKAGE(X11 REQUIRED)
FIND_PACKAGE(GAMINGGEAR0 0.15.1 REQUIRED)
FIND_PACKAGE(M REQUIRED)

list_contains(DEVICES "ryosmk;ryostkl;ryosmkfx" contains)
IF(contains)
  FIND_PACKAGE(LUA ${WITH_LUA} EXACT REQUIRED)
ENDIF()

CONFIGURE_FILE(
  "${CMAKE_CURRENT_SOURCE_DIR}/cmake_uninstall.cmake.in"
  "${CMAKE_CURRENT_BINARY_DIR}/cmake_uninstall.cmake"
  @ONLY
)

ADD_CUSTOM_TARGET(uninstall
  "${CMAKE_COMMAND}" -P "${CMAKE_CURRENT_BINARY_DIR}/cmake_uninstall.cmake"
)

SET(V_MAJOR 5)
SET(V_MINOR 9)
SET(V_PATCH 0)

ENABLE_TESTING()

INCLUDE_DIRECTORIES(
  ${CMAKE_SOURCE_DIR}/include
  ${CMAKE_SOURCE_DIR}/libroccat
  ${CMAKE_SOURCE_DIR}/libroccathelper
  ${CMAKE_SOURCE_DIR}/libroccatwidget
  ${CMAKE_BINARY_DIR}/include
  ${CMAKE_BINARY_DIR}/libroccathelper
  ${DBUS_INCLUDE_DIRS}
  ${DBUS-GLIB_INCLUDE_DIRS}
  ${GTK_INCLUDE_DIRS}
  ${GUDEV_INCLUDE_DIRS}
  ${GAMINGGEAR0_INCLUDE_DIR}
  ${M_INCLUDE_DIR}
)

MACRO(add_dbus_bindings name)
  ADD_CUSTOM_COMMAND(
    OUTPUT
      ${CMAKE_CURRENT_BINARY_DIR}/${name}_dbus_server_glue.h
      ${CMAKE_CURRENT_BINARY_DIR}/${name}_dbus_plain_services.h
    COMMAND dbus-binding-tool --mode=glib-server --prefix=server ${CMAKE_CURRENT_SOURCE_DIR}/${name}_dbus_services.xml --output=${CMAKE_CURRENT_BINARY_DIR}/${name}_dbus_server_glue.h
    COMMAND dbus-binding-tool --mode=glib-client ${CMAKE_CURRENT_SOURCE_DIR}/${name}_dbus_services.xml --output=${CMAKE_CURRENT_BINARY_DIR}/${name}_dbus_plain_services.h
    DEPENDS
      ${CMAKE_CURRENT_SOURCE_DIR}/${name}_dbus_services.xml
  )

  ADD_CUSTOM_TARGET(${name}_dbus_bindings ALL
    DEPENDS
      ${CMAKE_CURRENT_BINARY_DIR}/${name}_dbus_server_glue.h
      ${CMAKE_CURRENT_BINARY_DIR}/${name}_dbus_plain_services.h
  )
ENDMACRO()

ADD_SUBDIRECTORY(include)
ADD_SUBDIRECTORY(icons)
ADD_SUBDIRECTORY(sounds)
ADD_SUBDIRECTORY(libroccat)
ADD_SUBDIRECTORY(libroccatwidget)
ADD_SUBDIRECTORY(udev)
ADD_SUBDIRECTORY(libroccathelper)
ADD_SUBDIRECTORY(roccateventhandler)
ADD_SUBDIRECTORY(po)

FOREACH(device ${ALL_DEVICES})
  ADD_SUBDIRECTORY(${device})
ENDFOREACH()
