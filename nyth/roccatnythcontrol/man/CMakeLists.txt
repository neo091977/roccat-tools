FOREACH(language ${TRANSLATION_LANGUAGES})
  INSTALL(FILES roccatnythcontrol.${language}.1
    DESTINATION share/man/${language}/man1
    RENAME roccatnythcontrol.1
  )
ENDFOREACH()