@echo off

cd t2-output

IF NOT "%HIPPO_VERSION%" == "" GOTO BuildIdSet

SET HIPPO_VERSION=unknown_version

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

copy %QT5_BIN%\Qt5Core.dll HippoPlayer
copy %QT5_BIN%\Qt5Widgets.dll HippoPlayer
copy %QT5_BIN%\Qt5Gui.dll HippoPlayer
copy %QT5_BIN%\..\plugins\platforms\qwindows.dll HippoPlayer\platforms
copy win64-msvc-release-default\hippo_player.exe HippoPlayer
copy win64-msvc-release-default\*.dll HippoPlayer

..\bin\windows\7za a ..\hippoplayer_win_%HIPPO_VERSION%.7z HippoPlayer

cd ..
