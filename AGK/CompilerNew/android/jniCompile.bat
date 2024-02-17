@echo off

set NDKBUILDCMD="%NDK_PATH%\ndk-build"
call %NDKBUILDCMD% -j16 2> log.txt
if not %ERRORLEVEL% equ 0 ( GOTO failed )

:failed

if "%1"=="nopause" goto end
pause
:end

if not %ERRORLEVEL% equ 0 ( EXIT /B 1 )