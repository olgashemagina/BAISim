#include <iostream>
#include <fstream>

#include "LFAgent.h"
#include "LFEngine.h"

#include "agent/detectors.h"
#include "agent/corrector_trainer.h"
#include "agent/supervisors.h"
#include "utils/xml.h"

void usage()
{
    std::cout << "Supervisor correctiors v1.0" << std::endl <<
        "Usage:" << std::endl << "<supervisor_correctors.exe>  train|test " <<
        "--det=<path_to_detector> or --agent=<path_to_agent> " <<
        "--save_path=<path_to_save_agent> " <<
        "--db_folder=<path_to_img_directory> " << std::endl;
    std::cout << "\t--det\t\tPath to XML with detector (TLFDetectEngine)" << std::endl;
    std::cout << "\t--agent\t\tPath to XML with agent" << std::endl <<
        "\t--db_folder\t\tPath to directory with images" << std::endl;
    std::cout << "\t--save_path\t\tSave agent to XML path" << std::endl;
    
}


int main(int argc, char* argv[]) {
    if (argc < 5) {
        usage();
        return -1;
    }
    TLFString mode = argv[1];
    TLFString det_path;
    TLFString agent_path;
    TLFString save_path;
    TLFString db_folder_path;

    for (int i = 2; i < argc; i++) {
        TLFString key(argv[i]);
        if (key.substr(0, 6) == "--help" || key.substr(0, 2) == "-h") {
            usage();
            return 0;
        }
        if (key.substr(0, 5) == "--det") {
            if (key.length() < 7 || key[5] != '=') {
                std::cerr << "Parsing error: use \"det=<path>\", no extra spaces" << std::endl;
                usage();
                return -3;
            }
            if (!det_path.empty()) {
                std::cerr << "Engine file was already specified in parameters" << std::endl;
                usage();
                return -4;
            }
            det_path = key.substr(6);
        }
        else if (key.substr(0, 11) == "--db_folder") {
            if (key.length() < 13 || key[11] != '=') {
                std::cerr << "Parsing error: use \"db_folder=<path>\", no extra spaces" << std::endl;
                usage();
                return -4;
            }
            if (db_folder_path.length() > 0) {
                std::cerr << "Database folder was already specified in parameters" << std::endl;
                usage();
                return -4;
            }
            db_folder_path = key.substr(12);
            std::cout << "Using database folder: " << db_folder_path << std::endl;
        }
        else if (key.substr(0, 7) == "--agent") {
            if (key.length() < 9 || key[7] != '=') {
                std::cerr << "Parsing error: use \"agent=<path>\", no extra spaces" << std::endl;
                usage();
                return -4;
            }
            if (agent_path.length() > 0) {
                std::cerr << "Database folder was already specified in parameters" << std::endl;
                usage();
                return -4;
            }
            agent_path = key.substr(8);
            std::cout << "Using database folder: " << agent_path << std::endl;
        }
        else if (key.substr(0, 11) == "--save_path") {
            if (key.length() < 13 || key[11] != '=') {
                std::cerr << "Parsing error: use \"save_path=<path>\", no extra spaces" << std::endl;
                usage();
                return -4;
            }
            if (save_path.length() > 0) {
                std::cerr << "Database folder was already specified in parameters" << std::endl;
                usage();
                return -4;
            }
            save_path = key.substr(12);
            std::cout << "Using database folder: " << save_path << std::endl;
        }
        else {
            std::cerr << "Unknown key: " << key << std::endl;
            usage();
            return -2;
        }
    }

    std::unique_ptr<TLFAgent> agent;
    if (!agent_path.empty()) {
        agent = std::make_unique<TLFAgent>();
        agent = load_xml(agent_path, [](TiXmlElement* node) {
            auto  agent = std::make_unique<TLFAgent>();
            if (node && agent->LoadXML(node))
                return agent;
            return std::unique_ptr<TLFAgent>{};
            });
        assert(agent);
    }
    else if (!det_path.empty()) {
        agent = LoadAgentFromEngine(det_path);
    }
    else {
        usage();
        return -20;
    }

    auto sv = std::make_shared<agent::TDBSupervisor>();
    int count = sv->LoadDB(db_folder_path);
    if (count <= 0) {
        std::cerr << "LoadDB return ERROR: " << count << std::endl;
        return count;
    }
    if (mode == "train") {
    auto sv = std::make_shared<agent::TDBSupervisor>();
    int count = sv->LoadDB(db_folder_path);
    if (count <= 0) {
        std::cerr << "LoadDB return ERROR: " << count << std::endl;
        return count;
    }
    if (mode == "train")
        agent->SetSupervisor(sv);
    }
    else if (mode == "test") {
        //todo
    }
    else {
        usage();
        return -10;
    }

    float overlap = 0.4f;

    // False Posititive, False Negative, True Positive
    int FP = 0, FN = 0, TP = 0;

    for (int i = 0; i < count; i++) {
        std::cout << "Processing " << i + 1 << " out of " << count << std::endl;
        std::shared_ptr<TLFImage> img = sv->LoadImg(i);
        auto dets = agent->Detect(img);

        auto gt = sv->Detect(img);

        auto [fp, fn] = CalcStat(gt, dets, overlap);

        FP += fp;
        FN += fn;
        TP += (gt.size() + dets.size() - fp - fn) / 2;
    }

    float precision = TP / float(TP + FP);
    float recall = TP / float(FN + TP);

    std::cout << "Overall Precision=" << precision << " Recall=" << recall << std::endl;

    if (!save_path.empty()) {
        save_xml(save_path, agent->SaveXML());
    }

    return 0;
}

