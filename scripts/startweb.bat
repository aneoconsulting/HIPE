REM @echo off

rem Ensure this Node.js and npm are first in the PATH
CD /D "C:\soft\nodejs"
call "C:\soft\nodejs\nodevars.bat"
if NOT ["%errorlevel%"]==["0"] (
    pause
    exit /b %errorlevel%
)

set "PATH=%APPDATA%\npm;%~dp0;%PATH%"

cd "c:\xampp\mysql\bin"
if NOT ["%errorlevel%"]==["0"] (
    pause
    exit /b %errorlevel%
)

mysqld.exe --uninstall "mysql_hipe"
if NOT ["%errorlevel%"]==["0"] (
    pause
    exit /b %errorlevel%
)

mysqld.exe --install "mysql_hipe"
if NOT ["%errorlevel%"]==["0"] (
    pause
    exit /b %errorlevel%
)
net start "mysql_hipe"


cd /D "D:\workspace\hipe-vc2017\webserver\webserver-hipe"
if NOT ["%errorlevel%"]==["0"] (
    pause
    exit /b %errorlevel%
)

node app.js
if NOT ["%errorlevel%"]==["0"] (
    pause
    exit /b %errorlevel%
)

cd %~dp0
