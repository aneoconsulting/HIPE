@echo off
set mypath=
call :treeProcess
goto :eof
echo test
:treeProcess
setlocal
for %%f in (*.h*) do echo #include ^<filter\Algos\%mypath%%%f^>
for /D %%d in (*) do (
    set mypath=%mypath%%%d\
    cd %%d
    call :treeProcess
    cd ..
)
endlocal

REM forfiles /s /m *.h* /c "cmd /c echo @relpath"
:eof

exit /b
