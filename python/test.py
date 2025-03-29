from awplflib import SupervisorCorrectors as sc
from awplflib import TreeBuilder as tb
from awplflib import CSBuilder as builder
import os
if not "3rdparty/Boost/lib/" in os.environ["PATH"]:
    library_path = os.path.dirname(os.path.realpath(__file__)) + '\\..\\3rdparty\\Boost\\lib\\' 
    os.environ["PATH"] += library_path
#    os.add_dll_directory(library_path)
script_path = os.path.dirname(os.path.realpath(__file__)) + '\\..\\library\\awplflib\\scripts'
os.environ["pyscript_path"] = script_path

# Создание тестирование модуля работы с виртуальным супервизором
# db_path - путь до каталога с базой данных размеченных изображений
# agent_path - путь до супервизора
# detector_path - путь до детектора объектов в случае, если используется именно он, а не супервизор (детектор с обученными корректорами)
# save_path - путь для сохранения обученного супервизора. Если не задан, то супервизор не сохраняется

# Creating a testing module for working with a virtual supervisor
# db_path - path to the directory containing the labeled image database
# agent_path - path to the supervisor
# detector_path - path to the object detector if it is being used instead of the supervisor (a detector with trained correctors)
# save_path - path for saving the trained supervisor. If not specified, the supervisor is not saved

a = sc(db_path="..\\dataset\\small_rtank", agent_path="..\\models\\railway\\agent_railway.xml", detector_path="..\\models\\railway\\rtank_1224.xml", save_path="..\\models\\railway\\agent_railway.xml")

# Обучение и сохранение супервизора
# Train and save supervisor
if(a.train()):
    print("Test passed!")

# Тестирование супервизора
# Test supervisor
if(a.test()):
    print("Test passed!")

# Создание и теcтирование модуля экспорта интеллектуальных агентов TreeBuilder
# Creating and testing the export module for intelligent agents TreeBuilder
b = tb(tree_engine_path="..\\models\\agent\\agent_tree_merge_python.xml", agent_path="..\\models\\agent\\random_agent.xml", tree_paths=("RandomAgent",))

# Создается объект типа TLFTreeEngine из его описания по пути <tree_engine_path> и добавляет агент(<agent_path>) по путям иерархии, определяемые параметром tree_paths. 
# An object of type TLFTreeEngine is created from its description located at <tree_engine_path> and adds an agent (<agent_path>) along the hierarchy paths defined by the tree_paths parameter.
if(b.merge()):
    print("Test passed!")

# Изменение параметров класса TreeBuilder
# Modifying the parameters of the TreeBuilder class
b.agent_path="..\\models\\agent\\random_agent2.xml"
b.tree_paths=("RandomAgent.Agent2",)

# В существующий объект типа TLFTreeEngine по пути <tree_engine_path> добавляется еще один агент(<agent_path>) по путям иерархии, определяемые параметром tree_paths. 
# An additional agent (<agent_path>) is added to an existing TLFTreeEngine object located at <tree_engine_path> along the hierarchy paths defined by the tree_paths parameter.
if(b.merge()):
    print("Test passed!")

# Изменение параметров класса TreeBuilder
# Modifying the parameters of the TreeBuilder class
b.tree_engine_path="..\\models\\agent\\agent_tree.xml"
b.agent_path="..\\models\\agent\\agent2_exported_python.xml"
b.tree_paths=("RandomAgent.Agent2",)

# Из существующего объекта типа TLFTreeEngine по пути <tree_engine_path> экспортируется агент(<agent_path>) по путям иерархии, определяемые параметром tree_paths. 
# An agent (<agent_path>) is exported from an existing TLFTreeEngine object located at <tree_engine_path> along the hierarchy paths defined by the tree_paths parameter.
if(b.export()):
    print("Test passed!")

# Создание и модификация детектора 
# Creating and modifying a detector
c = builder("..\\models\\face\\csbuild_face_ms1000_ff0p1.xml")

# Создание детектора
# Creating a detector
if(c.build()):
    print("Test passed!")

# Вывод информации о детекторе
# Displaying information about the detector
if(c.info()):
    print("Test passed!")

# Дообучение детектора
# Modifying the detector
if(c.add()):
    print("Test passed!")
