Описание конфигурационного файла для построения детектора объектов
CSBuilder64.exe -b <path_to_parameters.xml>
Файл *.xml содержит информацию о параметрах, необходимых для построения многокаскадного детектора:
<?xml version="1.0" encoding="utf-8"?>
<!--BUILD DETECTOR VERSION 1.0-->
<BuildDetector 
bkground_base="path_to_bkgdb" negative_examples="path_to_negdb" log_name="csbuild.log" positive_examples="path_to_posdb" num_samples_per_image="2000" min_negative="200" base_height="16" base_width="16" detector_name="DetectorName.xml" max_stages="1000" num_negative="2500" finish_far="0.50">
<Features 
TLFHFeature="0" TLFDAFeature="0" TLFHAFeature="0" TLFCAFeature="0" TLFVAFeature="0" CSFeature="1" TLFVFeature="0" TLFDFeature="0" TLFCFeature="0" TLFLBPFeature="0">
</Features>
</BuildDetector>
где
•	bkground_base - полный путь до каталога, содержащего фоновые изображения, то есть изображения, на которых гарантированно не присутствует объект, детектор которого должен быть создан в процессе работы модуля;
•	negative_examples – полный путь до каталога, куда будут копироваться фрагменты фоновых изображений в процессе построения детектора;
•	log_name – названия файла отчета работы модуля; 
•	positive_examples - полный путь до каталога, содержащего изображения объекта, детектор которого должен быть создан в процессе работы модуля;
•	min_negative – минимальное количество образцов фона, при котором возможно построение нового каскада детектора. Построение детектора прекращается, когда невозможно набрать заданное количество образцов фона, которые еще не были использованы при построении предыдущих каскадов;
•	base_height, base_width определяют апертуру детектора и характеризуют минимальный размер в пикселах объекта, который может быть найден при помощи этого детектора;
•	detector_name – имя построенного в результате работы модуля детектора;
•	max_stages – максимальное количество итераций при построении детектора; 
•	num_negative – количество фрагментов фона, при достижении этого значения запускается новая итерация в обучении;
•	finish_far = [0,100] – целевое значение ошибки первого рода (в процентах), которое должно быть достигнуто в процессе обучения детектора;
•	название_признака = {0,1} – выбор признаков, на основе вычисления которых будет строиться слабый классификатор детектора. Значение признака, равное единице, означает выбор соответствующего признака. Параметр «название_признака» может принимать следующие значения: TLFAFeature  (simply brigness feature), TLFSFeature (simply dispertion feature), TLFHFeature (horizontal gradient feature with norm), TLFVFeature (vertical gradient feature with norm), TLFDFeature (diagonal gradient feature with norm), TLFCFeature (center feature with norm), TLFLHFeature (horizontal line feature with norm), TLFLVFeature (vertical line feature with norm) – вариации признаков, основанные на функциях Хаара; 
TLFLBPFeature (256 pin binary pattern), TCSSensor - 512 pin binary pattern (census feature); 
