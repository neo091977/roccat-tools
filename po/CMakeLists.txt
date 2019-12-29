FILE(GLOB_RECURSE _GETTEXT_SOURCES_REL RELATIVE ${CMAKE_SOURCE_DIR} ${CMAKE_SOURCE_DIR}/*.c)

# Resulting files of targets gettext_template and gettext_translation_* are part
# of the source distribution and are to be created/updated explicitly by developers.
# mo files are created automatically as part of the compilation/installation process.

# After a new po file has been created, please change charset to UTF-8.

# Build portable object template
ADD_CUSTOM_TARGET(gettext_template
  # charset in resulting .pot is not set to UTF-8 if sources use only ascii chars.
  xgettext --default-domain=${CMAKE_PROJECT_NAME}
  --output=${CMAKE_CURRENT_SOURCE_DIR}/${CMAKE_PROJECT_NAME}.pot --sort-output
  --keyword --keyword=_ --keyword=N_ --keyword=C_:1c,2 --keyword=NC_:1c,2
  --from-code=UTF-8 --language=C
  --package-name=${CMAKE_PROJECT_NAME} --package-version=${V_MAJOR}.${V_MINOR}.${V_PATCH}
  ${_GETTEXT_SOURCES_REL}
  WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
)

FOREACH(language ${TRANSLATION_LANGUAGES})
  # Build portable object
  ADD_CUSTOM_TARGET(gettext_translation_${language}
    if [ -e ${CMAKE_CURRENT_SOURCE_DIR}/${language}.po ]\; then
      # Not using --update option for msgmerge because timestamp is not updated
      # when nothing changed.
      msgmerge --no-fuzzy-matching ${CMAKE_CURRENT_SOURCE_DIR}/${language}.po ${CMAKE_CURRENT_SOURCE_DIR}/${CMAKE_PROJECT_NAME}.pot --output-file=${CMAKE_CURRENT_SOURCE_DIR}/${language}.po\;
    else
      msginit --locale=${language} --input=${CMAKE_CURRENT_SOURCE_DIR}/${CMAKE_PROJECT_NAME}.pot --output-file=${CMAKE_CURRENT_SOURCE_DIR}/${language}.po\;
    fi
  )

  # Build machine object
  ADD_CUSTOM_COMMAND(
    OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/${language}.mo
    COMMAND msgfmt ${CMAKE_CURRENT_SOURCE_DIR}/${language}.po --output-file=${CMAKE_CURRENT_BINARY_DIR}/${language}.mo
    DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/${language}.po
  )

  ADD_CUSTOM_TARGET(gettext_mo_${language} ALL
    DEPENDS ${CMAKE_CURRENT_BINARY_DIR}/${language}.mo
  )

  INSTALL(FILES ${CMAKE_CURRENT_BINARY_DIR}/${language}.mo
    DESTINATION share/locale/${language}/LC_MESSAGES
    RENAME ${CMAKE_PROJECT_NAME}.mo
  )  
ENDFOREACH()

UNSET(_GETTEXT_SOURCES_REL)
