@echo off
setlocal
SETLOCAL ENABLEDELAYEDEXPANSION
set PATH=C:\Program Files\CMake\bin;%PATH%

pushd %~dp0
set script_dir=%CD%
popd
echo "Path to binary " %script_dir%
cd %script_dir%

call compile.bat
if NOT ["%errorlevel%"]==["0"] (
    pause
    exit /b %errorlevel%
)

cd modules
call compile.bat


if NOT ["%errorlevel%"]==["0"] (
    pause
    exit /b %errorlevel%
)

cd %dp0%
pause