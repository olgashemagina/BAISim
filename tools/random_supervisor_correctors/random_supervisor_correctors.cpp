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


int main(int argc, char* argv[]) {

    auto agent = tests::build_random_agent();

    // Setup random image
    awpImage* image = nullptr;
    awpCreateImage(&image, 1280, 720, 3, AWP_BYTE);
    std::shared_ptr<TLFImage> img = std::make_shared<TLFImage>();
    img->SetImage(image);
    agent->Detect(img);
    return 0;

}

