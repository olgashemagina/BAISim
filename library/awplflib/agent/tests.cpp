#include "tests.h"
#include "agent.h"

using namespace agent;

std::unique_ptr<IWorker> TRandomDetector::CreateWorker() {
	return std::make_unique<TRandomWorker>(this);
}

