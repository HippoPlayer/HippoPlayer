#/bin/bash

# Remove and copy the inital setup of HippoPlayer

echo "Clearing old build"

rm -rf t2-output/HippoPlayer.app
cp -rf t2-output/macosx-clang-release-default/HippoPlayer.app t2-output/HippoPlayer.app
cp -vr t2-output/macosx-clang-release-default/libwrui_qt.dylib t2-output/HippoPlayer.app/Contents/MacOS

cd t2-output

"$QT5/bin/macdeployqt" HippoPlayer.app -executable HippoPlayer.app/Contents/MacOS/libwrui_qt.dylib

prefix="`grealpath $QT5`";
frameworks=(HippoPlayer.app/Contents/Frameworks/Qt*.framework) &&
frameworks=("${frameworks[@]#HippoPlayer.app/Contents/Frameworks/}") &&
frameworks=("${frameworks[@]%.framework}") &&
for framework in "${frameworks[@]}"; do
	for target in "${frameworks[@]}"; do
		install_name_tool -change @rpath/$target.framework/Versions/5/$target @executable_path/../Frameworks/$target.framework/Versions/5/$target HippoPlayer.app/Contents/Frameworks/$framework.framework/$framework;
	done;
done;
for plugin in HippoPlayer.app/Contents/PlugIns/*/*.dylib; do
	for target in "${frameworks[@]}"; do
		install_name_tool -change @rpath/$target.framework/Versions/5/$target @executable_path/../Frameworks/$target.framework/Versions/5/$target $plugin;
	done;
done;

cd ..

# Copy the QtFrameWorks

# echo "Copy Qt frameworks"

# mkdir t2-output/HippoPlayer.app/Contents/Frameworks
# cp -r $QT5/lib/QtCore.framework t2-output/HippoPlayer.app/Contents/Frameworks/QtCore.framework
# cp -r $QT5/lib/QtGui.framework t2-output/HippoPlayer.app/Contents/Frameworks/QtGui.framework
# cp -r $QT5/lib/QtWidgets.framework t2-output/HippoPlayer.app/Contents/Frameworks/QtWidgets.framework

# echo "Copy Plugins"

# mkdir t2-output/HippoPlayer.app/Contents/Plugins
# cp -vr t2-output/macosx-clang-release-default/*.dylib t2-output/HippoPlayer.app/Contents/Plugins
# mv t2-output/HippoPlayer.app/Contents/Plugins/libwrui_qt.dylib t2-output/HippoPlayer.app/Contents/MacOS

# echo "Copy Qt plugins"

# mkdir t2-output/HippoPlayer.app/Contents/MacOS/Platforms/
# cp -r $QT5/plugins/platforms/libqcocoa.dylib t2-output/HippoPlayer.app/Contents/MacOS/Platforms

# echo "Redirecting bindings"

# install_name_tool -change $QT5/lib/QtCore.framework/Versions/5/QtCore @executable_path/../Frameworks/QtCore.framework/Versions/5/QtCore t2-output/HippoPlayer.app/Contents/MacOS/libwrui_qt.dylib
# install_name_tool -change $QT5/lib/QtWidgets.framework/Versions/5/QtWidgets @executable_path/../Frameworks/QtCore.framework/Versions/5/QtWidgets t2-output/HippoPlayer.app/Contents/MacOS/libwrui_qt.dylib
# install_name_tool -change $QT5/lib/QtGui.framework/Versions/5/QtGui @executable_path/../Frameworks/QtGui.framework/Versions/5/QtGui t2-output/HippoPlayer.app/Contents/MacOS/libwrui_qt.dylib

# install_name_tool -change $QT5/lib/QtCore.framework/Versions/5/QtCore @executable_path/../Frameworks/QtCore.framework/Versions/5/QtCore t2-output/HippoPlayer.app/Contents/MacOS/Platforms/libqcocoa.dylib
# install_name_tool -change $QT5/lib/QtWidgets.framework/Versions/5/QtWidgets @executable_path/../Frameworks/QtCore.framework/Versions/5/QtWidgets t2-output/HippoPlayer.app/Contents/MacOS/Platforms/libqcocoa.dylib
# install_name_tool -change $QT5/lib/QtGui.framework/Versions/5/QtGui @executable_path/../Frameworks/QtGui.framework/Versions/5/QtGui t2-output/HippoPlayer.app/Contents/MacOS/Platforms/libqcocoa.dylib

