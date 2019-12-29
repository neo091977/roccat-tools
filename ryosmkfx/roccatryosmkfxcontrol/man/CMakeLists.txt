FOREACH(language ${TRANSLATION_LANGUAGES})
  INSTALL(FILES roccatryosmkfxcontrol.${language}.1
    DESTINATION share/man/${language}/man1
    RENAME roccatryosmkfxcontrol.1
  )
ENDFOREACH()