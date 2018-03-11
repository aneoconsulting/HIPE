set PATH=C:\workspace\ANEO\Project\HipeExternal\win64\ffmpeg\bin;C:\workspace\ANEO\Project\HipeExternal\win64\boost_1_62_0\lib64-msvc-14.0;C:\workspace\ANEO\Project\HipeExternal\win64\opencv-3.4\x64\vc14\bin;C:\workspace\ANEO\Project\HipeExternal\win64\gstreamer\1.0\x86_64\bin;C:\workspace\ANEO\Project\HipeExternal\win64\liblept\bin;%PATH%
set TESSDATA_PREFIX=C:\Users\ddubuc.ANEO\Dropbox (ANEO)\HIPE_GROUP\Demos\ID Plates\data\Tesseract

cd %dp0%

hipe_server.exe -m "%cd%\..\..\..\hipeModules\bin\Debug\filter.dll"

