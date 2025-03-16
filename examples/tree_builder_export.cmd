echo off
echo "Пример экспорта модуля агента"
rem "Change Path to your Python installation folder."
set PATH=../3rdparty/boost/lib;%LOCALAPPDATA%\Programs\Python\Python311;%PATH%

..\build\bin\x64\Release\tree_builder.exe export --tree="../models/agent/agent_tree.xml" --agent="../models/agent/agent2_exported.xml" --tree_path="RandomAgent.Agent2"