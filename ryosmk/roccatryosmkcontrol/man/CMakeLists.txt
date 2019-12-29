FOREACH(language ${TRANSLATION_LANGUAGES})
  INSTALL(FILES roccatryosmkcontrol.${language}.1
    DESTINATION share/man/${language}/man1
    RENAME roccatryosmkcontrol.1
  )
ENDFOREACH()