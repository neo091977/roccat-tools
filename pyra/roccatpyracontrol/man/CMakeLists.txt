FOREACH(language ${TRANSLATION_LANGUAGES})
  INSTALL(FILES roccatpyracontrol.${language}.1
    DESTINATION share/man/${language}/man1
    RENAME roccatpyracontrol.1
  )
ENDFOREACH()