@echo off

rem define the %NDK_PATH% environment variable on your system

set NDKBUILDCMD="%NDK_PATH%\ndk-build"
call %NDKBUILDCMD% NDK_OUT=../../build/jniObjs NDK_LIBS_OUT=./jniLibs 2> log.txt

pause
