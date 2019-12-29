FOREACH(language ${TRANSLATION_LANGUAGES})
  INSTALL(FILES roccatryostklcontrol.${language}.1
    DESTINATION share/man/${language}/man1
    RENAME roccatryostklcontrol.1
  )
ENDFOREACH()