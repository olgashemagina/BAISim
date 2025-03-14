# Руководство по установке BAISim

В этом руководстве описаны шаги по установке и настройке BAISim на вашем компьютере.

## Минимальные системные требования

Для корректной работы BAISim ваш компьютер должен соответствовать следующим требованиям:

- **Операционная система:** Windows 10 или Windows 11 (64-bit)
- **Процессор:** 8-ядерный CPU
- **Оперативная память:** 16 ГБ
- **Видеокарта:** NVIDIA GPU не ниже **RTX 2080 Ti** с 8 ГБ видеопамяти
- **Дисковое пространство:** минимум 30 ГБ свободного места

## Предварительные требования

Перед установкой убедитесь, что у вас установлено следующее программное обеспечение:

- **Python 3.11 x64**
- **Visual Studio Community Edition с поддержкой C++17**
- **Boost 1.87**

## Шаги установки

### 1. Установите Python 3.11

- Скачайте и установите Python 3.11 x64 с официального сайта:  
  [Python 3.11 Download](https://www.python.org/downloads/release/python-3110/)

### 2. Установите зависимости

- Установите необходимые Python-пакеты с помощью команды:

```bash
pip install -r requirements.txt
```

### 3. Настройте путь к Python в `BAISim.props`

- Откройте файл `./vc2019/BAISim.props` и установите правильный путь к Python в параметре `PythonPath`.

### 4. Установите Visual Studio Community Edition

- Скачайте и установите Visual Studio Community с официального сайта:  
  [Visual Studio Community](https://visualstudio.microsoft.com/)

### 5. Соберите библиотеку Boost 1.87

- Скачайте и скопируйте библиотеку Boost 1.87 в папку `./3rdparty/boost`.
- Перейдите в каталог `./3rdparty/boost` и выполните команды:

```bash
.\bootstrap.bat
.\b2 --build-type=complete --prefix="ПУТЬ_К_BAISIM\3rdparty\boost" -a --reconfigure install
```

(Замените `ПУТЬ_К_BAISIM` на реальный путь к папке с BAISim.)

### 6. Откройте решение в Visual Studio

- Откройте файл решения `./vc2019/BAISim.sln` в Visual Studio Community. Если требуется, измените версии проектов на более новые.
- Соберите проект через меню `Build > Build Solution` или сочетанием клавиш `Ctrl+Shift+B`.

## Примеры использования

Примеры использования находятся в папке `examples`:

- `train_correctors.cmd` — запуск агента в режиме обучения корректоров.
- `test_correctors.cmd` — запуск агента в режиме тестирования.
- `example_agent.cpp` — интеграция агента во внешний код.
- `example_build.cpp` — интеграция обучения детектора.



# BAISim Installation Guide

This document provides step-by-step instructions for installing and setting up BAISim on your system.

## Minimum System Requirements

To ensure stable performance, your system should meet the following minimum requirements:

- **Operating System:** Windows 10 or Windows 11 (64-bit)  
- **Processor:** 8-core CPU  
- **Memory (RAM):** 16 GB  
- **Graphics Card:** NVIDIA GPU, no lower than **RTX 2080 Ti** with 8gb GRAM
- **Storage:** At least **30 GB of free disk space**  

## Prerequisites

Before you begin, ensure the following software is installed on your system:

1. **Python 3.11 x64**   
2. **Visual Studio Community Edition with c++17 support**  
3. **Boost 1.87**

## Installation Steps

### 1. Install Python 3.11

- Download Python 3.11 x64 from the official website:  
  [Python 3.11 Download](https://www.python.org/downloads/release/python-3110/)

### 2. Install Dependencies

- Install the required Python dependencies using `pip`:

  ```bash
  pip install -r requirements.txt

### 3. Set Python Path in `BAISim.props`

- Set the correct path to your Python installation in the `PythonPath` property inside the `./vc2019/BAISim.props` file.

### 4. Install Visual Studio Community Edition

- Download and install Visual Studio Community from the official website:  
  [Visual Studio Community](https://visualstudio.microsoft.com/)

### 5. Build Boost 1.87

- Download and copy Boost 1.87 into the folder `./3rdparty/boost`.

- Navigate to the `./3rdparty/boost` directory and run the following commands:

  ```bash
  .\bootstrap.bat
  .\b2 --build-type=complete --prefix="PATH_TO_BAISIM\3rdparty\boost" -a --reconfigure install

(Replace PATH_TO_BAISIM with the actual path to your BAISim project directory.)

### 6. Open the Solution in Visual Studio

- Open the solution file `./vc2019/BAISim.sln` in Visual Studio Community Edition and change version for all projects if needed.
- Build the project by selecting `Build > Build Solution` from the top menu or by pressing `Ctrl+Shift+B`.
