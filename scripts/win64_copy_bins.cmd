@echo off
copy %QT5_BIN%\Qt5Core.dll t2-output\win64-msvc-debug-default\Qt5Core.dll /y
copy %QT5_BIN%\Qt5Gui.dll t2-output\win64-msvc-debug-default\Qt5Gui.dll /y
copy %QT5_BIN%\Qt5Widgets.dll t2-output\win64-msvc-debug-default\Qt5Widgets.dll /y
robocopy %QT5_BIN%\..\plugins t2-output\win64-msvc-debug-default\plugins /s /e

copy %QT5_BIN%\Qt5Core.dll t2-output\win64-msvc-release-default\Qt5Core.dll /y
copy %QT5_BIN%\Qt5Gui.dll t2-output\win64-msvc-release-default\Qt5Gui.dll /y
copy %QT5_BIN%\Qt5Widgets.dll t2-output\win64-msvc-release-default\Qt5Widgets.dll /y
robocopy %QT5_BIN%\..\plugins t2-output\win64-msvc-release-default\plugins /s /e


