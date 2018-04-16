@echo off
setlocal
SETLOCAL ENABLEDELAYEDEXPANSION
set PATH=C:\Program Files\CMake\bin;%PATH%
cd %dp0%

REM cmake.exe --build build --config RelWithDebInfo 
cmake.exe --build build --config RelWithDebInfo --target INSTALL
if NOT ["%errorlevel%"]==["0"] (
    pause
    exit /b %errorlevel%
)

cd modules
REM cmake.exe --build build --config RelWithDebInfo
cmake.exe --build build --config RelWithDebInfo --target INSTALL

if NOT ["%errorlevel%"]==["0"] (
    pause
    exit /b %errorlevel%
)

cd %dp0%
