@echo off
setlocal

rem Win32

if not exist %~dp0Build mkdir %~dp0Build
if not exist %~dp0Build\Runtimes\Win32 mkdir %~dp0Build\Runtimes\Win32

cd %~dp0Build\Runtimes\Win32
if errorlevel 1 goto error

cmake -G "Visual Studio 16 2019" -A Win32 -DEXCLUDE_GUI=1 %~dp0
if errorlevel 1 goto error

cmake --build . --config Release --target SDL2Runtime --parallel 4
if errorlevel 1 goto error

copy /b %~dp0Build\Runtimes\Win32\Release\SDL2Runtime.exe %~dp0Sources\Compiler\Resources\Win32Runtime.bin
if errorlevel 1 goto error

exit /B 0

:error
exit /B 1
