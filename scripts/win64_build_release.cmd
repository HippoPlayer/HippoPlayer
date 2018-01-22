@echo off

IF NOT "%BUILD_DATE_ID%" == "" GOTO DateSet

set BUILD_DATE_ID=%DATE% %TIME%
SET BUILD_FILE_ID=%DATE:~10,4%%DATE:~7,2%%DATE:~4,2%_%TIME:~0,2%%TIME:~3,2%

:DateSet

bin\windows\tundra2 win64-msvc-release
