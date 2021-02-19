@echo off
setlocal

set QT=C:\Qt\5.15.0\mingw81_32
if exist %QT%\bin\moc.exe goto qtfound

echo FATAL ERROR! Qt not found!
goto error

:qtfound

set MINGW=C:\Qt\Tools\mingw810_32
if exist %MINGW%\bin\gcc.exe goto gccfound

echo FATAL ERROR! MinGW not found!
goto error

:gccfound

if not exist %~dp0Build mkdir %~dp0Build
if not exist %~dp0Build\MinGW32Debug mkdir %~dp0Build\MinGW32Debug

cd %~dp0Build\MinGW32Debug
if errorlevel 1 goto error

set PATH=%MINGW%\bin;%PATH%

cmake -G "MinGW Makefiles" -DCMAKE_BUILD_TYPE=Debug -DCMAKE_PREFIX_PATH=%QT% %~dp0
if errorlevel 1 goto error

mingw32-make -j 4
if errorlevel 1 goto error

exit /B 0

:error
exit /B 1
