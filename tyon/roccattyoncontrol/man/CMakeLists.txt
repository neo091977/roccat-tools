FOREACH(language ${TRANSLATION_LANGUAGES})
  INSTALL(FILES roccattyoncontrol.${language}.1
    DESTINATION share/man/${language}/man1
    RENAME roccattyoncontrol.1
  )
ENDFOREACH()