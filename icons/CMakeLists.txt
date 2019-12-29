FOREACH(size 16 24 32 48 64 72 96 128)
  INSTALL(FILES roccat_${size}x${size}x32.png
    DESTINATION share/icons/hicolor/${size}x${size}/apps
    RENAME roccat.png
  )
ENDFOREACH()