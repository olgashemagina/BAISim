# Используемые датасеты

BAISim может быть использован с разными датасетами, при условии подготовки их в поддерживаемом формате. Папка `dataset` в корне репозитория является базовой для хранения датасетов. Ниже приведены краткое описание и ссылки на подготовленные датасеты, которые могут быть распакованы в папку для последующей с ними работы.

- **Face_Test:** тестовый датасет, содержащий экспортированные фрагменты лиц для демонстрации возможностей обучения детекторов.
  - [Ссылка на Face_Test](https://www.kaggle.com/datasets/nuidelirina/face-test)
  - [Лицензия](https://opendatacommons.org/licenses/dbcl/1-0/)

- **Face_Test_100images:** субдатасет из 100 фрагментов датасета Face_Test. Демонстрирует возможности построения детектора по 100 примерам.
  - [Ссылка на Face_Test_100images](https://www.kaggle.com/datasets/nuidelirina/face-test-100images)
  - [Лицензия](https://opendatacommons.org/licenses/dbcl/1-0/)

- **Background:** датасет негативных примеров, необходимый для построения детекторов.
  - [Ссылка на Background](https://www.kaggle.com/datasets/nuidelirina/background)
  - [Лицензия](https://opendatacommons.org/licenses/dbcl/1-0/)

- **Railway_rtank_test:** датасет изображений цистерн с разметкой. Демонстрирует возможности построения агентов с корректорами и их тестирование.
  - [Ссылка на Railway_rtank_test](https://www.kaggle.com/datasets/nuidelirina/railway-rtank-test)
  - [Лицензия](https://cdla.dev/permissive-1-0/)

- **Railway:** датасет с данными, относящимися к различным объектам железной дороги (номера на железнодорожных вагонах, цифры от 0 до 9 с номеров вагонов, железнодорожный состав из нескольких вагонов, товарный вагон, цистерна).
  - [Ссылка на Railway](https://www.kaggle.com/datasets/nuidelirina/railway)
  - [Лицензия](https://cdla.dev/permissive-1-0/)

- **Road types:** датасет с дорогами трех типов: дороги с асфальтовым покрытием, дороги с грунтовым покрытием, заснеженные дороги.
  - [Ссылка на Road types](https://www.kaggle.com/datasets/nuidelirina/road-types)
  - [Лицензия](https://opendatacommons.org/licenses/dbcl/1-0/)

- **Landing Pad:** датасет с изображениями двух типов посадочных платформ для беспилотного летательного аппарата вертолетного типа: площадка в виде буквы «Н», традиционно используемая; посадочная площадка, предложенная разработчиками, изображение которой инвариантно к повороту.
  - [Ссылка на Landing Pad](https://www.kaggle.com/datasets/nuidelirina/landing-pad)
  - [Лицензия](https://opendatacommons.org/licenses/dbcl/1-0/)

- **Face recognition:** датасет с изображениями лиц.
  - [Ссылка на Face recognition](https://www.kaggle.com/datasets/nuidelirina/face-recognition)
  - [Лицензия](https://opendatacommons.org/licenses/dbcl/1-0/)

---

Во всех датасетах, кроме `Face`, `Face_100` и `Railway_rtank_test` содержатся каталоги `correct`, `test`, `train` (внутри которого находится каталог `dbexport` с экспортированными фрагментами для построения детектора). Изображения и одноименные файлы в формате XML (разметка) находятся в одной папке. Изображения могут быть в форматах **jpg** или **awp**, поддерживаемые BAISim. Файл разметки содержит информацию о размеченных объектах.

## Формат данных

Супервизор ожидает, что в папке датасета изображения и файлы разметки организованы определенным образом. Для каждого изображения в той же папке и с тем же именем должен располагаться XML-файл с координатами объектов. Ниже приведен формат файла разметки:

```xml
<?xml version="1.0" ?>
<TLFSemanticImageDescriptor ImageWidth="640" ImageHeight="640">
  <TLFDetectedItem DetectorName="Human marked" Raiting="0" Type="741be8a0-0538-40ff-8be3-b0023aa14c65" zoneType="1" Angle="0" Racurs="0" left="0" top="0" right="93" bottom="127" Comment="" />
</TLFSemanticImageDescriptor>
```

Блок `TLFSemanticImageDescriptor` содержит информацию о размерах изображения, а также информацию обо всех размеченных объектах в блоках `TLFDetectedItem`.

Количество блоков `TLFDetectedItem` соответствует количеству размеченных объектов на изображении. Каждый блок содержит информацию о:
- источнике разметки – ручная или автоматическая при помощи детектора.
- поле `Raiting` заполняется в случае автоматической разметки и соответствует уровню уверенности детектора при обнаружении объекта.
- поле `Type` представляет собой UUID типа размеченного объекта.
- поле `zoneType` представляет собой код типа разметки – прямоугольник, многоугольник и т.д.
- поля `Angle` и `Racurs` – дополнительные свойства размеченного объекта, могут выставляться при ручной разметке и использоваться для фильтрации изображений.
- поля `left`, `top`, `right`, `bottom` задают размеры прямоугольника, ограничивающего область разметки.

В случае ручной разметки при помощи Image Marker файл разметки формируется автоматически при выделении объекта на изображении (подробнее описано в инструкции к [Image Marker](https://github.com/olgashemagina/BAISim/tree/main/Documentation/IM_pr_description_user_manual.docx)).


---

# Used Datasets

BAISim can be used with different datasets, provided they are prepared in a supported format. The `dataset` folder at the root of the repository serves as the base for storing datasets. Below is a brief description and links to prepared datasets that can be unpacked into this folder for further use.

- **Face_Test:** A test dataset containing exported face fragments to demonstrate detector training capabilities.
  - [Link to Face_Test](https://www.kaggle.com/datasets/nuidelirina/face-test)
  - [License](https://opendatacommons.org/licenses/dbcl/1-0/)

- **Face_Test_100images:** A subset of 100 fragments from the Face_Test dataset. Demonstrates the ability to build a detector from 100 examples.
  - [Link to Face_Test_100images](https://www.kaggle.com/datasets/nuidelirina/face-test-100images)
  - [License](https://opendatacommons.org/licenses/dbcl/1-0/)

- **Background:** A dataset of negative examples needed for building detectors.
  - [Link to Background](https://www.kaggle.com/datasets/nuidelirina/background)
  - [License](https://opendatacommons.org/licenses/dbcl/1-0/)

- **Railway_rtank_test:** A dataset of tank car images with markup. Demonstrates the capabilities of building agents with correctors and testing them.
  - [Link to Railway_rtank_test](https://kaggle.com/datasets/3266611f5a350adf9309967982d67b51a9a3027449ef524ec1e2c8833143a1a4)
  - [License](https://cdla.dev/permissive-1-0/)

- **Railway:** A dataset with data related to various railway objects (numbers on railcars, digits 0 to 9 from car numbers, a train of several cars, a freight car, a tank car).
  - [Link to Railway](https://kaggle.com/datasets/fbfe48ad425a0d4ef55a30aaf5f5997fca2d8d34b43cba94943ce4a88c3e3026)
  - [License](https://cdla.dev/permissive-1-0/)

- **Road types:** A dataset with roads of three types: asphalt roads, dirt roads, snowy roads.
  - [Link to Road types](https://www.kaggle.com/datasets/nuidelirina/road-types)
  - [License](https://opendatacommons.org/licenses/dbcl/1-0/)

- **Landing Pad:** A dataset with images of two types of landing pads for an unmanned aerial vehicle of the helicopter type: an “H”-shaped pad traditionally used; and a landing pad proposed by the developers, whose image is invariant to rotation.
  - [Link to Landing Pad](https://www.kaggle.com/datasets/nuidelirina/landing-pad)
  - [License](https://opendatacommons.org/licenses/dbcl/1-0/)

- **Face recognition:** A dataset with images of faces.
  - [Link to Face recognition](https://www.kaggle.com/datasets/nuidelirina/face-recognition)
  - [License](https://opendatacommons.org/licenses/dbcl/1-0/)

---

All datasets except `Face`, `Face_100`, and `Railway_rtank_test` contain the directories `correct`, `test`, `train` (inside which is the `dbexport` directory with exported fragments for building a detector). Images and XML files (markup) with the same name are located in the same folder. Images can be in **jpg** or **awp** formats supported by BAISim. The markup file contains information about the labeled objects.

## Data Format

The supervisor expects images and markup files in the dataset folder to be organized in a specific way. For each image, there should be an XML file in the same folder and with the same name, containing object coordinates. Below is the markup file format:

```xml
<?xml version="1.0" ?>
<TLFSemanticImageDescriptor ImageWidth="640" ImageHeight="640">
  <TLFDetectedItem DetectorName="Human marked" Raiting="0" Type="741be8a0-0538-40ff-8be3-b0023aa14c65" zoneType="1" Angle="0" Racurs="0" left="0" top="0" right="93" bottom="127" Comment="" />
</TLFSemanticImageDescriptor>
```

The `TLFSemanticImageDescriptor` block contains image size information and information about all labeled objects in `TLFDetectedItem` blocks.

The number of `TLFDetectedItem` blocks corresponds to the number of labeled objects in the image. Each block contains information about:
- the source of the labeling – manual or automatic using a detector.
- the `Raiting` field is filled in case of automatic labeling and corresponds to the detector's confidence level.
- the `Type` field is the UUID of the labeled object type.
- the `zoneType` field is the code for the markup type – rectangle, polygon, etc.
- the `Angle` and `Racurs` fields are additional properties of the labeled object, can be set during manual labeling and used for filtering images.
- the `left`, `top`, `right`, `bottom` fields define the bounding box dimensions of the labeled area.

In the case of manual labeling using Image Marker, the markup file is generated automatically when selecting an object in the image (described in more detail in the [Image Marker manual](https://github.com/olgashemagina/BAISim/tree/main/Documentation/IM_pr_description_user_manual.docx)).


