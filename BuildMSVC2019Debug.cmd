@echo off
setlocal

set QT=C:\Qt\5.15.0\msvc2019_64
if exist %QT%\bin\moc.exe goto found

echo FATAL ERROR! Qt not found!
goto error

:found

if not exist %~dp0Build mkdir %~dp0Build
if not exist %~dp0Build\MSVC2019 mkdir %~dp0Build\MSVC2019

cd %~dp0Build\MSVC2019
if errorlevel 1 goto error

cmake -G "Visual Studio 16 2019" -A x64 -DCMAKE_PREFIX_PATH=%QT% %~dp0
if errorlevel 1 goto error

cmake --build . --config Debug --parallel 4
if errorlevel 1 goto error

exit /B 0

:error
exit /B 1
