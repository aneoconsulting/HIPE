@echo off
setlocal
SETLOCAL ENABLEDELAYEDEXPANSION
set PATH=C:\Program Files\CMake\bin;%PATH%
cd %dp0%

cmake.exe  --build build --target INSTALL --config RelWithDebInfo
if NOT ["%errorlevel%"]==["0"] (
    pause
    exit /b %errorlevel%
)

cmake.exe  --build build --target PACKAGE --config RelWithDebInfo
if NOT ["%errorlevel%"]==["0"] (
    pause
    exit /b %errorlevel%
)


cd modules
REM cmake.exe --build build --config RelWithDebInfo --target INSTALL

if NOT ["%errorlevel%"]==["0"] (
    pause
    exit /b %errorlevel%
)

cd %dp0%
pause