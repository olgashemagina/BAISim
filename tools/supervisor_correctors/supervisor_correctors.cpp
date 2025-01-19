#include <iostream>
#include "LFAgent.h"

// supervisor_correctors.exe path_to_xml path_to_jpeg
int main(int argc, char* argv[]) {
    if (argc != 3) {
        std::cout << "Usage: supervisor_correctors.exe path_to_xml path_to_jpeg" << std::endl;
        return -1;
    }

    std::string path_to_xml = argv[1];
    std::string path_to_image = argv[2];

    auto agent = agent::CreateAgent();
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
    agent->Detect(img);
    return 0;
}

