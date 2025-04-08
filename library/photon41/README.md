
# ImageMarker Installation Guide

This document provides step-by-step instructions for installing and setting up ImageMarker on your system.

## Minimum System Requirements

To ensure stable performance, your system should meet the following minimum requirements:

- **Operating System:** Windows 10 or Windows 11 (64-bit)  
- **Processor:** 8-core CPU  
- **Memory (RAM):** 16 GB  
- **Storage:** At least **30 GB of free disk space**  

## Prerequisites

Before you begin, ensure the following software is installed on your system:

1. **Embarcadero C++ Builder 12 Community Edition**  
2. **photon41 package for Borland Builder**  


## Installation Steps

### 1. Install Embarcadero C++ Builder 12 Community Edition

- Download C++ Builder 12 from the official website:  
  [C++ Builder Download](https://www.embarcadero.com/products/cbuilder/starter)
- Install C++ Builder. When you will be prompted to choose the components to install, set the TeeChart Standard on the right side of the screen.

### 2. Install photon41

- Open PhotonLibs.groupproj in C++ Builder
- Build and install photon41.bpl
- In case of problems with linking ([ilink64 Error] Fatal: Error detected (EXE1832)) run using CLI:
  ```
  "c:\program files (x86)\embarcadero\studio\23.0\bin\ilink64.exe" -G8 -L.\Win64\Release;"c:\program files (x86)\embarcadero\studio\23.0\lib\Win64\Release";photon;..\..\build\lib\borland\Win64\Release;"c:\program files (x86)\embarcadero\studio\23.0\lib\win64\release";"c:\program files (x86)\embarcadero\studio\23.0\lib\win64\release\psdk";C:\Users\Public\Documents\Embarcadero\Studio\23.0\Dcp\Win64;C:\Users\Public\Documents\Embarcadero\Studio\23.0\DCP\Win64\Release;C:\Users\Public\Documents\Embarcadero\Studio\23.0\Dcp\Win64 -j.\Win64\Release;"c:\program files (x86)\embarcadero\studio\23.0\lib\Win64\Release";photon;..\..\build\lib\borland\Win64\Release;"c:\program files (x86)\embarcadero\studio\23.0\lib\win64\release";"c:\program files (x86)\embarcadero\studio\23.0\lib\win64\release\psdk";C:\Users\Public\Documents\Embarcadero\Studio\23.0\Dcp\Win64;C:\Users\Public\Documents\Embarcadero\Studio\23.0\DCP\Win64\Release;C:\Users\Public\Documents\Embarcadero\Studio\23.0\Dcp\Win64 -lC:\Users\Public\Documents\Embarcadero\Studio\23.0\DCP\Win64\Release -v -Gi -Gn -GBphoton41 -Tpd c0pkg64 rtl.bpi vcl.bpi memmgr.a sysinit.o .\Win64\Release\photon41.o .\Win64\Release\DIBImage41.o .\Win64\Release\ExportRaster.o .\Win64\Release\FImage41.o .\Win64\Release\ImportRaster.o .\Win64\Release\PhBatchProcessor.o .\Win64\Release\PhFrames.o .\Win64\Release\PhImageTool.o .\Win64\Release\PhLandmarksTool.o .\Win64\Release\PhLenzTool.o .\Win64\Release\PhMediaSource.o .\Win64\Release\PhPalette.o .\Win64\Release\PhPaneTool.o .\Win64\Release\PhReadImagesThread.o .\Win64\Release\PhRulerTool.o .\Win64\Release\PhSelectRectTool.o .\Win64\Release\PhTrackBar.o .\Win64\Release\PhTriangleTool.o .\Win64\Release\PhUtils.o .\Win64\Release\PhVideoTrackBar.o .\Win64\Release\PhZonesTool.o .\Win64\Release\PhZoomToRectTool.o .\Win64\Release\pnglibio.o .\Win64\Release\tifflibio.o , C:\Users\Public\Documents\Embarcadero\Studio\23.0\Bpl\Win64\photon41.dll , C:\Users\Public\Documents\Embarcadero\Studio\23.0\Bpl\Win64\photon41.map , import64.a cp64mti.a , , .\Win64\Release\photon41.res
  ```
- If Embarcadero C++ Builder 12 Community Edition is installed in a different path, change it in the command line accordingly.
  The Photon41.dll file, which will appear as a result of linking, is sufficient for building Image Marker.

- If building and installing photon41.bpl on a 64-bit architecture fails, install the 32-bit photon41.bpl from the current directory in Embarcadero C++ Builder.
  To do this, open Embarcadero C++ Builder, go to the menu item Component -> Install packages... and select photon41.bpl.
  This component is required to maintain the ability to edit the Image Marker GUI. 

### 3. Build ObjectMarker

- Open PhotonGroup.groupproj
- Build ObjectMarker.exe


# ImageMarker Инструкция по установке

В этом документе предлагается пошаговая инструкция по установке и настройке ImageMaker'а на вашей системе.

## Минимальные системные требования

Чтобы обеспечить оптимальную производительность, система должна удовлетворять следующим минимальным требованиям

- **ОС:** Windows 10 or Windows 11 (64-bit)  
- **Процессор:** 8-core CPU  
- **Оперативная память (RAM):** 16 GB  
- **Диск:** Как минимум **30 GB свободного пространства**  

## Перед установкой

Перед установкой необходимо установить следующее программное обеспечение

1. **Embarcadero C++ Builder 12 Community Edition**  
2. **photon41 package for Borland Builder**  


## Установка

### 1. Установите Embarcadero C++ Builder 12 Community Edition

- Загрузите C++ Builder 12 с официального сайта:  
  [C++ Builder Download](https://www.embarcadero.com/products/cbuilder/starter)
- Установите C++ Builder. При выборе компонент для установки отметьте TeeChart Standard в правой части экрана

### 2. Установка photon41

- Откройте PhotonLibs.groupproj в C++ Builder
- Соберите и установите photon41.bpl
- В случае проблем с линковкой([ilink64 Error] Fatal: Error detected (EXE1832)) выполните в командной строке:
  ```  
  "c:\program files (x86)\embarcadero\studio\23.0\bin\ilink64.exe" -G8 -L.\Win64\Release;"c:\program files (x86)\embarcadero\studio\23.0\lib\Win64\Release";photon;..\..\build\lib\borland\Win64\Release;"c:\program files (x86)\embarcadero\studio\23.0\lib\win64\release";"c:\program files (x86)\embarcadero\studio\23.0\lib\win64\release\psdk";C:\Users\Public\Documents\Embarcadero\Studio\23.0\Dcp\Win64;C:\Users\Public\Documents\Embarcadero\Studio\23.0\DCP\Win64\Release;C:\Users\Public\Documents\Embarcadero\Studio\23.0\Dcp\Win64 -j.\Win64\Release;"c:\program files (x86)\embarcadero\studio\23.0\lib\Win64\Release";photon;..\..\build\lib\borland\Win64\Release;"c:\program files (x86)\embarcadero\studio\23.0\lib\win64\release";"c:\program files (x86)\embarcadero\studio\23.0\lib\win64\release\psdk";C:\Users\Public\Documents\Embarcadero\Studio\23.0\Dcp\Win64;C:\Users\Public\Documents\Embarcadero\Studio\23.0\DCP\Win64\Release;C:\Users\Public\Documents\Embarcadero\Studio\23.0\Dcp\Win64 -lC:\Users\Public\Documents\Embarcadero\Studio\23.0\DCP\Win64\Release -v -Gi -Gn -GBphoton41 -Tpd c0pkg64 rtl.bpi vcl.bpi memmgr.a sysinit.o .\Win64\Release\photon41.o .\Win64\Release\DIBImage41.o .\Win64\Release\ExportRaster.o .\Win64\Release\FImage41.o .\Win64\Release\ImportRaster.o .\Win64\Release\PhBatchProcessor.o .\Win64\Release\PhFrames.o .\Win64\Release\PhImageTool.o .\Win64\Release\PhLandmarksTool.o .\Win64\Release\PhLenzTool.o .\Win64\Release\PhMediaSource.o .\Win64\Release\PhPalette.o .\Win64\Release\PhPaneTool.o .\Win64\Release\PhReadImagesThread.o .\Win64\Release\PhRulerTool.o .\Win64\Release\PhSelectRectTool.o .\Win64\Release\PhTrackBar.o .\Win64\Release\PhTriangleTool.o .\Win64\Release\PhUtils.o .\Win64\Release\PhVideoTrackBar.o .\Win64\Release\PhZonesTool.o .\Win64\Release\PhZoomToRectTool.o .\Win64\Release\pnglibio.o .\Win64\Release\tifflibio.o , C:\Users\Public\Documents\Embarcadero\Studio\23.0\Bpl\Win64\photon41.dll , C:\Users\Public\Documents\Embarcadero\Studio\23.0\Bpl\Win64\photon41.map , import64.a cp64mti.a , , .\Win64\Release\photon41.res
  ```
- Если Embarcadero C++ Builder 12 Community Edition установлен по другому пути, то поменяйте его в командной строке на соответствующий.
  Photon41.dll, которая появится в результате линковке, достаточно для сборки Image Marker.
- Если в 64-битной архитектуре не удалось собрать и установить photon41.bpl, то установите в Embarcadero C++ Builder 32-битный photon41.bpl из текущей директории.
  Для этого откройте Embarcadero C++ Builder, пункт меню Component->Install packages... и выберите photon41.bpl. Компонент необходим для сохранения возможности редакирования GUI Image Marker. 

### 3. Сборка ObjectMarker

- Откройте PhotonGroup.groupproj
- Соберите ObjectMarker.exe
