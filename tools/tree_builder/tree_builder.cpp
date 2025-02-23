#pragma hdrstop
#pragma argsused

#ifdef _WIN32
#include <tchar.h>
#else
  typedef char _TCHAR;
  #define _tmain main
#endif

#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <numeric>

#include "tinyxml.h"

#include "utils/xml.h"

#include "LF.h"
#include "LFEngine.h"
#include "LFFileUtils.h"

#include <memory>
#include <mutex>

//merge --tree="../../../../models/railway/rw_tree.xml" --det="../../../../models/railway/rtank_1224.xml" --tree_path="RailwayTank" --tree_path="RailwayTank2"

//export --tree="../../../../models/railway/rw_tree.xml" --det="../../../../models/railway/rtank_exported.xml" --tree_path="RailwayTank"

  //merge --tree="../../../../models/railway/rw_tree.xml" --det="../../../../models/railway/rtank_1224.xml" --tree_path="RailwayTank"
  //merge --tree="../../../../models/railway/rw_tree.xml" --det="../../../../models/railway/Rtank_Num_Digits/rnum_cs.xml" --tree_path="RailwayTank.Numbers"
  //merge --tree="../../../../models/railway/rw_tree.xml" --det="../../../../models/railway/Rtank_Num_Digits/0_075_v3.xml" --tree_path="RailwayTank.Numbers.Digit0"
  //merge --tree="../../../../models/railway/rw_tree.xml" --det="../../../../models/railway/Rtank_Num_Digits/1_075_v3.xml" --tree_path="RailwayTank.Numbers.Digit1"
  //merge --tree="../../../../models/railway/rw_tree.xml" --det="../../../../models/railway/Rtank_Num_Digits/2_075_v3.xml" --tree_path="RailwayTank.Numbers.Digit2"
  //merge --tree="../../../../models/railway/rw_tree.xml" --det="../../../../models/railway/Rtank_Num_Digits/3_075_v3.xml" --tree_path="RailwayTank.Numbers.Digit3"
  //merge --tree="../../../../models/railway/rw_tree.xml" --det="../../../../models/railway/Rtank_Num_Digits/4_075_v3.xml" --tree_path="RailwayTank.Numbers.Digit4"
  //merge --tree="../../../../models/railway/rw_tree.xml" --det="../../../../models/railway/Rtank_Num_Digits/5_075_v3.xml" --tree_path="RailwayTank.Numbers.Digit5"
  //merge --tree="../../../../models/railway/rw_tree.xml" --det="../../../../models/railway/Rtank_Num_Digits/6_075_v3.xml" --tree_path="RailwayTank.Numbers.Digit6"
  //merge --tree="../../../../models/railway/rw_tree.xml" --det="../../../../models/railway/Rtank_Num_Digits/7_075_v3.xml" --tree_path="RailwayTank.Numbers.Digit7"
  //merge --tree="../../../../models/railway/rw_tree.xml" --det="../../../../models/railway/Rtank_Num_Digits/8_075_v3.xml" --tree_path="RailwayTank.Numbers.Digit8"
  //merge --tree="../../../../models/railway/rw_tree.xml" --det="../../../../models/railway/Rtank_Num_Digits/9_075_v3.xml" --tree_path="RailwayTank.Numbers.Digit9"

// test agent
//merge --tree="../../../../models/agent/agent_tree.xml" --agent="../../../../models/agent/random_agent.xml" --tree_path="RandomAgent"
//merge --tree="../../../../models/agent/agent_tree.xml" --agent="../../../../models/agent/random_agent2.xml" --tree_path="RandomAgent.Agent2"
//merge --tree="../../../../models/agent/agent_tree.xml" --agent="../../../../models/agent/random_agent3.xml" --tree_path="RandomAgent.Agent3"
//export --tree="../../../../models/agent/agent_tree.xml" --agent="../../../../models/agent/agent2_exported.xml" --tree_path="RandomAgent.Agent2"
//export --tree="../../../../models/agent/agent_tree.xml" --agent="../../../../models/agent/agent3_exported.xml" --tree_path="RandomAgent.Agent3"

enum EStatus {
	kStatus_Success = 0,
	kStatus_WrongPath,
	kStatus_WrongArgument,

};


