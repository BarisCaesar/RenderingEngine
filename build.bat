@echo off
cd /d "%~dp0"


for /f "tokens=*" %%i in ('"C:\Program Files (x86)\Microsoft Visual Studio\Installer\vswhere.exe" -latest -products * -requires Microsoft.VisualStudio.Component.VC.Tools.x86.x64 -property displayName') do (
    set VS_VERSION=%%i
)


set VS_GENERATOR=
if "%VS_VERSION%"=="2026" set VS_GENERATOR=Visual Studio 18 2026
if "%VS_VERSION%"=="2022" set VS_GENERATOR=Visual Studio 17 2022
if "%VS_VERSION%"=="2019" set VS_GENERATOR=Visual Studio 16 2019
if "%VS_VERSION%"=="2017" set VS_GENERATOR=Visual Studio 15 2017

if "%VS_GENERATOR%"=="" (
    set VS_GENERATOR=Visual Studio 17 2022
)

echo Using CMake generator: %VS_GENERATOR%


if not exist build mkdir build
cd build
cmake -G "%VS_GENERATOR%" ..
cmake --build . --config Debug

pause