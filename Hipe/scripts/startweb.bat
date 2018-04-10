@echo off

rem Ensure this Node.js and npm are first in the PATH
call "C:\Program Files\nodejs\nodevars.bat"

set "PATH=%APPDATA%\npm;%~dp0;%PATH%"

cd "c:\xampp\mysql\bin"
mysqld.exe --uninstall "mysql_hipe"
mysqld.exe --install "mysql_hipe"
net start "mysql_hipe"

cd "c:\workspace\ANEO\Project\hipe-group\hipeWebFinal"
node app.js

cd %~dp0
