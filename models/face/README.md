## Состав каталога / Directory Structure:

- `csbuild_face_ms1000_ff0p1.xml` - Конфигурационный файл для демонстрации возможности построения детектора по 100 изображениям.

- `csbuild_face_ms1000_ff0p3.xml` - Конфигурационный файл для демонстрации возможности построения детектора на примере базы данных Face_Test.

- `csbuild_face_ms1000_ff0p2.xml` - Конфигурационный файл для демонстрации возможности изменения точности интеллектуального агента.

- `csbuild_face_ms5000_ff0p1.xml` - Конфигурационный файл для измерения времени построения 5000 интеллектуальных агентов.

- `csbuild_face_ms1000_ff0p1.xml` - Configuration file for demonstrating the ability to build a detector from 100 images.

- `csbuild_face_ms1000_ff0p3.xml` - Configuration file for demonstrating the ability to build a detector using Face_Test database.

- `csbuild_face_ms1000_ff0p2.xml` - Configuration file for demonstrating the ability to change the accuracy of the intelligent agent.

- `csbuild_face_ms5000_ff0p1.xml` - Configuration file for measuring the time required to build 5000 intelligent agents.

---

## Описание конфигурационного файла / Description of the Configuration File

To start the detector-building process, use the command:

```bash
CSBuilder.exe -b <path_to_parameters.xml>
```

### Example XML Configuration File

```xml
<?xml version="1.0" encoding="utf-8"?>
<!--BUILD DETECTOR VERSION 1.0-->
<BuildDetector
    bkground_base="path_to_bkgdb"
    negative_examples="path_to_negdb"
    log_name="csbuild.log"
    positive_examples="path_to_posdb"
    num_samples_per_image="2000"
    min_negative="200"
    base_height="16"
    base_width="16"
    detector_name="DetectorName.xml"
    max_stages="1000"
    num_negative="2500"
    finish_far="0.50">

    <Features
        TLFHFeature="0"
        TLFDAFeature="0"
        TLFHAFeature="0"
        TLFCAFeature="0"
        TLFVAFeature="0"
        CSFeature="1"
        TLFVFeature="0"
        TLFDFeature="0"
        TLFCFeature="0"
        TLFLBPFeature="0">
    </Features>
</BuildDetector>
```

---

## Параметры / Parameters

- `bkground_base` - Полный путь до каталога с фоновыми изображениями.\
  Full path to the directory containing background images.

- `negative_examples` - Полный путь до каталога для сохранения фрагментов фоновых изображений.\
  Full path to the directory for storing background image fragments.

- `log_name` - Название файла отчета.\
  Name of the log file.

- `positive_examples` - Полный путь до каталога с изображениями объекта.\
  Full path to the directory containing object images.

- `min_negative` - Минимальное количество фоновых образцов для создания нового каскада.\
  Minimum number of background samples for building a new cascade.

- `base_height`, `base_width` - Минимальные размеры анализируемого фрагмента в пикселях.\
  Minimum size of the analyzed fragment in pixels.

- `detector_name` - Имя созданного детектора.\
  Name of the created detector.

- `max_stages` - Максимальное количество итераций.\
  Maximum number of iterations.

- `num_negative` - Количество фоновых фрагментов для запуска новой итерации обучения.\
  Number of background fragments for starting a new training iteration.

- `finish_far` - Целевое значение ошибки первого рода (в процентах).\
  Target false positive rate (in percentage).

- `feature_name = {0,1}` - Выбор признаков для построения слабого классификатора.\
  Selection of features for building the weak classifier.

---

## Типы признаков / Feature Types

- `TLFAFeature` - Простая яркость (Simple brightness feature).
- `TLFSFeature` - Простое рассеивание (Simple dispersion feature).
- `TLFHFeature` - Горизонтальный градиент (Horizontal gradient feature).
- `TLFVFeature` - Вертикальный градиент (Vertical gradient feature).
- `TLFDFeature` - Диагональный градиент (Diagonal gradient feature).
- `TLFCFeature` - Центрированный признак (Center feature).
- `TLFLHFeature` - Горизонтальная линия (Horizontal line feature).
- `TLFLVFeature` - Вертикальная линия (Vertical line feature).
- `TLFLBPFeature` - Бинарный паттерн из 256 точек (256-pin binary pattern).
- `TCSSensor` - Бинарный паттерн из 512 точек (512-pin binary pattern, Census feature).

