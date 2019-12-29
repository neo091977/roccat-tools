FOREACH(language ${TRANSLATION_LANGUAGES})
  INSTALL(FILES roccatsovacontrol.${language}.1
    DESTINATION share/man/${language}/man1
    RENAME roccatsovacontrol.1
  )
ENDFOREACH()