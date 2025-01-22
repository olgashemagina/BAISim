#include <iostream>
#include "LFAgent.h"
#include "agent/tests.h"
#include "agent/detectors.h"
#include "agent/supervisors.h"

int test(int argc, char* argv[]) {
    std::string db_folder = "d:/work/AI/testsupervisors/correct";
    std::string det_path = "d:/work/AI/testsupervisors/megapolis_2062im.xml";
    auto sv = std::make_shared<agent::TDBSupervisor>();
    int count = sv->LoadDB(det_path, db_folder);
    if (count <= 0) {
        std::cerr << "LoadDB return ERROR: " << count << std::endl;
        return count;
    }

    agent::TRandomAgent agent;
    if (!agent.Initialize())
        return -1;

    agent.SetSupervisor(sv);

    for (int i = 0; i < count; i++) {
        std::cout << "Processing " << i + 1 << " out of " << count << std::endl;
        std::shared_ptr<TLFImage> img = sv->LoadImg(i);
        agent.Detect(img);
    }

    // mozhet bit neskolko
    //<TLFDetectedItem DetectorName="Human marked" Raiting="0.000000" Type="447FE06D-7393-42C5-8497-2D25BC8BCA6A" Angle="0" Racurs="0" Bw="24" Bh="12" left="163" top="192" right="1763" bottom="992" Comment="">
}

// supervisor_correctors.exe path_to_xml path_to_jpeg
int main(int argc, char* argv[]) {
    return test(argc, argv);

    agent::TRandomAgent agent;
    if (!agent.Initialize())
        return -1;

    auto sv = std::make_shared<agent::TRandomSupervisor>();
    agent.SetSupervisor(sv);

    if (argc != 3) {
        std::cout << "Usage: supervisor_correctors.exe path_to_xml path_to_jpeg" << std::endl;
        // Setup random image
        awpImage* image = nullptr;
        awpCreateImage(&image, 1280, 720, 3, AWP_BYTE);
        std::shared_ptr<TLFImage> img = std::make_shared<TLFImage>();
        img->SetImage(image);       
        agent.Detect(img);
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
        agent.Detect(img);
    }
    return 0;
}

