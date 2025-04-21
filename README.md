# BAISim – Биоморфная AI система распознавания образов с коррекцией ошибок

## Обзор

**BAISim (Biomorphic Artificial Intelligence Simulator)** – это библиотека для распознавания образов с возможностью адаптивной коррекции ошибок. Проект разработан в рамках системы **БСИИРОКО** («Биоморфная система искусственного интеллекта для распознавания образов с коррекцией ошибок»). BAISim предоставляет инструменты для создания каскадных детекторов, объединения их в иерархические структуры и улучшения качества распознавания за счет корректоров ошибок [1]. 

Библиотека ориентирована на разработчиков C++ (с поддержкой Python-обертки) и предназначена для исследований и приложений в области компьютерного зрения, где требуется объяснимая и адаптивная модель распознавания (например, распознавание лиц, символов или сложных объектов).

BAISim может быть использован для:

- анализа видеопотока “на лету”;
- нахождения объектов заданного типа и их атрибутов;
- определения контекста, в котором находятся эти объекты (классификации фона) и автоматического связывания объектов наблюдения с контекстом, на котором они появляются;
- разработки сложных систем, в которых вовлечены сотни интеллектуальных агентов, реализующих семантический анализ видеопотока и выявление нестандартных ситуаций или появление в зоне наблюдения нетипичного явления (например, задымление);
- систем видеонаблюдения, систем аннотации видеоданных, систем контроля качества, систем автоматического управления мобильными платформами и любых других систем, где требуется компьютерное зрение с возможностью понимания происходящего. 

## Возможности

1. **Многокаскадные детекторы**  
   Поддерживается создание и обучение каскадных детекторов [2,6]. Инструмент `CSBuilder` позволяет пошагово собирать детектор из нескольких стадий и обучать его на выборках.

2. **Иерархическая детекция объектов**  
   Модуль `tree_builder` позволяет объединять несколько детекторов в древовидную структуру. Это необходимо для распознавания составных объектов – например, последовательностей цифр на изображении (номера вагонов, автомобильные номера и т.п.) путем объединения отдельных детекторов цифр в единый иерархический распознаватель.

3. **Адаптивная корректировка ошибок**  
   С помощью модуля `supervisor_correctors` реализована возможность корректировки детекторов. Супервизор анализирует результаты первичного детектора на наборе эталонных данных и выявляет ошибки. По найденным ошибкам обучаются специальные модели-корректоры [1], которые интегрируются в агента и улучшают качество детекции в трудных случаях.

4. **Интеллектуальный агент**  
   Основным объектом системы является `TLFAgent` – агент распознавания. Он включает в себя детектор (`TLFObjectDetector`), сканер (`ILFScanner`) для фрагментации изображения, тренер корректоров и список корректоров для уточнения результатов. 

5. **Гибкая настройка и сохранение моделей**  
   Все компоненты (детекторы, деревья, агенты, корректоры) могут конфигурироваться через XML-файлы. Предусмотрены функции загрузки/сохранения конфигурации агента, что облегчает эксперименты и воспроизведение результатов. В репозитории приведены готовые примеры конфигураций (например, `random_agent.xml`, `agent_tree.xml` и др.).

6. **Python-API**  
   Для удобства интеграции имеется Python-модуль обертки ( `python/awplflib.py` ), позволяющий вызывать основные функции (обучение корректоров, экспорт/слияние деревьев, сборка каскада) через Python. Это облегчает быструю проверку идей и написание скриптов без необходимости работы напрямую с C++ кодом.

7. **Примеры и готовые сценарии**  
   В каталоге `examples/` представлены примеры использования библиотеки – готовые `.cpp` файлы с демонстрацией API, а также скрипты `.cmd`/`.bat` для запуска типовых задач (обучение корректоров, тестирование детектора, сборка детектора на основе лицевых изображений и др.).

## Архитектура

Библиотека BAISim реализует следующие базовые компоненты:

- **ILFObjectDetector**  
  Интерфейс, представляющий каскадный детектор объектов [2] и позволяющий осуществлять поиск объектов на изображении (а также в отдельных фрагментах) с помощью интерфейса сканера `ILFScanner`. Исходное изображение разбивается на множество фрагментов, в которых запускается алгоритм каскадного классификатора и находятся кандидаты объектов.