void usage()
{
	std::cout << "Tree Builder v1.0" << std::endl <<
		"Usage:" << std::endl << "<tree_builder.exe> " <<
		"export|merge  " <<
		"--tree=<path_to_tree_engine> " <<
		"--det=<path_to_detector>|--agent=<path_to_agent> " <<
		"--tree_path=<path_in_tree> [--tree_path=<path_in_tree> ...] " <<
		std::endl;

	std::cout << "\t--tree\t\tPath to XML with Tree Engine (TLFTreeEngine)" <<
		std::endl << "\t--det\t\tPath to XML with Engine (TLFDetectEngine)" <<
		std::endl << "\t--agent\t\tPath to XML with Agent (TLFAgent)" <<
		std::endl << "\t--tree_path\t\tPath in Tree Engine as Name1.Name2..." <<
		std::endl << std::endl;
}

static EStatus merge_agent(const TLFString& tree_path, const TLFString& agent_path,
	const std::vector<TLFString>& tree_path_det) {

	TLFTreeEngine tree_engine;
	tree_engine.SetAgentMode();
	if (!tree_engine.Load(tree_path)) {
		std::cerr << "Cant load Tree Engine. Creating empty one." << std::endl;
	}

	std::shared_ptr<TLFAgent> agent;
	if (!agent_path.empty()) {
		agent = std::make_shared<TLFAgent>();
		agent = load_xml(agent_path, [](TiXmlElement* node) {
			auto agent = std::make_shared<TLFAgent>();
			if (node && agent->LoadXML(node))
				return agent;
			return std::shared_ptr<TLFAgent>{};
			});
		assert(agent);
		if (!agent) {
			std::cerr << "TLFAgent couldn't parse file" <<
				agent_path << std::endl;
			return kStatus_WrongPath;
		}
	}
	else {
		std::cerr << "agent path is empty" <<
			agent_path << std::endl;
		return kStatus_WrongPath;
	}

	for (const auto& path : tree_path_det) {
		if (!tree_engine.SetAgent(path, agent))
			return kStatus_WrongPath;
	}

	if (!tree_engine.Save(tree_path))
		return kStatus_WrongPath;

	return kStatus_Success;
}

static EStatus export_agent(const TLFString& tree_path, const TLFString& agent_path,
	const TLFString& tree_path_det) {

	auto tree_engine = std::make_unique<TLFTreeEngine>();
	tree_engine->SetAgentMode();
	if (!tree_engine->Load(tree_path))
		return kStatus_WrongPath;


	auto agent = tree_engine->GetAgent(tree_path_det);

	if (!agent) {
		std::cerr << "TLFTreeEngine cant find path in tree " <<
			tree_path_det << std::endl;
		return kStatus_WrongPath;
	}

	tree_engine.reset();

	if (!save_xml(agent_path, agent->SaveXML())) {
		std::cerr << "TLFAgent couldn't write file" <<
			agent_path << std::endl;
		return kStatus_WrongPath;
	}

	return kStatus_Success;
}

static EStatus merge_detector(const TLFString& tree_path, const TLFString& det_path,
	const std::vector<TLFString>& tree_path_det) {

	TLFTreeEngine			tree_engine;

	if (!tree_engine.Load(tree_path)){
		std::cerr << "Cant load Tree Engine. Creating empty one." << std::endl;
	}

	std::shared_ptr<TLFDetectEngine> det = std::make_shared<TLFDetectEngine>();
	if (!det->Load(det_path.c_str())) {
		std::cerr << "TLFDetectEngine couldn't parse file" <<
			det_path << std::endl;
		return kStatus_WrongPath;
	}

	for (const auto& path : tree_path_det) {
		if (!tree_engine.SetDetector(path, std::shared_ptr<ILFObjectDetector>(det, det->GetDetector(0))))
			return kStatus_WrongPath;
	}

	if (!tree_engine.Save(tree_path))
		return kStatus_WrongPath;

	return kStatus_Success;
}

static EStatus export_detector(const TLFString& tree_path, const TLFString& det_path,
	const TLFString& tree_path_det) {

	auto tree_engine = std::make_unique<TLFTreeEngine>();

	if (!tree_engine->Load(tree_path))
		return kStatus_WrongPath;


	auto det = tree_engine->GetDetector(tree_path_det);

	if (!det) {
		std::cerr << "TLFTreeEngine cant find path in tree " <<
			tree_path_det << std::endl;
		return kStatus_WrongPath;
	}

	//Delete engine to hold pointer to detector det;
	tree_engine.reset();

	auto det_engine = std::make_unique<TLFDetectEngine>();


	det_engine->AddDetector(det.get());
		
	if (!det_engine->Save(det_path.c_str())) {
		std::cerr << "TLFDetectEngine couldn't write file" <<
			det_path << std::endl;
		return kStatus_WrongPath;
	}

	det_engine->RemoveDetector(0);

	return kStatus_Success;
}


