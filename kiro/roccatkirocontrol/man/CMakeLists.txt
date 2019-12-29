FOREACH(language ${TRANSLATION_LANGUAGES})
  INSTALL(FILES roccatkirocontrol.${language}.1
    DESTINATION share/man/${language}/man1
    RENAME roccatkirocontrol.1
  )
ENDFOREACH()