@echo off
setlocal
SETLOCAL ENABLEDELAYEDEXPANSION
set PATH=C:\Program Files\CMake\bin;%PATH%
pushd %~dp0
set script_dir=%CD%
popd
echo "Path to binary " %script_dir%
cd %script_dir%

cmake.exe  --build build --target INSTALL --config RelWithDebInfo
if NOT ["%errorlevel%"]==["0"] (
    pause
    exit /b %errorlevel%
)

REM cmake.exe  --build build --target PACKAGE --config RelWithDebInfo
REM if NOT ["%errorlevel%"]==["0"] (
    REM pause
    REM exit /b %errorlevel%
REM )


cd modules
REM cmake.exe --build build --config RelWithDebInfo --target INSTALL

if NOT ["%errorlevel%"]==["0"] (
    pause
    exit /b %errorlevel%
)

cd %dp0%
pause