# Примеры использования библиотеки

После сборки библиотеки вы можете опробовать её основные возможности на приведённых ниже примерах.

## Сценарии из командной строки

В репозитории предоставлены готовые скрипты для выполнения распространенных задач. Запустите их двойным кликом или через консоль:

### Обучение агента

Скрипт `train_correctors.cmd` запускает модуль supervisor_correctors в режиме обучения. Скрипт использует исходный детектор и датасет с разметкой, указанные в параметрах, и обучает корректоры. По завершении работы будет сохранен новый XML-файл агента с учетом обученных корректоров. Аналогичный пример с использованием GPU находится в файле `train_correctors_gpu.cmd`. 
Для корректной работы скрипта необходима установленная база [Railway_rtank_test](../dataset/README.md).

### Тестирование агента

Скрипт `test_correctors.cmd` запускает supervisor_correctors в режиме тестирования. Он берет готовый агент (с уже обученными корректорами), прогоняет его по тестовой выборке изображений и выводит метрики качества (например, процент обнаружения, число ошибок и т.д.) по заданному порогу overlap (IoU). Аналогичный пример с использованием GPU находится в файле `test_correctors_gpu.cmd`. 
Для корректной работы скрипта необходима установленная база [Railway_rtank_test](../dataset/README.md).

### Тестирование построения детекторов

В папке `examples/` находятся несколько `.bat` файлов, показывающих процесс поэтапного обучения каскадного детектора лица. 

- `build_face_ff0p1.bat` - Командный файл для построения детектора на примере базы данных [Face_Test](../dataset/README.md).
- `build_face_ms1000_ff0p2.bat` - Командный файл для демонстрации возможности изменения точности интеллектуального агента.
- `build_face_ms5000_ff0p1.bat` - Командный файл для измерения времени построения 5000 интеллектуальных агентов.

Для корректной работы необходимы установленные базы [Face_Test](../dataset/README.md) и [Background](../dataset/README.md).

### Тестирование построения детекторов на усеченной базе из 100 примеров

- Необходимо скачать датасет [Face_Test_100images](../dataset/README.md) и распаковать в корневой каталог `dataset` репозитория.
- Скачать и распаковать в корневой каталог `dataset` репозитория датасет [Background](../dataset/README.md), лицензия.
- Затем запустить файл `build_face_ms1000_100_ff0p1.bat` из каталога `examples` в корне репозитория.

Пример структуры каталогов `dataset` после распаковки:

```
dataset
    face_100 
        dbexport - каталог изображений, содержащих объекты
    bg – каталог изображений, не содержащих объекты
```

### Export/Merge древовидных детекторов

Модуль `tree_builder` поддерживает экспорт и слияние деревьев детекторов. Скрипт `tree_builder_export.cmd` показывает, как выгрузить обученного агента в XML-файл, а `tree_builder_merge.cmd` – как объединить несколько файлов в один комплексный детектор.

### Тестирование количественных характеристик

Скрипт `tests_agent.cmd` демонстрирует количественные характеристики работы агентов и тренеров корректора. Успешное прохождение тестов будет помечено словом `PASSED`.


## Использование API (C++)

### Пример создания агента распознавания (`example_agent.cpp`)

Пример показывает создание и использование агента для распознавания объектов:

```cpp

#include "LFAgent.h"
#include "LFEngine.h"
// ... подключение необходимых заголовков ...

// 1. Инициализация агента из конфигурационного XML-файла
auto agent = load_xml("path_to_agent.xml", [](TiXmlElement* node) {
	auto  agent = std::make_unique<TLFAgent>();
	if (node && agent->LoadXML(node))
		return agent;
	return std::unique_ptr<TLFAgent>{};
	});
	
// Альтернативно можно создать агент прямо из конфигурации детектора:
agent = LoadAgentFromEngine("path_to_detector.xml");
	
if (!agent) {
	std::cerr << "Ошибка загрузки!" << std::endl;
	return -1;
}

// Настройка параметров детектора (например, порог NMS)
agent->SetNmsThreshold(0.5f); 

// 2. Настройка супервизора и загрузка датасета
auto supervisor = std::make_shared<agent::TDBSupervisor>();
supervisor->LoadDB("path_to_dataset"); // папка с изображениями и разметкой
		
agent->SetSupervisor(supervisor); // привязать супервизора к агенту

	// 3. Загрузка изображения для теста
std::shared_ptr<TLFImage> img = supervisor->LoadImg(0);
	
// 4. Выполнение детекции
auto detections = agent->Detect(img);

// 5. Обработка и вывод результатов
std::cout << "Найдено объектов: " << detections.size() << std::endl;
for (const auto& detection : detections) {
	// Получение прямоугольника детекции
	TLFRect rect = detection.detected.GetBounds();
	std::cout << "Объект найден в координатах: ("
		<< rect.Left() << ", " << rect.Top() << ") - [Ширина: "
		<< rect.Width() << ", Высота: " << rect.Height() << "]" << std::endl;
}
			
// 6. Сохранение обновленного агента (например, с натренированными корректорами) в XML
save_xml("new_agent.xml", agent->SaveXML());


```
В этом примере агент загружается из XML-файла `path_to_agent.xml`, который описывает детектор и при необходимости подключенные корректоры. Затем мы создаем `Supervisor`, загружаем с его помощью датасет (в папке `path_to_dataset` предполагается набор изображений и файлов разметки), связываем супервизор с агентом и выполняем детекцию на первом изображении. Результаты (список обнаруженных объектов) выводятся в консоль, и наконец мы сохраняем состояние агента в новый XML (`new_agent.xml`).


