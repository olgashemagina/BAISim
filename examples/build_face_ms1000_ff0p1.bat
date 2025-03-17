echo off
rem "Change Path to your Python installation folder."
set PATH=../3rdparty/boost/lib;%LOCALAPPDATA%\Programs\Python\Python311;%PATH%
..\build\bin\x64\Release\CSBuilder.exe -b "../models/face/csbuild_face_ms1000_ff0p1.xml"
