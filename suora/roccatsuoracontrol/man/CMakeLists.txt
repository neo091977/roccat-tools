FOREACH(language ${TRANSLATION_LANGUAGES})
  INSTALL(FILES roccatsuoracontrol.${language}.1
    DESTINATION share/man/${language}/man1
    RENAME roccatsuoracontrol.1
  )
ENDFOREACH()