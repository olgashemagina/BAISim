echo off
echo "Пример изменения модели с использованием GPU во время работы с использованием системы корректоров"
rem "Change Path to your Python installation folder."
set PATH=../3rdparty/boost/lib;%LOCALAPPDATA%\Programs\Python\Python311;%PATH%

..\build\bin\x64\Release\supervisor_correctors train --db_folder=..\dataset\small_rtank --det_gpu=..\models\railway\rtank_1224.xml --save_path=..\models\railway\agent_railway_gpu.xml