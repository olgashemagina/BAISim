#include <iostream>
#include <fstream>

#include "LFAgent.h"
#include "agent/tests.h"
#include "agent/detectors.h"
#include "agent/corrector_trainer.h"
#include "agent/supervisors.h"



static std::unique_ptr<TSCObjectDetector>			build_random_detector(float activate_prob = 0.1f, int num_features = 10, int num_stages = 1, int base_width = 24, int base_height = 24) {
	auto detector = std::make_unique<TSCObjectDetector>();

	TLFObjectList* strongs = detector->GetStrongs();

	detector->SetBaseWidht(base_width);
	detector->SetBaseHeight(base_height);
	auto scanner = new TLFScanner();

	scanner->SetBaseWidth(base_width);
	scanner->SetBaseHeight(base_height);

	detector->SetScanner(scanner);

	//float weak_prob = std::powf(activate_prob, 1.f / float(num_features));

	for (int s = 0; s < num_stages; ++s) {

		TCSStrong* strong = new TCSStrong;
		strongs->Add(strong);

		strong->SetThreshold(1.0);

		for (int f = 0; f < num_features; ++f) {


			int w = tests::GetRand(1, base_width / 3);
			int h = tests::GetRand(1, base_height / 3);

			int x = tests::GetRand(0, base_width - w * 3);
			int y = tests::GetRand(0, base_height - h * 3);


			auto sensor = LFCreateFeature("CSFeature", x, y, w, h);

			TCSWeak* weak = new TCSWeak(sensor);
			strong->AddWeak(weak);
			weak->SetWeight(1.0 / double(num_features));
			int activated = 0;
			for (int i = 0; i < 512; ++i) {
				AWPBYTE activation = tests::GetRand(0, 100) < activate_prob * 100 ? 1 : 0;
				if (activation)
					activated++;
				weak->SetClassificator(i, activation);
			}

			//std::cout << "Activated patterns: " << activated << std::endl;

		}

	}
	return detector;
}

// Calculate size of description of detector in memory
static size_t calc_detector_size(TSCObjectDetector* detector) {
	size_t total = sizeof(TSCObjectDetector);

	total += sizeof(TLFScanner);

	TLFObjectList* strongs = detector->GetStrongs();

	for (int s = 0; s < strongs->GetCount(); ++s) {

		total += sizeof(TCSStrong);

		TCSStrong* strong = (TCSStrong*)strongs->Get(s);

		for (int f = 0; f < strong->GetCount(); ++f) {

			total += sizeof(TCSWeak);
			total += sizeof(TCSSensor);
		}
	}
	return total;
}

// Calculate size of description of detector in memory
static std::unique_ptr<TSCObjectDetector> copy_detector(TSCObjectDetector* other) {

	auto detector = std::make_unique<TSCObjectDetector>();

	TLFObjectList* strongs = detector->GetStrongs();

	detector->SetBaseWidht(other->GetBaseWidth());
	detector->SetBaseHeight(other->GetBaseHeight());
	auto scanner = new TLFScanner();

	scanner->SetBaseWidth(other->GetBaseWidth());
	scanner->SetBaseHeight(other->GetBaseHeight());

	detector->SetScanner(scanner);

	for (int s = 0; s < other->GetStagesCount(); ++s) {

		TCSStrong* other_strong = (TCSStrong*)other->GetStrongs()->Get(s);
		TCSStrong* strong = new TCSStrong;
		strongs->Add(strong);

		strong->SetThreshold(other_strong->GetThreshold());

		for (int f = 0; f < other_strong->GetCount(); ++f) {
			TCSWeak* other_weak = (TCSWeak*)other_strong->GetWeak(f);

			TCSWeak* weak = new TCSWeak(other_weak->Fetaure());
			strong->AddWeak(weak);
			weak->SetWeight(other_weak->Weight());
			for (int i = 0; i < 512; ++i) {
				weak->SetClassificator(i, other_weak->Classificator(i));
			}
		}

	}
	return detector;
}

