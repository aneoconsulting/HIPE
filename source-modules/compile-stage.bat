@echo off
setlocal
SETLOCAL ENABLEDELAYEDEXPANSION
set PATH=C:\Program Files\CMake\bin;%PATH%
pushd %~dp0
set script_dir=%CD%
popd



echo "Path to binary " %script_dir%
cd "%script_dir%/stage/vision"



mkdir build
cd build

cmake.exe -DWITH_CAFFE=ON .. -G "Visual Studio 15 2017 Win64"
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
pause