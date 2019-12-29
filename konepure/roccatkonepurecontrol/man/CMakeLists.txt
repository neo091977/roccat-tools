FOREACH(language ${TRANSLATION_LANGUAGES})
  INSTALL(FILES roccatkonepurecontrol.${language}.1
    DESTINATION share/man/${language}/man1
    RENAME roccatkonepurecontrol.1
  )
ENDFOREACH()