- **TCSBuildDetector**  
  Класс построения каскадного детектора объектов `ILFObjectDetector`. Алгоритм использует базовые признаки типа модифицированного Цензуса для построения слабых классификаторов. Из слабых классификаторов формируются сильные с помощью алгоритма AdaBoost. Сильные классификаторы объединены в последовательность каскадов, отсечение (“нет”) может произойти на любом уровне, а ответ “да” – в случае срабатывания всех сильных классификаторов.

- **TLFAgent**  
  Интеллектуальный агент, использующий механизмы коррекции для уточнения результатов `ILFObjectDetector`. Включает:
  
  - Детектор (`agent::IDetector`) – классификатор фрагментов (например, каскадный классификатор или ансамбль слабых классификаторов), выполняющий основное распознавание. Интерфейс позволяет извлекать признаки из фрагментов для дальнейшего использования в корректорах.
  - Супервизор (`ILFSupervisor`) – интерфейс, позволяющий подключать внешнюю разметку изображений (например, заранее подготовленную или получаемую “на лету”).
  - Корректоры (`agent::TCorrectorCollection`) – набор корректоров, в базовом варианте включающий корректоры типа FN и FP (исправляющие ошибки 1-го и 2-го рода соответственно). В процессе работы агента этот набор может расширяться с помощью механизма обучения корректоров.
  - Обучатель корректоров (`agent::ICorrectorTrainer`) – алгоритм сбора ошибок и обучения корректоров. С помощью разметки супервизора трейнер определяет фрагменты, где детектор ошибается, и формирует списки пропусков или ложных срабатываний. При достижении необходимого количества ошибок, запускается алгоритм обучения корректора определенного типа. Алгоритм сбора и запуска обучения реализован в базовом классе `agent::TCorrectorTrainerBase` [4, 5].
  - NMS (Non-Maximum Suppression) - Алгоритм, объединяющий классифицированные фрагменты для точной локализации детектированного объекта, используя заданный порог пересечения [3].

- **TLFEngine/TreeEngine**  
  Опциональный компонент, представляющий древовидную структуру детекторов или агентов, если задача требует иерархического распознавания, контекстного распознавания или семантического описания сцены.

Взаимодействие компонентов происходит так:  
1. С помощью сканера изображение разбивается на фрагменты.  
2. Фрагменты классифицируются с помощью детектора.  
3. Признаки, извлеченные из каждого проверенного фрагмента, поступают на корректоры, и результат классификации уточняется.  
4. После чего уточняется локализация объекта алгоритмом NMS.  
5. Если к агенту подключен супервизор, то возможно добавление новых корректоров с помощью обучателя корректоров.  

## Установка

### Требования к системе

- Windows 10/11 (64-bit).  
- Компилятор Visual C++ с поддержкой C++17 (рекомендуется Microsoft Visual Studio 2019 или новее).  
- Python 3.11 (x64)  
- Библиотека Boost 1.87  
- Рекомендуемые ресурсы: 8-ядерный CPU, 16 ГБ ОЗУ, дисковое пространство ~30 ГБ, GPU NVIDIA RTX 2080 Ti (или лучше) при использовании GPU-функционала.

### Шаги установки

1. **Установка Python 3.11**  
   Установите Python версии 3.11 (64-bit) с официального сайта. При установке убедитесь, что добавили Python в системный PATH.

2. **Установка Python-зависимостей**  
   Перейдите в корневой каталог BAISim и выполните команду установки зависимостей:
   ```bash
   pip install -r requirements.txt
   ```
3. **Подготовка Visual Studio** 
   Установите Visual Studio Community 2019 (или 2022) с компонентами для разработки на C++ и сборки проектов под Windows (Platform Toolset v142 или v143).

4. **Установка Boost 1.87**
   Скачайте архив Boost 1.87 и распакуйте, либо склонируйте его в папку `3rdparty/boost` внутри `BAISim`.
   Откройте командную строку Windows cmd.exe, перейдите в каталог, куда распакован исходный код библиотеки Boost 1.87 и выполните:
   
   ```bat
   bootstrap.bat
   b2 --build-type=complete --prefix="C:\Path\to\BAISim\3rdparty\boost" -a --reconfigure install
   ```
   
   Замените `C:\Path\to\BAISim` на фактический путь к папке BAISim.README.md
   
   Признаком успешной сборки и инсталляции является наличие в папке C:\Path\to\BAISim\3rdparty\boost\lib файлов:
   - boost_numpy311-vc142-mt-gd-x64-1_87.dll
   - boost_numpy311-vc142-mt-gd-x64-1_87.lib 
   - boost_numpy311-vc142-mt-x64-1_87.dll 
   - boost_numpy311-vc142-mt-x64-1_87.lib 
   - boost_python311-vc142-mt-gd-x64-1_87.dll 
   - boost_python311-vc142-mt-gd-x64-1_87.lib 
   - boost_python311-vc142-mt-x64-1_87.dll 
   - boost_python311-vc142-mt-x64-1_87.lib 

