#pragma once

#include "tinyxml.h"

static bool save_xml(const std::string& fn, TiXmlElement* element)
{
	TiXmlDocument doc;
	TiXmlDeclaration* decl = new TiXmlDeclaration("1.0", "", "");
	doc.LinkEndChild(decl);
	doc.LinkEndChild(element);

	return doc.SaveFile(fn);
}

template<class C>
static auto load_xml(const std::string& fn, C&& create)
{
	TiXmlDocument doc;

	TiXmlElement* node = nullptr;

	if (doc.LoadFile(fn, TIXML_ENCODING_UTF8)) {
		TiXmlHandle handle(&doc);
		node = handle.FirstChildElement().Element();
	}
	return create(node);
}

template<typename T>
static std::vector<T>	split_vector(const std::string& str) {

	std::vector<T>		result;
	std::istringstream ss(str);
	T value = 0;
	while (ss >> value) {
		result.push_back(value);
	}
	return result;
}


template<typename T>
static bool	split_vector(const std::string& str, T* ptr, size_t size) {

	std::vector<T>		result;
	std::istringstream ss(str);
	T value = 0;
	while (size > 0 && ss >> *ptr) {
		size--;
		ptr++;
	}
	return size > 0;
}

template<typename T>
static std::string	serialize_vector(const std::vector<T>& vec) {

	std::stringstream ss;

	if (!vec.empty()) {
		for (size_t i = 0; i < vec.size() - 1; ++i) {
			ss << vec[i] << ' ';
		}
		ss << vec.back();
	}

	return ss.str();

}

template<typename T>
static std::string	serialize_vector(const T* vec, size_t size) {

	std::stringstream ss;

	if (size > 0) {
		for (size_t i = 0; i < size - 1; ++i) {
			ss << vec[i] << ' ';
		}
		ss << vec[size - 1];
	}

	return ss.str();

}



// Функция для сравнения двух элементов
static bool compare_xml_elements(TiXmlElement* elem1, TiXmlElement* elem2) {
	if (!elem1 || !elem2) {
		// Null element;
		return false;
	}

	// Сравнение тегов
	if (std::string(elem1->Value()) != std::string(elem2->Value())) {
		// Tag mismatch
		return false;
	}

	// Сравнение текста
	const char* text1 = elem1->GetText();
	const char* text2 = elem2->GetText();
	if ((text1 ? std::string(text1) : "") != (text2 ? std::string(text2) : "")) {
		return false;
	}

	// Сравнение атрибутов
	TiXmlAttribute* attr1 = elem1->FirstAttribute();
	TiXmlAttribute* attr2 = elem2->FirstAttribute();
	while (attr1 || attr2) {
		if (!attr1 || !attr2 || std::string(attr1->Name()) != std::string(attr2->Name()) || std::string(attr1->Value()) != std::string(attr2->Value())) {
			return false;
		}
		attr1 = attr1->Next();
		attr2 = attr2->Next();
	}

	// Сравнение дочерних элементов
	TiXmlElement* child1 = elem1->FirstChildElement();
	TiXmlElement* child2 = elem2->FirstChildElement();
	int index = 0;
	while (child1 || child2) {
		if (!child1 || !child2) {
			// Child element count mismatch
			return false;
		}
		if (!compare_xml_elements(child1, child2))
			return false;
		child1 = child1->NextSiblingElement();
		child2 = child2->NextSiblingElement();
		index++;
	}

	return true;
}

