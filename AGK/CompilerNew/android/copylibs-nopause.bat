@echo off

copy /y "obj\local\armeabi-v7a\libAGKCompiler.a" "..\Final\armeabi-v7a\libAGKCompiler.a"
if not %ERRORLEVEL% equ 0 ( GOTO failed )
copy /y "obj\local\arm64-v8a\libAGKCompiler.a" "..\Final\arm64-v8a\libAGKCompiler.a"
if not %ERRORLEVEL% equ 0 ( GOTO failed )

goto :eof

:failed
EXIT /B 1