#!/bin/bash
# Copyright (c) 2010-16 Bifrost Entertainment AS and Tommy Nguyen
# Distributed under the MIT License.
# (See accompanying file LICENSE or copy at http://opensource.org/licenses/MIT)

NDK_HOME=${NDK_HOME:-"/usr/local/opt/android-ndk"}
MIN_SDK_VERSION=${MIN_SDK_VERSION:-15}
TARGET_SDK_VERSION=${TARGET_SDK_VERSION:-22}

BUILD_DIR=$(pwd)
COPYRIGHT="# Copyright $(date +%Y) Bifrost Entertainment AS and Tommy Nguyen."
LICENSE="# Distributed under the MIT License."
HEADER="# This file was generated by $(basename $0)"
PACKAGE="com/bifrostentertainment/rainbow"
PROJECT=$(cd -P "$(dirname $0)/.." && pwd)
TARGET=rainbow

# Auto-generate files
cd $PROJECT
tools/shaders-gen.py
cd $BUILD_DIR

echo -n "Removing stale files…"
rm -fr AndroidManifest.xml ant.properties bin build.xml gen jni libs \
    local.properties obj proguard-project.txt project.properties res src
echo " done"

echo -n "Generating project files…"
mkdir jni
android --silent create project \
    --name "Rainbow" \
    --target "android-$TARGET_SDK_VERSION" \
    --path jni \
    --package "${PACKAGE//\//.}" \
    --activity "RainbowActivity" \
    || exit 1
mv jni/* .

# Replace placeholder RainbowActivity with real implementation
rm src/$PACKAGE/RainbowActivity.java
ln -s $PROJECT/src/Platform/Android/RainbowActivity.java src/$PACKAGE/

# Link third-party libraries
ln -s $PROJECT/lib/FMOD/lib/android/fmod.jar libs/
echo " done"

echo -n "Generating jni/Android.mk…"
function cleanup {
  rm -fr CMakeCache.txt CMakeFiles Makefile cmake_install.cmake
}
trap cleanup EXIT

SOURCE_FILES=$(cmake -DANDROID=1 $@ -L $PROJECT | grep SOURCE_FILES | sed -e 's/^SOURCE_FILES:STRING=//' -e 's/[^;]*\.h;//g' -e 's/;/ /g')
SOURCE_FILES=${SOURCE_FILES//$PROJECT\//}

cat > jni/Android.mk << ANDROID_MK
$COPYRIGHT
$LICENSE
$HEADER

LOCAL_PATH := $PROJECT

#
# FMOD shared library
#
include \$(CLEAR_VARS)

LOCAL_MODULE             := fmod
LOCAL_SRC_FILES          := lib/FMOD/lib/android/\$(TARGET_ARCH_ABI)/libfmod.so
LOCAL_EXPORT_CFLAGS      := -DRAINBOW_AUDIO_FMOD=1
LOCAL_EXPORT_C_INCLUDES  := \$(LOCAL_PATH)/lib/FMOD/inc

include \$(PREBUILT_SHARED_LIBRARY)

#
# FMOD Studio shared library
#
include \$(CLEAR_VARS)

LOCAL_MODULE             := fmodstudio
LOCAL_SRC_FILES          := lib/FMOD/lib/android/\$(TARGET_ARCH_ABI)/libfmodstudio.so
LOCAL_EXPORT_CFLAGS      := -DRAINBOW_AUDIO_FMOD=1
LOCAL_EXPORT_C_INCLUDES  := \$(LOCAL_PATH)/lib/FMOD/inc

include \$(PREBUILT_SHARED_LIBRARY)

#
# Rainbow
#
include \$(CLEAR_VARS)

LOCAL_MODULE            := $TARGET
LOCAL_SRC_FILES         := $SOURCE_FILES
LOCAL_C_INCLUDES        := \$(LOCAL_PATH)/src \$(LOCAL_PATH)/lib \\
                           \$(LOCAL_PATH)/src/ThirdParty/FreeType \$(LOCAL_PATH)/lib/FreeType/include \$(LOCAL_PATH)/lib/FreeType/src \\
                           \$(LOCAL_PATH)/lib/Lua \\
                           \$(LOCAL_PATH)/src/ThirdParty/libpng \$(LOCAL_PATH)/lib/libpng \\
                           \$(LOCAL_PATH)/lib/nanosvg/src \\
                           \$(LOCAL_PATH)/lib/nanovg/src \\
                           \$(LOCAL_PATH)/lib/spine-runtimes/spine-c/include \\
                           $NDK_HOME/sources/android/native_app_glue
LOCAL_CFLAGS            := $@
LOCAL_CPPFLAGS          := -std=gnu++1y -Wall -Wextra -Woverloaded-virtual -Wsign-promo -fno-rtti -fno-exceptions
LOCAL_STATIC_LIBRARIES  := android_native_app_glue
LOCAL_SHARED_LIBRARIES  := fmodstudio fmod
LOCAL_LDLIBS            := -landroid -lEGL -lGLESv2 -llog -lz

include \$(BUILD_SHARED_LIBRARY)

\$(call import-module,android/native_app_glue)
ANDROID_MK
echo " done"

echo -n "Generating jni/Application.mk…"
cat > jni/Application.mk << APPLICATION_MK
$COPYRIGHT
$LICENSE
$HEADER

APP_ABI := armeabi-v7a  # all
APP_PLATFORM := android-$MIN_SDK_VERSION
APP_STL := gnustl_shared
APPLICATION_MK
echo " done"

echo -n "Generating res/values/themes.xml…"
cat > res/values/themes.xml << THEMES_XML
<?xml version="1.0" encoding="utf-8"?>
<resources>
  <style name="Rainbow.Theme.Default" parent="@android:style/Theme.NoTitleBar.Fullscreen"></style>
</resources>
THEMES_XML
echo " done"

echo -n "Generating res/values-v11/themes.xml…"
mkdir -p res/values-v11
cat > res/values-v11/themes.xml << THEMES_XML
<?xml version="1.0" encoding="utf-8"?>
<resources>
  <style name="Rainbow.Theme.Default" parent="@android:style/Theme.Holo.NoActionBar.Fullscreen"></style>
</resources>
THEMES_XML
echo " done"

echo -n "Generating AndroidManifest.xml…"
cat > AndroidManifest.xml << ANDROIDMANIFEST_XML
<?xml version="1.0" encoding="utf-8"?>
<manifest xmlns:android="http://schemas.android.com/apk/res/android"
          package="com.bifrostentertainment.rainbow"
          android:versionCode="1"
          android:versionName="1.0">
  <uses-permission android:name="android.permission.MODIFY_AUDIO_SETTINGS" />
  <!-- uses-permission android:name="android.permission.RECORD_AUDIO" / -->
  <uses-permission android:name="android.permission.WRITE_EXTERNAL_STORAGE"
                   android:maxSdkVersion="18" />
  <uses-sdk android:minSdkVersion="$MIN_SDK_VERSION" android:targetSdkVersion="$TARGET_SDK_VERSION" />
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

NDK_DEBUG=${NDK_DEBUG:-1} NDK_TOOLCHAIN_VERSION=4.9 ndk-build -j &&
ant debug
