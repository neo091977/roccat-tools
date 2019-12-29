FOREACH(language ${TRANSLATION_LANGUAGES})
  INSTALL(FILES roccatkonepureopticalcontrol.${language}.1
    DESTINATION share/man/${language}/man1
    RENAME roccatkonepureopticalcontrol.1
  )
ENDFOREACH()