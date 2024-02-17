@echo off

call copylibs-nopause.bat
if not %ERRORLEVEL% equ 0 ( GOTO failed )

echo Success
pause
exit

:failed
echo Failed
pause
exit
