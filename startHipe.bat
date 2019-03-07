pushd %~dp0
set script_dir=%CD%
popd


cd %script_dir%\install\hipe-core\bin\Release\

starthipe.bat

pause