### Пример построения детектора (`example_build.cpp`)

Пример демонстрирует, как создать и обучить каскадный детектор:

```cpp
#include "LF.h"
#include "LFBuilder.h"
// Загрузка конфигурации и обучение каскадного детектора...

//1. Создание объекта класса TCSBuildDetector
TCSBuildDetector builder;

//2. Загрузка параметров конфигурации из XML-файла
if (!builder.LoadConfig("detector_config.xml")) {
    std::cerr << "Ошибка загрузки конфигурационного файла!" << std::endl;
    return -1;
}

//3. Отображение информации о структуре детектора
std::cout << "Информация о текущем детекторе:" << std::endl;
builder.PrintDetectorInfo();

//4.1 Добавление нового сильного классификатора
builder.AddNewClassifier();
   

//4.2 Или запуск процесса обучения всего детектора
if (!builder.Build()) {
    std::cerr << "Ошибка во время процесса обучения детектора!" << std::endl;
    return -1;
}

```

### Пример работы с древовидным детектором (`example_tree_builder.cpp`)

Показан процесс загрузки и применения древовидного детектора:

```cpp
#include "LFAgent.h"
#include "LFEngine.h"
// Загрузка древовидного детектора и обработка изображений...

TLFTreeEngine tree_engine;
std::string tree_path = "../../../../models/railway/rw_tree_numbers_num3.xml";
//1. Загружаем TLFTreeEngine из файла
if (!tree_engine.Load(tree_path)) {
	std::cerr << "Ошибка загрузки файла: " << tree_path << std::endl;
	return -1;
}
std::string img_path = "../../../../images/vagon3.jpg";
std::shared_ptr<TLFImage> img = std::make_shared<TLFImage>();
//2. Загружаем изображение из файла
if (!img->LoadFromFile(img_path.c_str())) {
	std::cerr << "Ошибка загрузки файла: "  << img_path << std::endl;
	return -1;
}
//3. Детектим нашим TLFTreeEngine изображение и на выходе получаем список детекций так как на этой картинке есть цифра 3
auto opt_detected_items = tree_engine.Detect(img);
if (!opt_detected_items) {
	std::cerr << "tree_engine return false for " << img_path << std::endl;
	return false;
}
//4. Здесь получаем список детекций
auto vec = opt_detected_items.value();

```


---

# Library Usage Examples

After building the library, you can try its main features using the examples below.

## Command Line Scenarios

The repository provides ready-made scripts for common tasks. Run them via double-click or console:

### Agent Training

The `train_correctors.cmd` script runs the `supervisor_correctors` module in training mode. It uses the specified base detector and labeled dataset and trains the correctors. A new agent XML file with trained correctors will be saved after completion. A similar GPU-enabled script is `train_correctors_gpu.cmd`.  
To run this script properly, the [Railway_rtank_test](../dataset/README.md) dataset must be installed.

### Agent Testing

The `test_correctors.cmd` script runs `supervisor_correctors` in test mode. It takes a trained agent, runs it on a test image set, and outputs performance metrics (e.g., detection rate, number of errors) for a given overlap threshold (IoU). A GPU variant is available in `test_correctors_gpu.cmd`.  
Requires the [Railway_rtank_test](../dataset/README.md) dataset.

### Detector Construction Testing

The `examples/` folder contains `.bat` files demonstrating step-by-step training of a face cascade detector:

