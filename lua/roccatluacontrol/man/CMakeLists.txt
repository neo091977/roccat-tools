FOREACH(language ${TRANSLATION_LANGUAGES})
  INSTALL(FILES roccatluacontrol.${language}.1
    DESTINATION share/man/${language}/man1
    RENAME roccatluacontrol.1
  )
ENDFOREACH()