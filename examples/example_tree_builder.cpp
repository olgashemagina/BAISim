#include "LFAgent.h"
#include "LFEngine.h"
#include "agent/detectors.h"
#include "agent/corrector_trainer.h"
#include "agent/supervisors.h"
#include "LFFileUtils.h"
#include <iostream>

int main() {
	TLFTreeEngine tree_engine;
	std::string tree_path = "../../../../models/railway/rw_tree_numbers_num3.xml";
	// Загружаем TLFTreeEngine из файла
	if (!tree_engine.Load(tree_path)) {
		std::cerr << "Ошибка загрузки файла: " << tree_path << std::endl;
		return -1;
	}
	std::string img_path = "../../../../images/vagon3.jpg";
	std::shared_ptr<TLFImage> img = std::make_shared<TLFImage>();
	// Загружаем изображение из файла
	if (!img->LoadFromFile(img_path.c_str())) {
		std::cerr << "Ошибка загрузки файла: "  << img_path << std::endl;
		return -1;
	}
	// Детектим нашим TLFTreeEngine изображение и на выходе получаем список детекций так как на этой картинке есть цифра 3
	auto opt_detected_items = tree_engine.Detect(img);
	if (!opt_detected_items) {
		std::cerr << "tree_engine return false for " << img_path << std::endl;
		return false;
	}
	// Здесь получаем список детекций
	auto vec = opt_detected_items.value();
	
	return 0;
}