- `build_face_ff0p1.bat` – Builds a detector using the [Face_Test](../dataset/README.md) dataset.
- `build_face_ms1000_ff0p2.bat` – Demonstrates adjusting the agent's precision.
- `build_face_ms5000_ff0p1.bat` – Measures time for training 5000 agents.

Requires installed [Face_Test](../dataset/README.md) and [Background](../dataset/README.md) datasets.

### Training with a Reduced Dataset of 100 Examples

- Download and extract the [Face_Test_100images](../dataset/README.md) dataset to the root `dataset` folder.
- Also download and extract the [Background](../dataset/README.md) dataset to the same folder.
- Then run `build_face_ms1000_100_ff0p1.bat` from the `examples` directory.

Example structure of `dataset` after extraction:

```
dataset
    face_100 
        dbexport - image folder with objects
    bg – image folder without objects
```

### Export/Merge Tree Detectors

The `tree_builder` module supports export and merging of detector trees. The script `tree_builder_export.cmd` exports an agent to XML, and `tree_builder_merge.cmd` merges several files into a composite detector.

### Quantitative Characteristic Testing

The `tests_agent.cmd` script tests the performance of agents and corrector trainers. A successful test run will be marked with `PASSED`.

## API Usage (C++)

### Agent Creation Example (`example_agent.cpp`)

This example shows how to create and use an object detection agent:

```cpp
#include "LFAgent.h"
#include "LFEngine.h"

// 1. Initialize agent from XML
auto agent = load_xml("path_to_agent.xml", [](TiXmlElement* node) {
	auto agent = std::make_unique<TLFAgent>();
	if (node && agent->LoadXML(node))
		return agent;
	return std::unique_ptr<TLFAgent>{};
});

// Alternatively create from detector config
agent = LoadAgentFromEngine("path_to_detector.xml");

if (!agent) {
	std::cerr << "Load error!" << std::endl;
	return -1;
}

agent->SetNmsThreshold(0.5f); 

// 2. Set up supervisor and load dataset
auto supervisor = std::make_shared<agent::TDBSupervisor>();
supervisor->LoadDB("path_to_dataset");

agent->SetSupervisor(supervisor);

// 3. Load test image
std::shared_ptr<TLFImage> img = supervisor->LoadImg(0);

// 4. Perform detection
auto detections = agent->Detect(img);

// 5. Print results
std::cout << "Objects found: " << detections.size() << std::endl;
for (const auto& detection : detections) {
	TLFRect rect = detection.detected.GetBounds();
	std::cout << "Found at: (" << rect.Left() << ", " << rect.Top() << ") - [Width: "
		<< rect.Width() << ", Height: " << rect.Height() << "]" << std::endl;
}

// 6. Save updated agent
save_xml("new_agent.xml", agent->SaveXML());
```

### Detector Building Example (`example_build.cpp`)

Demonstrates building and training a cascade detector:

```cpp
#include "LF.h"
#include "LFBuilder.h"


//1. Create TCSBuildDetector object
TCSBuildDetector builder;

//2. Load params from XML
if (!builder.LoadConfig("detector_config.xml")) {
    std::cerr << "Ошибка загрузки конфигурационного файла!" << std::endl;
    return -1;
}

//3. Show detector information
builder.PrintDetectorInfo();

//4.1 Add new classifier
builder.AddNewClassifier();
   

//4.2 Or build detector from scratch
if (!builder.Build()) {
    std::cerr << "Training error!" << std::endl;
    return -1;
}
```

### Tree Detector Example (`example_tree_builder.cpp`)

Shows how to use a tree detector:

```cpp
#include "LFAgent.h"
#include "LFEngine.h"

TLFTreeEngine tree_engine;
std::string tree_path = "../../../../models/railway/rw_tree_numbers_num3.xml";

if (!tree_engine.Load(tree_path)) {
	std::cerr << "Failed to load: " << tree_path << std::endl;
	return -1;
}

std::string img_path = "../../../../images/vagon3.jpg";
std::shared_ptr<TLFImage> img = std::make_shared<TLFImage>();

if (!img->LoadFromFile(img_path.c_str())) {
	std::cerr << "Failed to load: " << img_path << std::endl;
	return -1;
}

auto opt_detected_items = tree_engine.Detect(img);
if (!opt_detected_items) {
	std::cerr << "tree_engine returned false for " << img_path << std::endl;
	return false;
}

auto vec = opt_detected_items.value();
```

