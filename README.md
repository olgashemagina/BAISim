
# BAISim Installation Guide

This document provides step-by-step instructions for installing and setting up BAISim on your system.

## Prerequisites

Before you begin, ensure the following software is installed on your system:

1. **Python 3.11**  
2. **Visual Studio Community 2019**  
3. **Boost 1.87**

## Installation Steps

### 1. Install Python 3.11

- Download Python 3.11 from the official website:  
  [Python 3.11 Download](https://www.python.org/downloads/release/python-3110/)

### 2. Install Dependencies (NumPy and scikit-learn)

- Install the required Python dependencies using `pip`:

  ```bash
  pip install numpy==1.26.4 scikit-learn==1.6.1

### 3. Set Python Path in `BAISim.props`

- Set the correct path to your Python installation in the `PythonPath` property inside the `./vc2019/BAISim.props` file.

### 4. Install Visual Studio Community 2019

- Download and install Visual Studio Community 2019 from the official website:  
  [Visual Studio 2019](https://visualstudio.microsoft.com/visual-cpp-build-tools/)

### 5. Build Boost 1.87

- Download and build Boost 1.87 into the folder `./3rdparty/boost`.

- Navigate to the `./3rdparty` directory and run the following commands:

  ```bash
  bootstrap.bat vc142
  b2 --build-type=complete --prefix="PATH_TO_BAISIM\3rdparty\boost" -a --reconfigure install

(Replace PATH_TO_BAISIM with the actual path to your BAISim project directory.)

### 6. Open the Solution in Visual Studio 2019

- Open the solution file `./vc2019/BAISim.sln` in Visual Studio 2019.
- Build the project by selecting `Build > Build Solution` from the top menu or by pressing `Ctrl+Shift+B`.