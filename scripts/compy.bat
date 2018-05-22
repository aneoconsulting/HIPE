@echo off
setlocal
SETLOCAL ENABLEDELAYEDEXPANSION

ECHO COPY IF SIZE DIFFER

SET "REMOTE_FILE=%1"
SET "LOCAL_FILE=%2"

fc "%REMOTE_FILE%" "%LOCAL_FILE%" > nul
if errorlevel 1 goto error

goto isok

:error
echo file differ copy file
XCOPY /Y "%REMOTE_FILE%" "%LOCAL_FILE%"
goto :end

:isok
echo file are identical

:end

