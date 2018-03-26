#/bin/bash

# Remove and copy the inital setup of HippoPlayer

echo "Clearing old build"

if [ -z "$BUILD_FILE_ID" ]; then
  export BUILD_FILE_ID=$(date +%Y%m%d_%H%M)
fi

rm -rf t2-output/HippoPlayer.app
cp -rf t2-output/macosx-clang-release-default/HippoPlayer.app t2-output/HippoPlayer.app
cp -vr t2-output/macosx-clang-release-default/*.dylib t2-output/HippoPlayer.app/Contents/MacOS

mkdir t2-output/HippoPlayer.app/Contents/MacOS/bin
cp -vr bin/player t2-output/HippoPlayer.app/Contents/MacOS/bin
cp -vr bin/plugins t2-output/HippoPlayer.app/Contents/MacOS/bin

cd t2-output
rm -rfv build
mkdir build

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

# Package the build to a dmg image

hdiutil create -srcfolder "HippoPlayer.app" -volname "HippoPlayer" -fs HFS+ -fsargs "-c c=64,a=16,e=16" -format UDRW -size 64000k pack.temp.dmg
device=$(hdiutil attach -readwrite -noverify -noautoopen "pack.temp.dmg" | egrep '^/dev/' | sed 1q | awk '{print $1}')
chmod -Rf go-w /Volumes/HippoPlayer
sync
sync
hdiutil detach ${device}
hdiutil convert "pack.temp.dmg" -format UDZO -imagekey zlib-level=9 -o "build/hippoplayer_mac_${BUILD_FILE_ID}.dmg"
rm -f pack.temp.dmg

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

