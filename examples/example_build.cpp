#include "LF.h"
#include "LFBuilder.h"
#include "tinyxml.h"
#include <iostream>

int main() {

    // Создание объекта класса TCSBuildDetector
    TCSBuildDetector builder;

    // Загрузка параметров конфигурации из XML-файла
    if (!builder.LoadConfig("detector_config.xml")) {
        std::cerr << "Ошибка загрузки конфигурационного файла!" << std::endl;
        return -1;
    }

    // Отображение информации о структуре детектора
    std::cout << "Информация о текущем детекторе:" << std::endl;
    builder.PrintDetectorInfo();

    // Вариант 1: Добавление нового сильного классификатора
    builder.AddNewClassifier();
    
    std::cout << "Новый сильный классификатор успешно добавлен." << std::endl;

    //  Вариант 2: Запуск процесса обучения всего детектора
    if (!builder.Build()) {
        std::cerr << "Ошибка во время процесса обучения детектора!" << std::endl;
        return -1;
    }

    std::cout << "Процесс обучения успешно завершён." << std::endl;

    
    return 0;
}