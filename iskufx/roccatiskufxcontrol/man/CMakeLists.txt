FOREACH(language ${TRANSLATION_LANGUAGES})
  INSTALL(FILES roccatiskufxcontrol.${language}.1
    DESTINATION share/man/${language}/man1
    RENAME roccatiskufxcontrol.1
  )
ENDFOREACH()