5. **Конфигурация пути к Python**  
   Откройте файл `vc2019/BAISim.props` в любом редакторе и найдите параметр `PythonPath`. Пропишите путь к установленному Python 3.11 (например, `C:\Python311\python.exe`).

6. **Сборка BAISim**  
   - Откройте решение `vc2019/BAISim.sln` в Visual Studio.  
   - Выберите конфигурацию **Release x64**.  
   - Соберите решение: **Build > Build Solution** (или клавиши `Ctrl+Shift+B`).

   Должны успешно скомпилироваться все модули, в том числе исполняемые файлы:
   - `supervisor_correctors.exe`
   - `tree_builder.exe`
   - `csbuilder.exe`


7. **Сборка Image Marker (опционально)**  
   В составе проекта есть компонент Image Marker. Чтобы его собрать, перейдите в `library/photon41/` и следуйте инструкциям в `README.md`.

8. **Проверка установки**  
   После сборки в папке `build/bin/x64/Release` появятся исполняемые модули.  
   Запустите Python-обертку:

   ```bash
   python python/test.py
   ```
   Или , если установлен **Py Launcher**:

   ```bash
   py python/test.py
   ```

   При успехе появятся сообщения **"Test passed!"** для каждого теста.

## Примеры

   После сборки библиотеки можно опробовать ее возможности на примерах из папки `examples/`.

