FOREACH(language ${TRANSLATION_LANGUAGES})
  INSTALL(FILES roccatkonecontrol.${language}.1
    DESTINATION share/man/${language}/man1
    RENAME roccatkonecontrol.1
  )
ENDFOREACH()