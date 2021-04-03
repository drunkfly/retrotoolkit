@echo off
setlocal

set JAVA_HOME=C:\Program Files\Java\jdk1.5.0_22
set ANT=%~dp0..\..\Libs\Ant\bin\ant.bat

call "%ANT%"
if errorlevel 1 goto error

cmake "-DSRCPATH=%~dp0Build/Tools" "-DDSTPATH=%~dp0Build/_/Compiler/Java" -P "%~dp0CMakeEmbedClasses.cmake"
if errorlevel 1 goto error

exit /B 0

:error
echo ERROR !!!
exit /B 1
