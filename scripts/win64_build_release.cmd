@echo off

IF NOT "%BUILD_DATE_ID%" == "" GOTO DateSet

set BUILD_DATE_ID=%DATE% %TIME%

:DateSet

bin\windows\tundra2 win64-msvc-release
