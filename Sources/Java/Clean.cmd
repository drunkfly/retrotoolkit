@echo off
setlocal

set JAVA_HOME=C:\Program Files\Java\jdk1.5.0_22
set ANT=%~dp0..\..\Libs\Ant\bin\ant.bat

call "%ANT%" clean
if errorlevel 1 goto error

exit /B 0

:error
echo ERROR !!!
exit /B 1
