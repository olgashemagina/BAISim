#include <iostream>
#include "LFAgent.h"
#include "agent/tests.h"

// supervisor_correctors.exe path_to_xml path_to_jpeg
int main(int argc, char* argv[]) {

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

