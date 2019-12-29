FOREACH(language ${TRANSLATION_LANGUAGES})
  INSTALL(FILES roccatkova2016control.${language}.1
    DESTINATION share/man/${language}/man1
    RENAME roccatkova2016control.1
  )
ENDFOREACH()