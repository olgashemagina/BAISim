#include "LFAgent.h"
#include "LFEngine.h"
#include "agent/detectors.h"
#include "agent/corrector_trainer.h"
#include "agent/supervisors.h"
#include "LFFileUtils.h"
#include <iostream>

int main() {
	
	// Вариант 1: Создание агента из файла
	auto agent = load_xml("path_to_agent.xml", [](TiXmlElement* node) {
		auto  agent = std::make_unique<TLFAgent>();
		if (node && agent->LoadXML(node))
			return agent;
		return std::unique_ptr<TLFAgent>{};
		});
		
	
	// Вариант 2: Создание агента из детектора
	agent = LoadAgentFromEngine("path_to_detector.xml");
	
	if (!agent) {
		std::cerr << "Ошибка загрузки!" << std::endl;
		return -1;
	}

	agent->SetNmsThreshold(0.5f); // Порог non-maximum suppression

	// Настройка и запуск супервизора для оценки качества работы агента
	auto supervisor = std::make_shared<agent::TDBSupervisor>();
	supervisor->LoadDB("path_to_dataset");
		
	agent->SetSupervisor(supervisor);

	// Загрузка тестового изображения
	std::shared_ptr<TLFImage> img = supervisor->LoadImg(0);
	
	// Выполнение детекции на изображении
	auto detections = agent->Detect(img);

	// Обработка и вывод результатов
	std::cout << "Найдено объектов: " << detections.size() << std::endl;
	for (const auto& detection : detections) {
		// Получение прямоугольника детекции
		TLFRect rect = detection.detected.GetBounds();
		std::cout << "Объект найден в координатах: ("
			<< rect.Left() << ", " << rect.Top() << ") - [Ширина: "
			<< rect.Width() << ", Высота: " << rect.Height() << "]" << std::endl;
	}
			
	// Сохранение результатов работы агента в XML
	save_xml("new_agent.xml", agent->SaveXML());

	return 0;
}
