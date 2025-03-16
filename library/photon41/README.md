
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
- Install C++ Builder

### 2. Install photon41

- Open photon41.cbproj in C++ Builder
- Build and install photon41.bpl
- In case of problems with linking run using CLI:
  "c:\program files (x86)\embarcadero\studio\23.0\bin\ilink64.exe" -G8 -L.\Win64\Debug;"c:\program files (x86)\embarcadero\studio\23.0\lib\Win64\debug";photon;..\..\build\lib\borland\Win64\Debug;"c:\program files (x86)\embarcadero\studio\23.0\lib\win64\release";"c:\program files (x86)\embarcadero\studio\23.0\lib\win64\release\psdk";C:\Users\Public\Documents\Embarcadero\Studio\23.0\Dcp\Win64;C:\Users\Public\Documents\Embarcadero\Studio\23.0\DCP\Win64\Debug;C:\Users\Public\Documents\Embarcadero\Studio\23.0\Dcp\Win64 -j.\Win64\Debug;"c:\program files (x86)\embarcadero\studio\23.0\lib\Win64\debug";photon;..\..\build\lib\borland\Win64\Debug;"c:\program files (x86)\embarcadero\studio\23.0\lib\win64\release";"c:\program files (x86)\embarcadero\studio\23.0\lib\win64\release\psdk";C:\Users\Public\Documents\Embarcadero\Studio\23.0\Dcp\Win64;C:\Users\Public\Documents\Embarcadero\Studio\23.0\DCP\Win64\Debug;C:\Users\Public\Documents\Embarcadero\Studio\23.0\Dcp\Win64 -lC:\Users\Public\Documents\Embarcadero\Studio\23.0\DCP\Win64\Debug -v -Gi -Gn -GBphoton41 -Tpd c0pkg64 rtl.bpi vcl.bpi memmgr.a sysinit.o .\Win64\Debug\photon41.o .\Win64\Debug\DIBImage41.o .\Win64\Debug\ExportRaster.o .\Win64\Debug\FImage41.o .\Win64\Debug\ImportRaster.o .\Win64\Debug\PhBatchProcessor.o .\Win64\Debug\PhFrames.o .\Win64\Debug\PhImageTool.o .\Win64\Debug\PhLandmarksTool.o .\Win64\Debug\PhLenzTool.o .\Win64\Debug\PhMediaSource.o .\Win64\Debug\PhPalette.o .\Win64\Debug\PhPaneTool.o .\Win64\Debug\PhReadImagesThread.o .\Win64\Debug\PhRulerTool.o .\Win64\Debug\PhSelectRectTool.o .\Win64\Debug\PhTrackBar.o .\Win64\Debug\PhTriangleTool.o .\Win64\Debug\PhUtils.o .\Win64\Debug\PhVideoTrackBar.o .\Win64\Debug\PhZonesTool.o .\Win64\Debug\PhZoomToRectTool.o .\Win64\Debug\pnglibio.o .\Win64\Debug\tifflibio.o , C:\Users\Public\Documents\Embarcadero\Studio\23.0\Bpl\Win64\photon41.dll , C:\Users\Public\Documents\Embarcadero\Studio\23.0\Bpl\Win64\photon41.map , import64.a cp64mti.a , , .\Win64\Debug\photon41.res

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
- Установите C++ Builder

### 2. Установка photon41

- Откройте photon41.cbproj в C++ Builder
- Соберите и установите photon41.bpl
- В случае проблем с линковкой выполните в командной строке:
  "c:\program files (x86)\embarcadero\studio\23.0\bin\ilink64.exe" -G8 -L.\Win64\Debug;"c:\program files (x86)\embarcadero\studio\23.0\lib\Win64\debug";photon;..\..\build\lib\borland\Win64\Debug;"c:\program files (x86)\embarcadero\studio\23.0\lib\win64\release";"c:\program files (x86)\embarcadero\studio\23.0\lib\win64\release\psdk";C:\Users\Public\Documents\Embarcadero\Studio\23.0\Dcp\Win64;C:\Users\Public\Documents\Embarcadero\Studio\23.0\DCP\Win64\Debug;C:\Users\Public\Documents\Embarcadero\Studio\23.0\Dcp\Win64 -j.\Win64\Debug;"c:\program files (x86)\embarcadero\studio\23.0\lib\Win64\debug";photon;..\..\build\lib\borland\Win64\Debug;"c:\program files (x86)\embarcadero\studio\23.0\lib\win64\release";"c:\program files (x86)\embarcadero\studio\23.0\lib\win64\release\psdk";C:\Users\Public\Documents\Embarcadero\Studio\23.0\Dcp\Win64;C:\Users\Public\Documents\Embarcadero\Studio\23.0\DCP\Win64\Debug;C:\Users\Public\Documents\Embarcadero\Studio\23.0\Dcp\Win64 -lC:\Users\Public\Documents\Embarcadero\Studio\23.0\DCP\Win64\Debug -v -Gi -Gn -GBphoton41 -Tpd c0pkg64 rtl.bpi vcl.bpi memmgr.a sysinit.o .\Win64\Debug\photon41.o .\Win64\Debug\DIBImage41.o .\Win64\Debug\ExportRaster.o .\Win64\Debug\FImage41.o .\Win64\Debug\ImportRaster.o .\Win64\Debug\PhBatchProcessor.o .\Win64\Debug\PhFrames.o .\Win64\Debug\PhImageTool.o .\Win64\Debug\PhLandmarksTool.o .\Win64\Debug\PhLenzTool.o .\Win64\Debug\PhMediaSource.o .\Win64\Debug\PhPalette.o .\Win64\Debug\PhPaneTool.o .\Win64\Debug\PhReadImagesThread.o .\Win64\Debug\PhRulerTool.o .\Win64\Debug\PhSelectRectTool.o .\Win64\Debug\PhTrackBar.o .\Win64\Debug\PhTriangleTool.o .\Win64\Debug\PhUtils.o .\Win64\Debug\PhVideoTrackBar.o .\Win64\Debug\PhZonesTool.o .\Win64\Debug\PhZoomToRectTool.o .\Win64\Debug\pnglibio.o .\Win64\Debug\tifflibio.o , C:\Users\Public\Documents\Embarcadero\Studio\23.0\Bpl\Win64\photon41.dll , C:\Users\Public\Documents\Embarcadero\Studio\23.0\Bpl\Win64\photon41.map , import64.a cp64mti.a , , .\Win64\Debug\photon41.res

### 3. Сборка ObjectMarker

- Откройте PhotonGroup.groupproj
- Соберите ObjectMarker.exe
