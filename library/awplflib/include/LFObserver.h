#pragma once

#include <vector>
#include <memory>
#include <string>
#include "tinyxml.h"

class TLFObserver {
public:
	virtual ~TLFObserver() {}

	std::shared_ptr<TLFObserver>		CreateObserver(const std::string& name) {

	}

	void		Add(const std::string& name, double value) {
		values_.push_back({ name, value });
	}

	void		Remove(const std::string& name) {
		for (auto it = values_.begin(); it != values_.end(); ++it) {
			if (it->first == name) {
				values_.erase(it);
				break;
			}
		}
	}

	void		Clear() {
		values_.clear();
		for (size_t i = 0; i < observers_.size(); ++i) {
			observers_[i]->Clear();
		}
	}

	std::unique_ptr<TiXmlElement> SaveXML() const {

		auto parent = std::make_unique<TiXmlElement>("LFObserver");

		parent->SetAttribute("name", name_.c_str());

		for (size_t i = 0; i < observers_.size(); ++i) {
			auto obs = observers_[i]->SaveXML();
			parent->LinkEndChild(obs.release());
		}

		for (size_t i = 0; i < values_.size(); ++i) {
			auto value = std::make_unique<TiXmlElement>("LFValue");
			value->SetAttribute("name", values_[i].first.c_str());
			value->SetValue(std::to_string(values_[i].second).c_str());
			parent->LinkEndChild(value.release());
		}

		return parent;
	}

private:
	std::string										name_;
	std::vector<std::shared_ptr<TLFObserver>>	    observers_;

	std::vector<std::pair<std::string, double>>		values_;

};