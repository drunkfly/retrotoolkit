@echo off
setlocal

set JAVA_HOME=C:\Program Files (x86)\Java\jdk1.8.0_202
set ANT=C:\Program Files\Apache Ant\bin\ant.bat

call "%ANT%"
if errorlevel 1 goto error

exit /B 0

:error
echo ERROR !!!
exit /B 1
