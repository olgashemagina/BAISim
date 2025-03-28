#include <iostream>
#include <fstream>

#include "LFAgent.h"
#include "LFEngine.h"
#include "LFFileUtils.h"

#include "agent/detectors.h"
#include "agent/corrector_trainer.h"
#include "agent/supervisors.h"
#include "utils/xml.h"
#include "utils/time.h"



void usage()
{
    std::cout << "Supervisor correctiors v1.0" << std::endl <<
        "Usage:" << std::endl << "<supervisor_correctors.exe>  train|test " <<
        "--det=<path_to_detector>, --det_gpu=<path_to_detector> or --agent=<path_to_agent> " <<
        "--save_path=<path_to_save_agent> " <<
        "--db_folder=<path_to_img_directory> " << std::endl;
    std::cout << "\t--det\t\tPath to XML with detector (TLFDetectEngine)" << std::endl;
    std::cout << "\t--agent\t\tPath to XML with agent" << std::endl <<
        "\t--db_folder\t\tPath to directory with images" << std::endl;
    std::cout << "\t--save_path\t\tSave agent to XML path" << std::endl;
    std::cout << "\t--overlap\t\tOverlap for precision/recall {0, 1}, default = 0.5" << std::endl;
    std::cout << "\t--corrs_folder\t\tDescriptions of corrections for each image." << std::endl;
    
}


int main(int argc, char* argv[]) {
    
    if (argc < 4) {
        usage();
        return -1;
    }
    TLFString mode = argv[1];
    TLFString det_path;
    TLFString agent_path;
    TLFString save_path;
    TLFString corr_folder;
    TLFString db_folder_path;
    bool use_gpu = false;
    float overlap = 0.5;

    for (int i = 2; i < argc; i++) {
        TLFString key(argv[i]);
        if (key.substr(0, 6) == "--help" || key.substr(0, 2) == "-h") {
            usage();
            return 0;
        }
        if (key.substr(0, 9) == "--det_gpu") {
            if (key.length() < 11 || key[9] != '=') {
                std::cerr << "Parsing error: use \"det_gpu=<path>\", no extra spaces" << std::endl;
                usage();
                return -3;
            }
            if (!det_path.empty()) {
                std::cerr << "Engine file was already specified in parameters" << std::endl;
                usage();
                return -4;
            }
            det_path = key.substr(10);
            use_gpu = true;
            std::cout << "Using GPU detector: " << det_path << std::endl;
        } else if (key.substr(0, 5) == "--det") {
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

            std::cout << "Using detector: " << det_path << std::endl;
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
                std::cerr << "Agent path was already specified in parameters" << std::endl;
                usage();
                return -4;
            }
            agent_path = key.substr(8);
            std::cout << "Using agent path: " << agent_path << std::endl;
        }
        else if (key.substr(0, 11) == "--save_path") {
            if (key.length() < 13 || key[11] != '=') {
                std::cerr << "Parsing error: use \"save_path=<path>\", no extra spaces" << std::endl;
                usage();
                return -4;
            }
            if (save_path.length() > 0) {
                std::cerr << "Save path was already specified in parameters" << std::endl;
                usage();
                return -4;
            }
            save_path = key.substr(12);
            std::cout << "Using save path: " << save_path << std::endl;
        }
        else if (key.substr(0, 9) == "--overlap") {
            if (key.length() < 11 || key[9] != '=') {
                std::cerr << "Parsing error: use \"overlap=0.5\", no extra spaces" << std::endl;
                usage();
                return -4;
            }
            
            overlap = std::stof(key.substr(10));
            std::cout << "Using overlap for statistics: " << overlap << std::endl;
        }
        else if (key.substr(0, 14) == "--corrs_folder") {
            if (key.length() < 16 || key[14] != '=') {
                std::cerr << "Parsing error: use \"corrs_folder=<path>\", no extra spaces" << std::endl;
                usage();
                return -4;
            }

            corr_folder = key.substr(15);

            if (LFCreateDirs(corr_folder.c_str())) {

                std::cout << "Using corrs_folder: " << corr_folder << std::endl;
            }
            else {
                std::cout << "Cant using corrs_folder: " << corr_folder << std::endl;
                return -1;
            }
        }
        else {
            std::cerr << "Unknown key: " << key << std::endl;
            usage();
            return -2;
        }
    }

    std::unique_ptr<TLFAgent> agent;
    if (!agent_path.empty()) {
        agent = load_xml(agent_path, [](TiXmlElement* node) {
            auto  agent = std::make_unique<TLFAgent>();
            if (node && agent->LoadXML(node))
                return agent;
            return std::unique_ptr<TLFAgent>{};
            });
        
        if (!agent) {
            std::cout << "Cant load agent " << agent_path << std::endl;
            return -9;
        }
    }
    else if (!det_path.empty()) {
        agent = LoadAgentFromEngine(det_path, use_gpu);
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
        agent->SetSupervisor(sv);
    }
    else if (mode != "test") {
        usage();
        return -10;
    }

    
    // False Posititive, False Negative, True Positive
    int FP = 0, FN = 0, TP = 0;

    for (int i = 0; i < count; i++) {
        std::cout << "Processing " << i + 1 << " out of " << count << std::endl;
        std::shared_ptr<TLFImage> img = sv->LoadImg(i);
        TimeDiff td;
        auto dets = agent->Detect(img);

        std::cout << "Detecting time " << td.GetDiffMs() << " milliseconds" << std::endl;
        
        auto gt = sv->Detect(img);

        if (!corr_folder.empty()) {
            auto desc = CreateCorrectedDescription(dets, img->width(), img->height());

            if (desc) {
                auto file_name = LFGetFileName(sv->GetImageName(i));
                auto path = LFMakeFileName(corr_folder, file_name, ".xml");
                if (!save_xml(path, desc->SaveXML())) {
                    std::cout << "Cant save description to " << path << std::endl;
                }
            
            }
        }

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

