#include "LFEngine.h"
#include "LFTree.h"
#include "LFZone.h"
#include "LFDetector.h"
#include "LFFileUtils.h"

#define BUFFER_SIZE     1000
#define DETECTORS_COUNT	5
#define ANGLE 15

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

std::shared_ptr<ILFObjectDetector> TLFTreeEngine::LoadDetector(TiXmlElement* node)
{
	if (node == NULL)
		return NULL;

	auto node_name = node->ValueStr();


	if (node_name == "TSCObjectDetector") {

		auto detector = std::make_shared<TSCObjectDetector>();
		//TiXmlElement* e = elem->FirstChildElement();
		if (!detector->LoadXML(node))
			return NULL;

		return detector;
	}
	return NULL;
}

std::shared_ptr<TLFAgent> TLFTreeEngine::LoadAgent(TiXmlElement* node) {
	if (node == NULL)
		return NULL;

	auto node_name = node->ValueStr();
	if (node_name == "TLFAgent") {
		auto agent = std::make_shared<TLFAgent>();
		if (!agent->LoadXML(node))
			return NULL;
		return agent;
	}
	return NULL;
}

bool TLFTreeEngine::SetAgent(const std::string& path, std::shared_ptr<TLFAgent> agent) {
	agent_tree_t* tree = &agent_tree_;
	auto splitted = SplitPath(path);

	auto agent_name = splitted.back();
	splitted.pop_back();

	for (const auto& name : splitted) {
		auto it = tree->find(name);
		if (it != tree->end()) {
			tree = &it->second.second;
		}
		else {
			//Cant find node in tree;
			return false;
		}

	}
	(*tree)[agent_name] = { std::move(agent), agent_tree_t{} };
	return true;
}

std::shared_ptr<TLFAgent> TLFTreeEngine::GetAgent(const std::string& path) const {
	agent_tree_t const* tree = &agent_tree_;
	std::shared_ptr<TLFAgent> agent;

	auto splitted = SplitPath(path);
	for (const auto& name : splitted) {
		auto it = tree->find(name);
		if (it != tree->end()) {
			tree = &it->second.second;
			agent = it->second.first;
		}
		else {
			//Cant find node in tree;
			return nullptr;
		}

	}

	return agent;
}


bool TLFTreeEngine::SetDetector(const std::string& path, std::shared_ptr<ILFObjectDetector> detector) {
	tree_t* tree = &tree_;
	auto splitted = SplitPath(path);

	auto detector_name = splitted.back();
	splitted.pop_back();

	for (const auto& name : splitted) {
		auto it = tree->find(name);
		if (it != tree->end()) {
			tree = &it->second.second;
		}
		else {
			//Cant find node in tree;
			return false;
		}

	}
	(*tree)[detector_name] = { std::move(detector), tree_t{} };
	return true;
}

std::shared_ptr<ILFObjectDetector> TLFTreeEngine::GetDetector(const std::string& path) const {
	tree_t const* tree = &tree_;
	std::shared_ptr<ILFObjectDetector> detector;

	auto splitted = SplitPath(path);
	for (const auto& name : splitted) {
		auto it = tree->find(name);
		if (it != tree->end()) {
			tree = &it->second.second;
			detector = it->second.first;
		}
		else {
			//Cant find node in tree;
			return nullptr;
		}

	}

	return detector;
}

std::vector<std::string> TLFTreeEngine::SplitPath(const std::string& s) {
	std::vector<std::string> result;
	std::stringstream ss(s);
	std::string item;

	while (std::getline(ss, item, '.')) {
		result.push_back(item);
	}

	return result;
}

