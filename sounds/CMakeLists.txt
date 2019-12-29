# Soundfile conversions:
# oggenc --resample 44100 -q5 *.wav
# mmv "*.ogg" "#1.oga"

INSTALL(DIRECTORY cpi live_recording profile sensitivity timer typing
  DESTINATION share/roccat/sounds
)

INSTALL(FILES sleep.oga wakeup.oga
  DESTINATION share/roccat/sounds
)
