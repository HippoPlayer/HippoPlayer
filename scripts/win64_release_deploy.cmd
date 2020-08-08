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

robocopy %Qt5_DIR%\bin HippoPlayer Qt5Core.dll Qt5Widgets.dll Qt5Gui.dll
robocopy %Qt5_DIR%\plugins\platforms HippoPlayer\platforms qwindows.dll

robocopy win64-msvc-release-default HippoPlayer hippo_player.exe
copy win64-msvc-release-default\*.dll HippoPlayer

..\bin\windows\7za a ..\hippoplayer_windows.7z HippoPlayer

cd ..
