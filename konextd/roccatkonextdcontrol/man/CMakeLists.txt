FOREACH(language ${TRANSLATION_LANGUAGES})
  INSTALL(FILES roccatkonextdcontrol.${language}.1
    DESTINATION share/man/${language}/man1
    RENAME roccatkonextdcontrol.1
  )
ENDFOREACH()