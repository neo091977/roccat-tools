FOREACH(language ${TRANSLATION_LANGUAGES})
  INSTALL(FILES roccatskeltrcontrol.${language}.1
    DESTINATION share/man/${language}/man1
    RENAME roccatskeltrcontrol.1
  )
ENDFOREACH()