static bool test_agents_sequence() {
	// Make random HD image
	auto img = tests::create_empty_image(1280, 720, true);
	//auto img = create_empty_image(320, 240, true);

	//static_assert(noexcept(TLFAgent()), "!");

	std::vector<std::unique_ptr<TLFAgent>>			agents;

	for (size_t i = 0; i < 100; ++i) {
		agents.emplace_back(std::make_unique<TLFAgent>());

		auto detector_base = build_random_detector(0.6, 10);

		auto detector = agent::CreateCpuDetector();
		if (!detector->Initialize(std::move(detector_base))) {
			std::cout << "Cant initialize TStagesDetector" << std::endl;
		}
		detector->SetMinStages(0);

		agents.back()->Initialize(std::move(detector), nullptr);
		agents.back()->SetNmsThreshold(0.7);
	}

	TimeDiff	td_total(true);

	agent::TDetections		detections;
	for (size_t a = 0; a < agents.size(); ++a) {

		TimeDiff	td;
		auto items = agents[a]->Detect(img, &detections);

		std::cout << "Agent " << a + 1 << " detected " << items.size() << " items in " << detections.size() << " rois for " << td.GetDiffMs() << " ms." << std::endl;

		//if (items.size() > 5) {
		if (detections.empty()) {
			detections.clear();

			for (const auto& item : items) {
				auto rect = item.detected.GetBounds();
				// Increase size of rect;
				//rect.Inflate(rect.Width() * 0.1, rect.Height() * 0.1);
				detections.push_back(rect);
			}
		}

	}

	return true;
}

/// <summary>
///  Agent Tree test
/// </summary>

struct TNode {
	size_t					num_nodes = 0;
	size_t					size_in_bytes = 0;

	TLFAgent				agent;

	std::unique_ptr<TNode>	left;
	std::unique_ptr<TNode>	right;


	void Detect(std::shared_ptr<TLFImage> img, const std::vector<TLFRect>* rois = nullptr) {
		TimeDiff	td;
		agent::TDetections		detections;

		auto items = agent.Detect(img, rois);

		std::cout << "Agent " << num_nodes << " detected " << items.size() << " items for " << td.GetDiffMs() << " ms." << std::endl;

		if (items.size()) {

			for (const auto& item : items) {
				auto rect = item.detected.GetBounds();
				// Increase size of rect;
				//rect.Inflate(rect.Width() * 0.1, rect.Height() * 0.1);
				detections.push_back(rect);
			}

			if (left)
				left->Detect(img, &detections);

			if (right)
				right->Detect(img, &detections);
		}
	}
};

static std::unique_ptr<TNode>	build_agents_tree(int levels, float activate_prob = 0.65f, int num_features = 10) {
	if (levels <= 0)
		return nullptr;

	auto root = std::make_unique<TNode>();

	auto detector_base = build_random_detector(activate_prob, num_features);

	root->size_in_bytes = calc_detector_size(detector_base.get()) + sizeof(TLFAgent);

	auto detector = agent::CreateCpuDetector();
	if (!detector->Initialize(std::move(detector_base))) {
		std::cout << "Cant initialize TStagesDetector" << std::endl;
	}

	detector->SetMinStages(0);

	root->agent.Initialize(std::move(detector), nullptr);
	root->agent.SetNmsThreshold(0.5);

	root->left = build_agents_tree(levels - 1, activate_prob, num_features);
	root->right = build_agents_tree(levels - 1, activate_prob, num_features);

	if (root->left) {
		root->num_nodes += root->left->num_nodes;
		root->size_in_bytes += root->left->size_in_bytes;
	}

	if (root->right) {
		root->num_nodes += root->right->num_nodes;
		root->size_in_bytes += root->right->size_in_bytes;
	}

	root->num_nodes++;

	return std::move(root);
}


static bool test_agents_tree() {
	// Make random HD image
	auto img = tests::create_empty_image(1280, 720, true);
	//auto img = create_empty_image(320, 240, true);

	std::cout << "Input image size " << img->width() << "x" << img->height() << std::endl;

	auto root = build_agents_tree(13, 0.65);

	std::cout << "### Agents count " << root->num_nodes << " ###" << std::endl;
	std::cout << "### Agents size " << root->size_in_bytes / double(1024 * 1024) << " mb. ###" << std::endl;

	TimeDiff	td_total(true);

	root->Detect(img);

	std::cout << std::endl << "### Total running time " << td_total.GetDiffMs() << " ms. ###" << std::endl << std::endl;

	return true;
}

