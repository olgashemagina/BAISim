echo off
echo "Пример слияния модуля агента"
rem "Change Path to your Python installation folder."
set PATH=../3rdparty/boost/lib;%LOCALAPPDATA%\Programs\Python\Python311;%PATH%

echo "Создаем файл agent_tree_merge.xml с одним агентом"
..\build\bin\x64\Release\tree_builder.exe merge --tree="../models/agent/agent_tree_merge.xml" --agent="../models/agent/random_agent.xml" --tree_path="RandomAgent"
echo "Производим слияние одного агента с другим"
..\build\bin\x64\Release\tree_builder.exe merge --tree="../models/agent/agent_tree_merge.xml" --agent="../models/agent/random_agent2.xml" --tree_path="RandomAgent.Agent2"