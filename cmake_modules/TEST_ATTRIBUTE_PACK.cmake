# Stefan Achatz 2011
#
# Tries to find out if variable attribute pack works as expected
# following variables are set:
#
# HAVE_WORKING_ATTRIBUTE_PACK

TRY_RUN(RUN_RESULT_VAR COMPILE_RESULT_VAR
  "${CMAKE_BINARY_DIR}"
  "${CMAKE_SOURCE_DIR}/cmake_modules/test_attribute_pack.c"
)

IF(COMPILE_RESULT_VAR AND RUN_RESULT_VAR EQUAL 0)

  SET(HAVE_WORKING_ATTRIBUTE_PACK "YES")
  MESSAGE(STATUS "Compiler supports attribute pack")

ELSE(COMPILE_RESULT_VAR AND RUN_RESULT_VAR EQUAL 0)

  SET(HAVE_WORKING_ATTRIBUTE_PACK "NO")
  MESSAGE(STATUS "Compiler does not support attribute pack")

ENDIF(COMPILE_RESULT_VAR AND RUN_RESULT_VAR EQUAL 0)

MARK_AS_ADVANCED(HAVE_WORKING_ATTRIBUTE_PACK)