std::optional<std::vector<detected_item_ptr>> TLFTreeEngine::DetectInRect(const tree_t& tree, TLFImage* image, awpRect* rect) {
	std::vector<detected_item_ptr>		detections;

	for (const auto& [name, node] : tree) {

		if (!node.first->Init(image, rect))
			//Cant initialize scanner or image;
			return std::nullopt;

		//TODO: add NMS
		//TODO: move Non Maximum Suppression to detector;
		if (node.first->Detect() > 0) {
			//Objects detected, run tree
			for (int i = 0; i < node.first->GetNumItems(); i++) {
				TLFDetectedItem* item = node.first->GetItem(i);
				auto leaf_rect = item->GetBounds().GetRect();

				auto parent = std::make_shared<TLFDetectedItem>(*item);

				detections.push_back(parent);

				auto detected = DetectInRect(node.second, image, &leaf_rect);

				if (!detected)
					return std::nullopt;

				for (auto child : *detected) {
					//TODO: move to TLFSemanticImageDescriptor
					if (!child->parent())
						child->set_parent(parent);

					detections.push_back(child);
				}

			}

		}
	}

	return detections;
}

bool TLFTreeEngine::Load(const std::string& filename) {
	FILE* file = fopen(filename.c_str(), "rb");
	if (!file)
	{
		printf("TLFTreeEngine::Load fopen failed!!!\n");
		return false;
	}
	TiXmlDocument doc;
	bool result = doc.LoadFile(file, TIXML_ENCODING_UTF8);
	fclose(file);
	if (!result)
	{
		printf("ILFDetectEngine::Load failed!!!\n");
		return false;
	}
	TiXmlHandle hDoc(&doc);
	TiXmlElement* pElem = NULL;
	TiXmlHandle hRoot(0);
	pElem = hDoc.FirstChildElement().Element();

	return this->LoadXML(pElem);
}

bool TLFTreeEngine::Save(const std::string& filename) {
	TiXmlDocument doc;
	TiXmlDeclaration* decl = new TiXmlDeclaration("1.0", "", "");
	doc.LinkEndChild(decl);
	TiXmlElement* engine = SaveXML();
	if (engine == NULL)
		return false;
	doc.LinkEndChild(engine);
	FILE* file = fopen(filename.c_str(), "w");
	if (!file)
	{
		printf("TLFTreeEngine::Save fopen failed!!!\n");
		return false;
	}
	bool result = doc.SaveFile(file);
	fclose(file);
	return result;
}

TiXmlElement* TLFTreeEngine::SaveXML(const agent_tree_t& tree, agents_t& agents) {
	TiXmlElement* xml_tree = new TiXmlElement("TLFTree");

	for (const auto& [name, node] : tree) {
		TiXmlElement* xml_node = new TiXmlElement("TLFTreeNode");
		xml_node->SetAttribute("name", name);

		auto agent_it = std::find(agents.begin(), agents.end(), node.first);

		if (agent_it != agents.end()) {
			xml_node->SetAttribute("agent_id", std::distance(agents.begin(), agent_it));
		}
		else {
			xml_node->SetAttribute("agent_id", agents.size());
			agents.push_back(node.first);
		}
		auto tree_elem = SaveXML(node.second, agents);

		xml_node->LinkEndChild(tree_elem);
		xml_tree->LinkEndChild(xml_node);
	}

	return xml_tree;

}

TLFTreeEngine::agent_tree_t TLFTreeEngine::LoadXML(TiXmlElement* node, const agents_t& agents) {
	agent_tree_t tree;
	for (TiXmlNode* child = node->FirstChild("TLFTreeNode"); child; child = child->NextSibling()) {

		auto elem = child->ToElement();

		if (elem) {
			int id = 0;
			elem->QueryIntAttribute("agent_id", &id);

			std::string name;

			elem->QueryValueAttribute("name", &name);

			if (agents.size() < id)
				return {};

			auto xml_tree = elem->FirstChild("TLFTree");

			agent_tree_t child_tree;

			if (xml_tree) {
				child_tree = LoadXML(xml_tree->ToElement(), agents);
			}

			tree.emplace(name, std::pair{ agents[id], std::move(child_tree) });

		}
	}
	return tree;
}

