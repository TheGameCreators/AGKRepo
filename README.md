# AGK Repository
New Home of AGK Source Code for 2024 and Beyond. THIS GITHUB REPO IS CURRENTLY IN CONSTRUCTION AND TESTING. FEEL FREE TO CHECK OUT THE CODE. WE WILL ANNOUNCE WHEN IT IS READY FOR CONTRIBUTIONS.

## WINDOWS PC INSTRUCTIONS
The following instructions requires Windows 10 or above:

## Pre-requisites to compile AGK Studio
Ensure the following are installed with particular attention to the versions:
- Install Visual Studio 2022
- Install Android Studio Iguana 2023.2.1 (Jarsigner path changed to "C:\\Program Files\\Java\\jdk-17bin\\jarsigner.exe" in AGKBuild)
- Android SDK 34.0.0
- Install Java JDK 17
- Install GitHub Desktop
- Install Git Large File Storage (https://docs.github.com/en/repositories/working-with-files/managing-large-files/configuring-git-large-file-storage)

NOTE: Android Studio Iguana no longer installs jarsigner so path changed in AGKBuild to "C:\\Program Files\\Java\\jdk-17bin\\jarsigner.exe"

## Notes on project settings
New APKs submitted to the Google Store require API 34 (Android 14) from August 2024 : https://developer.android.com/google/play/requirements/target-sdk
Small changes to any pre-API 34 projects include:
- Add namespace 'com.thegamecreators.agk_player2' in "AGKPlayer2\build.gradle"
- Add mavenCentral() in "build.gradle"
- Add android.enableR8.fullMode=false to gradle.properties
- Remove jcenter() in "build.gradle"
- Update version classpath 'com.android.tools.build:gradle:8.1.1' // was 7.2.1
- Update version compileSdkVersion 34 in "AGKPlayer2\build.gradle"
- Update version buildToolsVersion "34.0.0" in "AGKPlayer2\build.gradle"
- Ensure that any Android Studio project points to JDK 17 (via "Settings>Build,Exe&Dep>Build Tools>Gradle>GradleJDK")

## Sequence to build AGK Studio
You need to open in Android Studio and build these projects once before attempting to compile AGK:
- \AGK\apps\interpreter_android_google
- \AGK\apps\interpreter_android_amazon
- \AGK\apps\interpreter_android_ouya 

## Setup all environment variables
- Add Environment Variable called NDK_PATH with absolute path to "D:\DEV\AGKREPO\AGK_Build\External\android-ndk-r20b"
- Add Environment Variable called USERNAMEFORAGK with the folder truncation of your Username (so would be 'fred' as might appear in "C:\Users\fred\Documents\"). Do not use your full username or include any path text, just the folder name that appears in the example given.
- Add Environment Variable called VULKAN_SDK_PATH with absolute path to "D:\DEV\AGKREPO\AGK_Build\External"
- Add Environment Variable called AGK_STUDIO_PATH with absolute path to "D:\DEV\AGKREPO\AGK"
- Add Environment Variable called STEAMWORKS_PATH with absolute path to "D:\DEV\AGKREPO\AGK_Build\External\Steamworks"
- Add Environment Variable called ANDROID_HOME with absolute path to "C:\Users\YOURS\AppData\Local\Android\Sdk" where YOURS is the folder assigned you from your username

## Prerequisite Zip Files (SDK files not part of repository, download separately from Release area)
- Unzip "AGK_Build-External.zip" into the "D:\DEV\AGKREPO\AGK_Build\External" folder
- Unzip "AGK_Build-Build.zip" into the "D:\DEV\AGKREPO\AGK_Build\Build" folder
- Unzip "AGK_Build-Shared.zip" into the "D:\DEV\AGKREPO\AGK_Build\Shared" folder

## Your Own Keystore file
- Part of the build process is being able to produce APKs that require a Keystore file that you must provide yourself in "AGK_Build\Signing\keystore.keystore"
- You will be able to enter the password of your own keystore file during the build process
- the default alias is set to tgc in the AGKBuild.sln (edit AGKBuild.sln to change to your own alias)

## Compile AGK in Visual Studio 2022
- Open the AGKBuild project found here "\AGK\tools\AGKBuildSystem\Windows"
- Stick with Debug x64 and select REBUILD on the project shown
- When compile completes, you can run this to open a command line menu to handle the build processes
- The system allows individual steps to be executed, or for simplicity, you can run the whole process in Sequence
- When all steps have been completed successfully and in order, the "AGK_Build\Builds" folder will contain updated files for AGK

Note: Step 19 'Copy shared files from other platforms' must be run after completing steps 1 to 18 by typing S19.

## MAC INSTRUCTIONS
The following instructions requires a modern Mac or Mac-Mini:

## Pre-requisites to compile the Apple binaries
Ensure the following are installed:
- XCODE xx or above
- Install GitHub Desktop for Mac-Mini
- Select YES to any prompt asking to initialize Git Large File Storage

## Sequence to build Apple binaries
- Use GitHub Desktop to clone AGKREPO into a local folder (could be "Users/YourName/DEV/AGKREPO")
- Open Finder and navigate to the tools folder (e.g. /Users/YourName/DEV/AGKREPO/AGK/tools/AGKBuildSystem/Mac
- Open Paths.h and confirm the paths specified here match existing folders in the repo folder structure
- Open the Terminal via the Utilities on your Mac and navigate to the folder (e.g. cd  /Users/YourName/DEV/AGKREPO/AGK/tools/AGKBuildSystem/Mac)
- Now type MAKE to compile and create the AGK Build Program
- Can ignore terminal warnings, there should be no errors reported
- You can now call "./AGKBuild" to run the build program
- Follow the steps in a similar way to the Windows steps sequence until all steps completed
- Any errors you get along the way should be output in the terminal for you to report to TGC (if any)

## LINUX INSTRUCTIONS
The Linux source code is not supported in the AGKREPO at this time.

## In-Construction Notes
* Linux version of AGK Studio downloaded from Steam, Tier 2 Android contains a bat file to start compiling C++ code, instead of an sh script.
* During compilation, it was found that there were no folders "android_common" and "android_common_native" in the composition.
* Need to ASK permission before the GDPR dialogue. This is due to Apple's new(ish) App Tracking Transparency App Tracking Transparency | Apple Developer Documentation.
* Include Apple App Tracking, and a Google certified CMP for asking the GDPR question.
