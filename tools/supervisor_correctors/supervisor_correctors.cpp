#include <iostream>
#include <fstream>

#include "LFAgent.h"
#include "LFEngine.h"

#include "agent/detectors.h"
#include "agent/corrector_trainer.h"
#include "agent/supervisors.h"


int main(int argc, char* argv[]) {

    // TODO: MODES [train, test]

    if (argc != 3) {
        std::cout << "Usage: supervisor_correctors path_to_db_folder detector_path" << std::endl;
        return -1;
    }

    float overlap = 0.4f;

    std::string db_folder = argv[1];
    std::string det_path = argv[2];

    auto agent = LoadAgentFromEngine(det_path);

    auto sv = std::make_shared<agent::TDBSupervisor>();
    int count = sv->LoadDB(db_folder);
    if (count <= 0) {
        std::cerr << "LoadDB return ERROR: " << count << std::endl;
        return count;
    }

    agent->SetSupervisor(sv);

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


    return 0;
}