## Структура репозитория

   - **library/awplflib** – Исходники C++-библиотеки: агенты, детекторы, корректоры и др.
   - **examples/** – Примеры: `example_agent.cpp`, `train_correctors.cmd` и др.
   - **models/** – XML-файлы конфигураций:
     - `models/agent/` – конфигурации агентов
     - `models/face/` – сценарии обучения детектора лиц
     - `models/railway/` – примеры для цифр на вагонах
   - **python/** – Python-обертка (`awplflib.py`, `test.py`)
   - **utils/** – Вспомогательные скрипты
   - **tools/** – Утилиты: подготовка датасетов, запуск агентов и др.
   - **3rdparty/** – Внешние зависимости
   - **vc2019/** – Проект Visual Studio 2019 (`BAISim.sln`, `BAISim.props`)
   - **Documentation/** – Руководство пользователя (.docx)
   - **README.md** – Основное описание проекта
   - **LICENSE** – Лицензия (GNU GPL v3.0)

## Используемые датасеты

   `BAISim` может работать с любыми датасетами, подготовленными в нужном формате.  
   Примеры и формат описаны в папке `dataset/`.

## Лицензия

   Проект распространяется под лицензией [GNU GPL v3.0](https://www.gnu.org/licenses/gpl-3.0.html).  
   Вы можете свободно использовать, изменять и распространять программный продукт, сохраняя лицензию для производных работ.  
   Текст лицензии — в файле `LICENSE`.

## Документация

   - **Руководство пользователя** – В папке `Documentation/` находится файл пользователського руководства (формат DOCX) с описанием интерфейса вспомогательного модуля `ImageMarker`.
   - **Документация разработчика** – В папке `library/awplflib/doc/html` приведена документация разработчика в формате Doxigen. Для отображения необходимо запустить файл `index.html` в браузере.
   - **Комментарии в коде** – Основные классы снабжены комментариями (на английском языке) в заголовочных файлах. Например, откройте library/agent/supervisors.h и corrector_trainer.h для понимания, как реализовано обучение корректоров, какие метрики считаются и т.д.
   - **XML-конфигурации** – примеры конфишгураций для обучения детекторов в `models/face/`
   - **README (установка)** – текущее описание


---

# BAISim – Biomorphic AI Pattern Recognition System with Error Correction

## Overview

**BAISim (Biomorphic Artificial Intelligence Simulator)** is a pattern recognition library with adaptive error correction capability. The project is part of the **BSIIROKO** system ("Biomorphic AI System for Pattern Recognition with Error Correction"). BAISim provides tools for creating cascade detectors, combining them into hierarchical structures, and improving recognition quality via error correctors [1].

The library targets C++ developers (with Python wrapper support) and is intended for research and applications in computer vision where an explainable and adaptive recognition model is required (e.g., face recognition, symbol recognition, or complex object recognition).

BAISim can be used for:

- Real-time video stream analysis;
- Detecting objects of a specified type and their attributes;
- Determining the context in which these objects appear (background classification) and automatically linking observed objects with the context;
- Developing complex systems with hundreds of intelligent agents that perform semantic video stream analysis and detect unusual events or phenomena (e.g., smoke detection);
- Video surveillance systems, video data annotation systems, quality control systems, automatic control for mobile platforms, and other computer vision systems requiring scene understanding.

## Features

1. **Multi-cascade Detectors**  
   Supports creation and training of cascade detectors [2,6]. The `CSBuilder` tool allows step-by-step construction and training on datasets.

2. **Hierarchical Object Detection**  
   The `tree_builder` module merges detectors into tree structures. Useful for recognizing compound objects – e.g., digit sequences (railcar numbers, license plates) via combining digit detectors into a hierarchical recognizer.

3. **Adaptive Error Correction**  
   The `supervisor_correctors` module implements error correction. The supervisor analyzes detection results on ground-truth data, identifies errors, and trains special corrector models [1] that integrate with the agent to improve detection in hard cases.

4. **Intelligent Agent**  
   The core object is `TLFAgent` – a recognition agent. It includes a detector (`TLFObjectDetector`), scanner (`ILFScanner`) for image fragmentation, corrector trainer, and a list of correctors to refine results.

5. **Flexible Configuration and Model Saving**  
   All components (detectors, trees, agents, correctors) are configurable via XML. Load/save functions simplify experiments and reproducibility. Sample configs provided (e.g., `random_agent.xml`, `agent_tree.xml`).

6. **Python API**  
   Python wrapper module (`python/awplflib.py`) enables calling core functions (corrector training, tree export/merge, cascade building) from Python. This facilitates scripting and rapid prototyping.

7. **Examples and Ready-to-Run Scenarios**  
   The `examples/` folder contains `.cpp` files showing API use and `.cmd`/`.bat` scripts for common tasks (corrector training, detector testing, face image-based detector building, etc.).

## Architecture

BAISim implements the following components:

- **ILFObjectDetector**  
  Interface representing a cascade detector [2] capable of searching objects in images or fragments using a scanner `ILFScanner`.

- **TCSBuildDetector**  
  Builds `ILFObjectDetector`. Uses modified Census features for weak classifiers, AdaBoost for strong classifier training, and cascade structure for early rejection.

- **TLFAgent**  
  Intelligent agent using correction mechanisms to refine `ILFObjectDetector` results. Includes:
  - Detector (`agent::IDetector`) – fragment classifier.
  - Supervisor (`ILFSupervisor`) – provides external image annotations.
  - Correctors (`agent::TCorrectorCollection`) – set of FN/FP correctors.
  - Corrector trainer (`agent::ICorrectorTrainer`) – error collection and training algorithm using ground-truth data.
  - NMS – Non-Maximum Suppression for localization using overlap threshold [3].

- **TLFEngine/TreeEngine**  
  Optional component for hierarchical or semantic recognition when needed.

Workflow:
1. The scanner splits the image into fragments.  
2. The detector classifies fragments.  
3. Extracted features are passed to correctors to refine classification.  
4. NMS localizes the object.  
5. If a supervisor is connected, new correctors can be added during training.

## Installation

### System Requirements

- Windows 10/11 (64-bit)  
- Visual C++ compiler with C++17 (Visual Studio 2019 or newer recommended)  
- Python 3.11 (x64)  
- Boost 1.87  
- Recommended: 8-core CPU, 16 GB RAM, ~30 GB disk, NVIDIA RTX 2080 Ti GPU (for GPU features)

### Installation Steps

1. **Install Python 3.11**  
   Download and install Python 3.11 (64-bit). Add it to PATH.

2. **Install Python Dependencies**  
   In BAISim root, run:
   ```bash
   pip install -r requirements.txt
   ```

3. **Prepare Visual Studio**  
   Install Visual Studio Community 2019/2022 with C++ and Windows build tools (v142 or v143).

4. **Install Boost 1.87**  
   Download/unpack or clone into `3rdparty/boost`.  
   Open the Windows command line (cmd.exe), navigate to the directory where the Boost 1.87 source code has been extracted, and execute:
   ```bat
   bootstrap.bat
   b2 --build-type=complete --prefix="C:\Path\to\BAISim\3rdparty\boost" -a --reconfigure install
   ```
   In case of successful build and installation the following files should be in the folder C:\Path\to\BAISim\3rdparty\boost\lib:
   - boost_numpy311-vc142-mt-gd-x64-1_87.dll
   - boost_numpy311-vc142-mt-gd-x64-1_87.lib 
   - boost_numpy311-vc142-mt-x64-1_87.dll 
   - boost_numpy311-vc142-mt-x64-1_87.lib 
   - boost_python311-vc142-mt-gd-x64-1_87.dll 
   - boost_python311-vc142-mt-gd-x64-1_87.lib 
   - boost_python311-vc142-mt-x64-1_87.dll 
   - boost_python311-vc142-mt-x64-1_87.lib

5. **Set Python Path**  
   Edit `vc2019/BAISim.props`, set `PythonPath` to installed Python (e.g., `C:\Python311\python.exe`).

6. **Build BAISim**  
   - Open `vc2019/BAISim.sln` in Visual Studio  
   - Select **Release x64**  
   - Build: **Build > Build Solution** (`Ctrl+Shift+B`)

   Executables:
   - `supervisor_correctors.exe`
   - `tree_builder.exe`
   - `csbuilder.exe`

7. **Build Image Marker (optional)**  
   For `Image Marker`, go to `library/photon41/` and follow `README.md`.

8. **Verify Installation**  
   After build, run:
   ```bash
   python python/test.py
   ```
   Or, using **Py Launcher**:

   ```bash
   py python/test.py
   ```

   On success you will see **"Test passed!"** for each test.

## Examples

After build, see `examples/` for usage.

## Repository Structure

   - `library/awplflib` – C++ sources: agents, detectors, correctors  
   - `examples/` – Examples like `example_agent.cpp`, `train_correctors.cmd`  
   - `models/` – Config files:
     - `models/agent/` – agent configs  
     - `models/face/` – face detector training  
     - `models/railway/` – digit detection  
   - `python/` – Python wrapper (`awplflib.py`, `test.py`)  
   - `utils/` – Utility scripts  
   - `tools/` – Tools for dataset prep, agent run, etc.  
   - `3rdparty/` – External dependencies  
   - `vc2019/` – Visual Studio 2019 project  
   - `Documentation/` – User manual (.docx)  
   - `README.md` – Project overview  
   - `LICENSE` – GNU GPL v3.0 license

## Datasets

`BAISim` works with any dataset in the required format.  
See `dataset/` for examples and format.

## License

This project is licensed under [GNU GPL v3.0](https://www.gnu.org/licenses/gpl-3.0.html).  
You are free to use, modify, and distribute the software, preserving the license in derived works.  
See `LICENSE`.

## Documentation

- **User Manual** – `Documentation/` includes a DOCX manual for the `ImageMarker` tool.  
- **Developer Docs** – Doxygen HTML in `library/awplflib/doc/html` (open `index.html`).  
- **Code Comments** – Key classes documented in headers (`supervisors.h`, `corrector_trainer.h`, etc.).  
- **XML Configs** – Examples in `models/face/`  
- **README (Install)** – This file

## Publications

1. Gorban et al. (2021). [High-Dimensional Separability](https://doi.org/10.3390/e23081090). *Entropy*, 23(8), 1090.  
2. Viola & Jones (2001). [Rapid object detection using a boosted cascade](https://doi.org/10.1109/CVPR.2001.990517). *IEEE CVPR*.  
3. Bodla et al. (2017). [Soft-NMS](https://arxiv.org/abs/1704.04503). *arXiv preprint*.  
4. Stasenko et al. (2022). *Biomorphic AI system with adaptive error correction*, *IEEE CNN 2022*.  
5. Stasenko et al. (2024). [Adaptive Correction of the Multi-Cascade Detector](https://ieeexplore.ieee.org/document/XXXXXXX). *IEEE CNN 2024*.  
6. Telnykh et al. (2021). [A Biomorphic Model of Cortical Column](https://doi.org/10.3390/e23111458). *Entropy*, 23(11), 1458.
