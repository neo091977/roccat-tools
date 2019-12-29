FOREACH(language ${TRANSLATION_LANGUAGES})
  INSTALL(FILES roccatsavucontrol.${language}.1
    DESTINATION share/man/${language}/man1
    RENAME roccatsavucontrol.1
  )
ENDFOREACH()