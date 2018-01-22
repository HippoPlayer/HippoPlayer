@echo off

cd t2-output

IF NOT "%BUILD_FILE_ID%" == "" GOTO BuildIdSet

SET BUILD_FILE_ID=%DATE:~10,4%%DATE:~7,2%%DATE:~4,2%_%TIME:~0,2%%TIME:~3,2%

:BuildIdSet:

rmdir /s /q HippoPlayer
rmdir /s /q build
mkdir build
mkdir HippoPlayer
mkdir HippoPlayer\platforms

copy %QT5%\bin\Qt5Core.dll HippoPlayer
copy %QT5%\bin\Qt5Widgets.dll HippoPlayer
copy %QT5%\bin\Qt5Gui.dll HippoPlayer
copy %QT5%\plugins\platforms\qwindows.dll HippoPlayer\platforms
copy win64-msvc-release-default\hippo_player.exe HippoPlayer
copy win64-msvc-release-default\*.dll HippoPlayer

7z a ..\hippoplayer_win_%BUILD_FILE_ID%.7z HippoPlayer

cd ..
