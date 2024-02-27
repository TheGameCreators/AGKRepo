# AGK Repository
New Home of AGK Source Code for 2024 and Beyond. THIS GITHUB REPO IS CURRENTLY IN CONSTRUCTION AND TESTING. FEEL FREE TO CHECK OUT THE CODE. WE WILL ANNOUNCE WHEN IT IS READY FOR CONTRIBUTIONS.

## Pre-requisites to compile AGK Studio
Ensure the following are installed with particular attention to the versions:
- Install Visual Studio 2022
- Chipmunk 2021.2.1 Canary 2
- Android SDK 33.0.3
- Android 12.0 platform
- Install Java JDK 17

## Notes on project settings
New APKs submitted to the Google Store require API 33 (Android 13) since August 2023 : https://developer.android.com/google/play/requirements/target-sdk
Small changes to any pre-API 33 projects include:
- Add namespace 'com.thegamecreators.agk_player2' in "AGKPlayer2\build.gradle"
- Add mavenCentral() in "build.gradle"
- Remove jcenter() in "build.gradle"
- Update version classpath 'com.android.tools.build:gradle:8.0.0' // was 7.2.1
- Update version compileSdkVersion 33 in "AGKPlayer2\build.gradle"
- Update version buildToolsVersion "33.0.3" in "AGKPlayer2\build.gradle"
- Ensure that any Android Studio project points to JDK 17 (via "Settings>Build,Exe&Dep>Build Tools>Gradle>GradleJDK")

## Sequence to build AGK Studio
You need to open in Android Studio and build these projects once before attempting to compile AGK:
- \AGK\apps\interpreter_android_google
- \AGK\apps\interpreter_android_amazon
- \AGK\apps\interpreter_android_ouya 

## Compile AGK in Visual Studio 2022
- Add Environment Variable called NDK_PATH with absolute path to "D:\DEV\AGKREPO\AGK_Build\External\android-ndk-r20b"
- Add Environment Variable called USERNAMEFORAGK with your Username (maybe 'fred' as might appear in "C:\Users\fred\Documents\")
- Add Environment Variable called VULKAN_SDK_PATH with absolute path to "D:\DEV\AGKREPO\AGK_Build\External\Vulkan"
- Add Environment Variable called AGK_STUDIO_PATH with absolute path to "D:\DEV\AGKREPO\AGK"
- Add Environment Variable called STEAMWORKS_PATH with absolute path to "D:\DEV\AGKREPO\AGK_Build\External\Steamworks"
- Open the AGKBuild project found here "\AGK\tools\AGKBuildSystem\Windows"
- Stick with Debug x64 and select REBUILD on the project shown
- When compile completes, you can run this to open a command line menu to handle the build processes

## In-Construction Notes
* Linux version of AGK Studio downloaded from Steam, Tier 2 Android contains a bat file to start compiling C++ code, instead of an sh script.
* During compilation, it was found that there were no folders "android_common" and "android_common_native" in the composition.
  
