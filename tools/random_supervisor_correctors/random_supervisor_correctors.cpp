#include <iostream>
#include <fstream>

#include "LFAgent.h"
#include "agent/tests.h"
#include "agent/detectors.h"
#include "agent/corrector_trainer.h"
#include "agent/supervisors.h"

std::unique_ptr<TLFAgent> build_random_agent() {
    auto detector = std::make_unique<agent::TRandomDetector>();

    // TODO: remove
    detector->Initialize();

    auto trainer = std::make_unique<agent::TCorrectorTrainerBase>();

    auto sv = std::make_shared<agent::TRandomSupervisor>();

    std::unique_ptr<TLFAgent>  agent = std::make_unique<TLFAgent>();
    agent->Initialize(std::move(detector), std::move(trainer));
    agent->SetSupervisor(sv);
    return std::move(agent);
}

std::unique_ptr<TLFAgent> load_xml(const std::string& fn)
{
    TiXmlDocument doc;

    TiXmlElement* node = nullptr;

    if (doc.LoadFile(fn, TIXML_ENCODING_UTF8)) {
        TiXmlHandle handle(&doc);
        node = handle.FirstChildElement().Element();

        std::unique_ptr<TLFAgent>  agent = std::make_unique<TLFAgent>();
        if (agent->LoadXML(node))
            return agent;
    }
    return nullptr;
}


void make_serialization_tests() {
    auto agent = build_random_agent();

    save_xml("test_agent.xml", agent->SaveXML());

    auto agent_loaded = load_xml("test_agent.xml");
        
    save_xml("test_agent_2.xml", agent_loaded->SaveXML());

}


int test(int argc, char* argv[]) {
    std::string db_folder = "d:/work/AI/testsupervisors/correct";
    std::string det_path = "d:/work/AI/testsupervisors/megapolis_2062im.xml";

    //db_folder = "D:\\Projects\\iap_ras\\BAISim\\dataset\\0\\source";
    //det_path = "D:\\Projects\\iap_ras\\BAISim\\models\\digits\\0_075_v3.xml";

    auto agent = LoadAgentFromEngine(det_path);

    save_xml("digit_0_agent.xml", agent->SaveXML());

    agent = load_xml("digit_0_agent.xml", [](TiXmlElement* node) {
        auto  agent = std::make_unique<TLFAgent>();
        if (node && agent->LoadXML(node))
            return agent;
        return std::unique_ptr<TLFAgent>{};
        });

    auto sv = std::make_shared<agent::TDBSupervisor>();
    int count = sv->LoadDB(db_folder);
    if (count <= 0) {
        std::cerr << "LoadDB return ERROR: " << count << std::endl;
        return count;
    }

    agent->SetSupervisor(sv);

    for (int i = 0; i < count; i++) {
        std::cout << "Processing " << i + 1 << " out of " << count << std::endl;
        std::shared_ptr<TLFImage> img = sv->LoadImg(i);
        agent->Detect(img);
    }
}
// supervisor_correctors.exe path_to_xml path_to_jpeg
int main(int argc, char* argv[]) {
    return test(argc, argv);

    if (!tests::make_serialization_tests()) {
        std::cout << "Serialization test ERROR " << std::endl;
    }
    else {
        std::cout << "Serialization test PASSED " << std::endl;
    }

    auto agent = tests::build_random_agent();



    if (argc != 3) {
        std::cout << "Usage: supervisor_correctors.exe path_to_xml path_to_jpeg" << std::endl;
        // Setup random image
        awpImage* image = nullptr;
        awpCreateImage(&image, 1280, 720, 3, AWP_BYTE);
        std::shared_ptr<TLFImage> img = std::make_shared<TLFImage>();
        img->SetImage(image);
        agent->Detect(img);
        return 0;
    }



    std::string path_to_xml = argv[1];
    std::string path_to_image = argv[2];

    //if (!agent->LoadXML(path_to_xml.c_str())) {
    //if (!agent->LoadXML(nullptr)) {
    //    std::cout << "LoadXML return false for " << path_to_xml << std::endl;
    //    return -2;
    //}
    std::shared_ptr<TLFImage> img = std::make_shared<TLFImage>();
    if (!img->LoadFromFile(path_to_image.c_str())) {
        std::cout << "LoadFromFile return false for " << path_to_image << std::endl;
        return -3;
    }

    for (int i = 0; i < 10; i++) {
        agent->Detect(img);
    }
    return 0;
}

