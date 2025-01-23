#include <iostream>
#include <fstream>

#include "LFAgent.h"
#include "LFEngine.h"

#include "agent/tests.h"
#include "agent/detectors.h"
#include "agent/corrector_trainer.h"
#include "agent/supervisors.h"


int main(int argc, char* argv[]) {
    if (argc != 3) {
        std::cout << "Usage: supervisor_correctors.exe path_to_db_folder detector_path" << std::endl;
        return -1;
    }

    std::string db_folder = argv[1];
    std::string det_path = argv[2];

    if (!tests::make_serialization_tests()) {
        std::cout << "Serialization test ERROR " << std::endl;
    }
    else {
        std::cout << "Serialization test PASSED " << std::endl;
    }

    db_folder = "d:/work/AI/testsupervisors/correct";
    det_path = "d:/work/AI/testsupervisors/megapolis_2062im.xml";

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

    return 0;
}

