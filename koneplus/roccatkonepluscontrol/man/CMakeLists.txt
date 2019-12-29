FOREACH(language ${TRANSLATION_LANGUAGES})
  INSTALL(FILES roccatkonepluscontrol.${language}.1
    DESTINATION share/man/${language}/man1
    RENAME roccatkonepluscontrol.1
  )
ENDFOREACH()