TiXmlElement* TLFTreeEngine::SaveXML(const tree_t& tree, detectors_t& detectors) {
	TiXmlElement* xml_tree = new TiXmlElement("TLFTree");

	for (const auto& [name, node] : tree) {
		TiXmlElement* xml_node = new TiXmlElement("TLFTreeNode");
		xml_node->SetAttribute("name", name);


		auto det_it = std::find(detectors.begin(), detectors.end(), node.first);

		if (det_it != detectors.end()) {
			xml_node->SetAttribute("detector_id", std::distance(detectors.begin(), det_it));
		}
		else {
			xml_node->SetAttribute("detector_id", detectors.size());
			detectors.push_back(node.first);
		}


		auto tree_elem = SaveXML(node.second, detectors);

		xml_node->LinkEndChild(tree_elem);
		xml_tree->LinkEndChild(xml_node);
	}

	return xml_tree;

}

TLFTreeEngine::tree_t TLFTreeEngine::LoadXML(TiXmlElement* node, const detectors_t& detectors) {

	tree_t tree;
	for (TiXmlNode* child = node->FirstChild("TLFTreeNode"); child; child = child->NextSibling()) {

		auto elem = child->ToElement();

		if (elem) {
			int id = 0;
			elem->QueryIntAttribute("detector_id", &id);

			std::string name;

			elem->QueryValueAttribute("name", &name);

			if (detectors.size() < id)
				return {};

			auto xml_tree = elem->FirstChild("TLFTree");

			tree_t child_tree;

			if (xml_tree) {
				child_tree = LoadXML(xml_tree->ToElement(), detectors);
			}

			tree.emplace(name, std::pair{ detectors[id], std::move(child_tree) });

		}
	}
	return tree;
}

bool TLFTreeEngine::LoadXML(TiXmlElement* parent) {
	printf("TLFTreeEngine: LoadXML.\n");

	try
	{
		if (parent == NULL)
			return false;

		if (parent->ValueStr() != GetName())
			return false;

		if (use_agent) {
			auto xml_agents = parent->FirstChild("TLFAgents");
			agents_t agents;
			if (xml_agents) {
				for (TiXmlNode* child = xml_agents->FirstChild(); child; child = child->NextSibling()) {

					auto agent = LoadAgent(child->ToElement());
					if (!agent)
						return false;

					agents.push_back(agent);
				}
			}
			auto xml_tree = parent->FirstChild("TLFTree");

			if (xml_tree) {
				agent_tree_ = LoadXML(xml_tree->ToElement(), agents);
			}

			if (agent_tree_.empty() && !agents.empty())
				return false;
		}
		else {
			auto xml_detectors = parent->FirstChild("TLFDetectors");

			detectors_t	detectors;

			if (xml_detectors) {

				for (TiXmlNode* child = xml_detectors->FirstChild(); child; child = child->NextSibling()) {

					auto detector = LoadDetector(child->ToElement());
					if (!detector)
						return false;

					detectors.push_back(detector);
				}
			}

			auto xml_tree = parent->FirstChild("TLFTree");

			if (xml_tree) {
				tree_ = LoadXML(xml_tree->ToElement(), detectors);
			}

			if (tree_.empty() && !detectors.empty())
				return false;
		}

	}
	catch (...)
	{
		printf("TLFDetectEngine: exeption while loading.");
		return false;
	}
	return true;
}




TiXmlElement* TLFTreeEngine::SaveXML() {
	TiXmlElement* parent = new TiXmlElement(GetName());

	if (use_agent) {
		agents_t agents;
		auto xml_tree = SaveXML(agent_tree_, agents);
		parent->LinkEndChild(xml_tree);
		TiXmlElement* xml_agents = new TiXmlElement("TLFAgents");

		for (const auto& agent : agents) {
			TiXmlElement* xml_agent = agent->SaveXML();
			xml_agents->LinkEndChild(xml_agent);
		}
		parent->LinkEndChild(xml_agents);
	}
	else {
		detectors_t		detectors;
		auto xml_tree = SaveXML(tree_, detectors);
		parent->LinkEndChild(xml_tree);
		TiXmlElement* xml_detectors = new TiXmlElement("TLFDetectors");

		for (const auto& det : detectors) {
			TiXmlElement* xml_det = det->SaveXML();
			xml_detectors->LinkEndChild(xml_det);
		}

		parent->LinkEndChild(xml_detectors);
	}
	return parent;
}

void TLFTreeEngine::SetAgentMode() {
	use_agent = true;
}
