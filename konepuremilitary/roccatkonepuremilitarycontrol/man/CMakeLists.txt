FOREACH(language ${TRANSLATION_LANGUAGES})
  INSTALL(FILES roccatkonepuremilitarycontrol.${language}.1
    DESTINATION share/man/${language}/man1
    RENAME roccatkonepuremilitarycontrol.1
  )
ENDFOREACH()