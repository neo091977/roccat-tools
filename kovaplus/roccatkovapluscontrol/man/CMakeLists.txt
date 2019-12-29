FOREACH(language ${TRANSLATION_LANGUAGES})
  INSTALL(FILES roccatkovapluscontrol.${language}.1
    DESTINATION share/man/${language}/man1
    RENAME roccatkovapluscontrol.1
  )
ENDFOREACH()