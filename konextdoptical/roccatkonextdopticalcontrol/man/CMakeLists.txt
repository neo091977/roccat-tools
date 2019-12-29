FOREACH(language ${TRANSLATION_LANGUAGES})
  INSTALL(FILES roccatkonextdopticalcontrol.${language}.1
    DESTINATION share/man/${language}/man1
    RENAME roccatkonextdopticalcontrol.1
  )
ENDFOREACH()