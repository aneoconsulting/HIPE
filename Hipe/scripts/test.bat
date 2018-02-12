setlocal
  cd "C:\Program Files (x86)\Jenkins\workspace\Windows Build\Hipe\header\filter\algos"
  if %errorlevel% neq 0 goto :cmEnd
  C:
  if %errorlevel% neq 0 goto :cmEnd
  cmd /c echo #pragma once > "C:\Program Files (x86)\Jenkins\workspace\Windows Build\Hipe\header\filter\References.h.cmp"
  if %errorlevel% neq 0 goto :cmEnd
  cmd /c echo ^// Algorithm header to reference >> "C:\Program Files (x86)\Jenkins\workspace\Windows Build\Hipe\header\filter\References.h.cmp"
  if %errorlevel% neq 0 goto :cmEnd
  cmd /c "C:\Program Files (x86)\Jenkins\workspace\Windows Build\Hipe\scripts\headerList.bat" "filter/algos" >> "C:\Program Files (x86)\Jenkins\workspace\Windows Build\Hipe\header\filter\References.h.cmp"
  if %errorlevel% neq 0 goto :cmEnd
  cmd /c "C:\Program Files (x86)\Jenkins\workspace\Windows Build\Hipe\scripts\compy.bat" "C:\Program Files (x86)\Jenkins\workspace\Windows Build\Hipe\header\filter\References.h.cmp" "C:\Program Files (x86)\Jenkins\workspace\Windows Build\Hipe\header\filter\References.h"
  if %errorlevel% neq 0 goto :cmEnd
  :cmEnd
  endlocal & call :cmErrorLevel %errorlevel% & goto :cmDone
  :cmErrorLevel
  exit /b %1
  :cmDone
  if %errorlevel% neq 0 goto :VCEnd
  :VCEnd
  
  pause