static bool test_cpu_gpu() {
	// Make random HD image
	auto img = tests::create_empty_image(1280, 720, true);
	//auto img = create_empty_image(1920, 1080, true);
	//auto img = create_empty_image(320, 240, true);

	TLFAgent		cpu_agent;
	TLFAgent		gpu_agent;

	auto detector_base_cpu = build_random_detector(0.7, 10, 2);
	auto detector_base_gpu = copy_detector(detector_base_cpu.get());

	auto cpu_detector = agent::CreateCpuDetector();
	if (!cpu_detector->Initialize(std::move(detector_base_cpu))) {
		std::cout << "Cant initialize TStagesDetector" << std::endl;
	}

	cpu_detector->SetMinStages(0);

	cpu_agent.Initialize(std::move(cpu_detector), nullptr);
	cpu_agent.SetNmsThreshold(0.7);

	{
		TimeDiff	td;
		auto items = cpu_agent.Detect(img);
		std::cout << std::endl << "### CPU Agent detected " << items.size() << " items for " << td.GetDiffMs() << " ms. ###" << std::endl << std::endl;
	}


	auto gpu_detector = agent::CreateGpuDetector();
	if (!gpu_detector->Initialize(std::move(detector_base_gpu))) {
		std::cout << "Cant initialize TAccelStagesDetector" << std::endl;
	}

	gpu_detector->SetMinStages(0);

	gpu_agent.Initialize(std::move(gpu_detector), nullptr);
	gpu_agent.SetNmsThreshold(0.7);
	gpu_agent.SetBatchSize(16192);
	gpu_agent.SetMaxThreads(8);

	{
		TimeDiff	td;
		const int tests = 10;

		std::vector<TLFAgent::item_t>	items;
		for (int i = 0; i < tests; ++i)
			items = gpu_agent.Detect(img);

		std::cout << std::endl << "### GPU Agent detected " << items.size() << " items for " << td.GetDiffMs() / tests << " ms. ###" << std::endl << std::endl;
	}

	return true;
}


static bool test_trainer() {

	int rows = 100;
	int cols = 24;

	TMatrix tp_matrix(rows, cols);
	TMatrix fp_matrix(rows, cols);


	for (int i = 0; i < rows; ++i) {

		for (int j = 0; j < cols; ++j) {
			tp_matrix.GetRow(i)[j] = tests::GetRand(0, 256);
			fp_matrix.GetRow(i)[j] = tests::GetRand(0, 256);
		}
	}


	auto corrector = agent::CreateBaselineTrainer("create_class_corr1.py", "");

	TimeDiff	td;
	int count = 10;
	for (int i = 0; i < count; i++)
		corrector->TrainFpCorrector(fp_matrix, tp_matrix);
	std::cout << "### Corrector full time " << td.GetDiffMs() / count << " ms. ###" << std::endl;
	return true;
}


int main(int argc, char* argv[]) {

    
    // Using maximum agents for time/memory consuming
    std::cout << "[1] Agents time/memory usage test... " << std::endl;
	std::cout << "================================================================================" << std::endl << std::endl;
    if (!test_agents_tree())
        std::cout << "Tree test NOT PASSED " << std::endl;
    else {
        std::cout << "Tree test PASSED " << std::endl;
    }

    std::cout << std::endl;

    // GPU/CPU usage
    std::cout << "[2] GPU/CPU time usage test... " << std::endl;
	std::cout << "================================================================================" << std::endl << std::endl;
    if (!test_cpu_gpu())
        std::cout << "GPU/CPU test NOT PASSED " << std::endl;
    else {
        std::cout << "GPU/CPU test PASSED " << std::endl;
    }

    std::cout << std::endl;

    // Correctors building
    std::cout << "[3] Corrector building test for 100 samples... " << std::endl;
	std::cout << "================================================================================" << std::endl << std::endl;
    if (!test_trainer())
        std::cout << "Corrector test NOT PASSED " << std::endl;
    else {
        std::cout << "Corrector test PASSED " << std::endl;
    }

    std::cout << std::endl;


    return 0;

}

