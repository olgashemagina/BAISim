# ����������� �� ��������� BAISim

� ���� ����������� ������� ���� �� ��������� � ��������� BAISim �� ����� ����������.

## ����������� ��������� ����������

��� ���������� ������ BAISim ��� ��������� ������ ��������������� ��������� �����������:

- **������������ �������:** Windows 10 ��� Windows 11 (64-bit)
- **���������:** 8-������� CPU
- **����������� ������:** 16 ��
- **����������:** NVIDIA GPU �� ���� **RTX 2080 Ti** � 8 �� �����������
- **�������� ������������:** ������� 30 �� ���������� �����

## ��������������� ����������

����� ���������� ���������, ��� � ��� ����������� ��������� ����������� �����������:

- **Python 3.11 x64**
- **Visual Studio Community Edition � ���������� C++17**
- **Boost 1.87**

## ���� ���������

### 1. ���������� Python 3.11

- �������� � ���������� Python 3.11 x64 � ������������ �����:  
  [Python 3.11 Download](https://www.python.org/downloads/release/python-3110/)

### 2. ���������� �����������

- ���������� ����������� Python-������ � ������� �������:

```bash
pip install -r requirements.txt
```

### 3. ��������� ���� � Python � `BAISim.props`

- �������� ���� `./vc2019/BAISim.props` � ���������� ���������� ���� � Python � ��������� `PythonPath`.

### 4. ���������� Visual Studio Community Edition

- �������� � ���������� Visual Studio Community � ������������ �����:  
  [Visual Studio Community](https://visualstudio.microsoft.com/)

### 5. �������� ���������� Boost 1.87

- �������� � ���������� ���������� Boost 1.87 � ����� `./3rdparty/boost`.
- ��������� � ������� `./3rdparty/boost` � ��������� �������:

```bash
.\bootstrap.bat
.\b2 --build-type=complete --prefix="����_�_BAISIM\3rdparty\boost" -a --reconfigure install
```

(�������� `����_�_BAISIM` �� �������� ���� � ����� � BAISim.)

### 6. �������� ������� � Visual Studio

- �������� ���� ������� `./vc2019/BAISim.sln` � Visual Studio Community. ���� ���������, �������� ������ �������� �� ����� �����.
- �������� ������ ����� ���� `Build > Build Solution` ��� ���������� ������ `Ctrl+Shift+B`.

## ������� �������������

������� ������������� ��������� � ����� `examples`:

- `train_correctors.cmd` � ������ ������ � ������ �������� �����������.
- `test_correctors.cmd` � ������ ������ � ������ ������������.
- `example_agent.cpp` � ���������� ������ �� ������� ���.
- `example_build.cpp` � ���������� �������� ���������.



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