int main(int argc, char* argv[])
{
	TLFString det_path;
	TLFString agent_path;
	bool use_agent = false;
	TLFString tree_path;
	std::vector<TLFString> tree_path_det;

	if (argc < 3) {
		usage();
		return -1;
	}

	TLFString command(argv[1]);
	// argv[0] is the launcher command, not parameter
	for (int i = 2; i < argc; i++) {
		TLFString key(argv[i]);
		if (key.substr(0, 6) == "--help" || key.substr(0, 2) == "-h") {
			usage();
			return kStatus_Success;
		}
		if (key.substr(0, 6) == "--det=") {
			if (key.length() < 7 || key[5] != '=') {
				std::cerr << "Parsing error: use \"det=<path>\", "
					"no extra spaces" << std::endl;
				usage();
				return kStatus_WrongArgument;
			}
			if (!agent_path.empty()) {
				std::cerr << "agent path was already specified "
					"in parameters" << std::endl;
				usage();
				return kStatus_WrongArgument;
			}
			if (!det_path.empty()) {
				std::cerr << "Engine file was already specified "
					"in parameters" << std::endl;
				usage();
				return kStatus_WrongArgument;
			}
			det_path = key.substr(6);
		} else if (key.substr(0, 8) == "--agent=") {
			use_agent = true;
			if (key.length() < 9 || key[7] != '=') {
				std::cerr << "Parsing error: use \"agent=<path>\", "
					"no extra spaces" << std::endl;
				usage();
				return kStatus_WrongArgument;
			}
			if (!det_path.empty()) {
				std::cerr << "det path was already specified "
					"in parameters" << std::endl;
				usage();
				return kStatus_WrongArgument;
			}
			if (!agent_path.empty()) {
				std::cerr << "Engine file was already specified "
					"in parameters" << std::endl;
				usage();
				return kStatus_WrongArgument;
			}
			agent_path = key.substr(8);
		} else if (key.substr(0, 7) == "--tree=") {
			if (key.length() < 8 || key[6] != '=') {
				std::cerr << "Parsing error: use \"tree=<path>\", "
					"no extra spaces" << std::endl;
				usage();
				return kStatus_WrongArgument;
			}
			if (!tree_path.empty()) {
				std::cerr << "Engine file was already specified "
					"in parameters" << std::endl;
				usage();
				return kStatus_WrongArgument;
			}
			tree_path = key.substr(7);
		}
		else if (key.substr(0, 12) == "--tree_path=") {
			if (key.length() < 13 || key[11] != '=') {
				std::cerr << "Parsing error: use \"tree_path=<path>\", "
					"no extra spaces" << std::endl;
				usage();
				return kStatus_WrongArgument;
			}

			tree_path_det.push_back(key.substr(12));
		
		} else {
			std::cerr << "Unknown key: " << key << std::endl;
			usage();
			return kStatus_WrongArgument;
		}
	}

	if (use_agent) {
		if (agent_path.size() == 0) {
			std::cerr << "No agent provided" << std::endl;
			usage();
			return kStatus_WrongArgument;
		}
		else {
			std::cout << "Using agent: " << agent_path << std::endl;
		}
	}
	else {
		if (det_path.size() == 0) {
			std::cerr << "No detector provided" << std::endl;
			usage();
			return kStatus_WrongArgument;
		}
		else {
			std::cout << "Using detector: " << det_path << std::endl;
		}
	}
	
	EStatus res = kStatus_Success;
	if (use_agent) {
		res = (command == "merge") ? merge_agent(tree_path, agent_path, tree_path_det) :
			(command == "export") ? export_agent(tree_path, agent_path, tree_path_det[0]) : kStatus_WrongArgument;
	}
	else {
		res = (command == "merge") ? merge_detector(tree_path, det_path, tree_path_det) :
			(command == "export") ? export_detector(tree_path, det_path, tree_path_det[0]) : kStatus_WrongArgument;
	}

	if (res == kStatus_Success) {
		std::cout << "Done" << std::endl;
	} else {
		std::cerr << "Stopped due to error" << std::endl;
    }

	return res;
}
