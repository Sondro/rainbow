#!/bin/bash
BUILD_DIR=$(pwd)
COPYRIGHT="# Copyright $(date +%Y) Bifrost Entertainment. All rights reserved."
HEADER="# This file was generated by $(basename $0)"
NDK_HOME="/opt/android/ndk"
PACKAGE="com/bifrostentertainment/rainbow"
PROJECT=$(cd -P "$(dirname $0)/.." && pwd)
TARGET=rainbow

# Auto-generate files
cd $PROJECT
tools/shaders-gen.py
cd $BUILD_DIR

# Clean the folder
rm -fr AndroidManifest.xml ant.properties bin build.xml gen jni libs \
	local.properties obj proguard-project.txt project.properties res src

# Create project files
mkdir jni
$NDK_HOME/../sdk/tools/android -s create project --name "Rainbow" \
	--target "android-17" --path jni \
	--package "com.bifrostentertainment.rainbow" --activity "Rainbow" || exit 1
rm -r jni/src/*
mv jni/* .

# Create package and link to RainbowActivity
mkdir -p src/$PACKAGE
ln -s $PROJECT/src/Platform/RainbowActivity.java src/$PACKAGE/

echo -n "Generating jni/Android.mk..."

# Gather Rainbow source files
cd $PROJECT
SRC_FILES=$(find src -name '*.cpp' | xargs)

# Include libraries
for lib in Box2D Lua libpng; do
	SRC_FILES="$SRC_FILES \\"$'\n'$(find lib/$lib -name '*.c' -and ! -name 'lua.c' -and ! -name 'luac.c' -and ! -name 'pngwutil.c' -or -name '*.cpp' | xargs)
done

# Manually include FreeType source
SRC_FILES="$SRC_FILES lib/FreeType/src/freetype.c"

cd $BUILD_DIR

cat > jni/Android.mk << ANDROID_MK
$COPYRIGHT
$HEADER

include \$(CLEAR_VARS)

LOCAL_PATH := $PROJECT
LOCAL_MODULE := $TARGET
LOCAL_SRC_FILES := $SRC_FILES

LOCAL_C_INCLUDES := $PROJECT/src $PROJECT/lib $PROJECT/lib/FreeType/include \
                    $PROJECT/lib/libpng $PROJECT/lib/Lua \
                    $NDK_HOME/sources/android/native_app_glue
LOCAL_CFLAGS := $@ -finline-functions
LOCAL_CPPFLAGS := -std=c++11 -Wall -Wextra -Woverloaded-virtual -Wsign-promo -fno-rtti -fno-exceptions

LOCAL_STATIC_LIBRARIES := android_native_app_glue
LOCAL_LDLIBS := -landroid -lEGL -lGLESv2 -lOpenSLES -llog -lz

include \$(BUILD_SHARED_LIBRARY)
\$(call import-module,android/native_app_glue)
ANDROID_MK
echo " done"

echo -n "Generating jni/Application.mk..."
cat > jni/Application.mk << APPLICATION_MK
$COPYRIGHT
$HEADER

APP_ABI := armeabi-v7a  # all
APP_PLATFORM := android-9
APP_STL := gnustl_shared  # Required by Box2D
APPLICATION_MK
echo " done"

echo -n "Generating res/values/themes.xml..."
cat > res/values/themes.xml << THEMES_XML
<?xml version="1.0" encoding="utf-8"?>
<resources>
	<style name="Rainbow.Theme.Default" parent="@android:style/Theme.NoTitleBar.Fullscreen"></style>
</resources>
THEMES_XML
echo " done"

echo -n "Generating res/values-v11/themes.xml..."
mkdir -p res/values-v11
cat > res/values-v11/themes.xml << THEMES_XML
<?xml version="1.0" encoding="utf-8"?>
<resources>
	<style name="Rainbow.Theme.Default" parent="@android:style/Theme.Holo.NoActionBar.Fullscreen"></style>
</resources>
THEMES_XML
echo " done"

echo -n "Generating AndroidManifest.xml..."
cat > AndroidManifest.xml << ANDROIDMANIFEST_XML
<?xml version="1.0" encoding="utf-8"?>
<manifest xmlns:android="http://schemas.android.com/apk/res/android"
          package="com.bifrostentertainment.rainbow"
          android:versionCode="1"
          android:versionName="1.0">
	<uses-permission android:name="android.permission.READ_EXTERNAL_STORAGE" />
	<uses-permission android:name="android.permission.RECORD_AUDIO" />
	<uses-permission android:name="android.permission.WRITE_EXTERNAL_STORAGE" />
	<uses-sdk android:minSdkVersion="9" android:targetSdkVersion="14" />
	<uses-feature android:name="android.hardware.screen.portrait"
	              android:glEsVersion="0x00020000" />
	<application android:icon="@drawable/ic_launcher"
	             android:label="@string/app_name"
	             android:theme="@style/Rainbow.Theme.Default">
		<activity android:configChanges="orientation|screenSize"
		          android:label="@string/app_name"
		          android:launchMode="singleTop"
		          android:name=".RainbowActivity"
		          android:screenOrientation="sensorLandscape">
			<intent-filter>
				<action android:name="android.intent.action.MAIN" />
				<category android:name="android.intent.category.LAUNCHER" />
			</intent-filter>
			<meta-data android:name="android.app.lib_name" android:value="rainbow" />
		</activity>
	</application>
</manifest>
ANDROIDMANIFEST_XML
echo " done"

NDK_DEBUG=${NDK_DEBUG:-1} NDK_TOOLCHAIN_VERSION=${NDK_TOOLCHAIN_VERSION:-clang} $NDK_HOME/ndk-build -j &&
ant debug
