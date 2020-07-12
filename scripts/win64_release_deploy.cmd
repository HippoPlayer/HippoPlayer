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

mkdir HippoPlayer\bin
mkdir HippoPlayer\data
mkdir HippoPlayer\bin\player
mkdir HippoPlayer\bin\plugins
mkdir HippoPlayer\bin\player\buttons
copy ..\bin\player\buttons\*.png HippoPlayer\bin\player\buttons
robocopy ..\bin\plugins HippoPlayer\bin\plugins /s /e
robocopy ..\data HippoPlayer\data /s /e

REM should be release here later
copy %QT5_BIN%\Qt5Cored.dll HippoPlayer
copy %QT5_BIN%\Qt5Widgetsd.dll HippoPlayer
copy %QT5_BIN%\Qt5Guid.dll HippoPlayer
copy %QT5_BIN%\..\plugins\platforms\qwindowsd.dll HippoPlayer\platforms
copy win64-msvc-release-default\hippo_player.exe HippoPlayer
copy win64-msvc-release-default\*.dll HippoPlayer

7z a ..\hippoplayer_win_%BUILD_FILE_ID%.7z HippoPlayer

cd ..
