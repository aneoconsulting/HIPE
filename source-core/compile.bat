@echo off
setlocal
SETLOCAL ENABLEDELAYEDEXPANSION
set PATH=C:\Program Files\CMake\bin;%PATH%

pushd %~dp0
set script_dir=%CD%
popd



echo "Path to binary " %script_dir%
cd %script_dir%



mkdir build
cd build

cmake.exe .. -G "Visual Studio 14 2015 Win64"
if NOT ["%errorlevel%"]==["0"] (
    pause
    exit /b %errorlevel%
)

cd ..
cmake.exe  --build build --target INSTALL --config RelWithDebInfo
if NOT ["%errorlevel%"]==["0"] (
    pause
    exit /b %errorlevel%
    
)

cd %dp0%