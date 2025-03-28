echo off
echo "Пример самообучения с использованием GPU для автоматического обнаружения различных видов описания сцены"
rem "Change Path to your Python installation folder."
set PATH=../3rdparty/boost/lib;%LOCALAPPDATA%\Programs\Python\Python311;%PATH%

..\build\bin\x64\Release\supervisor_correctors test --db_folder=..\dataset\small_rtank --agent=..\models\railway\agent_railway_gpu.xml