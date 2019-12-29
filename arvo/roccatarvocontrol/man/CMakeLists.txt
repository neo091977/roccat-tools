FOREACH(language ${TRANSLATION_LANGUAGES})
  INSTALL(FILES roccatarvocontrol.${language}.1
    DESTINATION share/man/${language}/man1
    RENAME roccatarvocontrol.1
  )
ENDFOREACH()