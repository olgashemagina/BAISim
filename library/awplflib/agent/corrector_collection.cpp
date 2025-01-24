#include "corrector_collection.h"
#include "agent/correctors.h"

using namespace agent;

std::unique_ptr<ICorrector> agent::TCorrectorCollection::LoadCorrector(TiXmlElement* corrector_node) {
	std::string name = corrector_node->ValueStr();

	if (name == "TBaselineCorrector") {
		auto corrector = std::make_unique<TBaselineCorrector>();
		if (corrector->LoadXML(corrector_node))
			return corrector;
	}

	return nullptr;

}
