#include <iostream>
#include <fstream>

#include "LFAgent.h"
#include "agent/tests.h"
#include "agent/detectors.h"
#include "agent/corrector_trainer.h"
#include "agent/supervisors.h"


int main(int argc, char* argv[]) {
    if (!tests::test_serialization()) {
        std::cout << "Serialization test ERROR " << std::endl;
    }
    else {
        std::cout << "Serialization test PASSED " << std::endl;
    }

    if (!tests::test_random_agent()) {
        std::cout << "Random agent test ERROR " << std::endl;
    }
    else {
        std::cout << "Random agent test PASSED " << std::endl;
    }

    if (!tests::test_import_detector("../../../../models/digits/0_075_v3.xml")) {
        std::cout << "Import test ERROR " << std::endl;
    }
    else {
        std::cout << "Import test PASSED " << std::endl;
    }
    
    if (!tests::test_build_correctors("\\"))
        std::cout << "Build corrector test ERROR " << std::endl;
    else {
        std::cout << "Build corrector test PASSED " << std::endl;
    }


    return 0;

}

