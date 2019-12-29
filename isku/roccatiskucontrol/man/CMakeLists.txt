FOREACH(language ${TRANSLATION_LANGUAGES})
  INSTALL(FILES roccatiskucontrol.${language}.1
    DESTINATION share/man/${language}/man1
    RENAME roccatiskucontrol.1
  )
ENDFOREACH()