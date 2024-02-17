# AGK Repository
New Home of AGK Source Code for 2024 and Beyond.

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

