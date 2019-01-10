#include "PseudoInstance.h"

#include "../Log.h"

extern "C" {
#include "lua.h"
#include "lualib.h"
#include "lauxlib.h"
}

using namespace std;


PseudoInstance::PseudoInstance(tinyxml2::XMLElement * xml_root)
	:my_type(),unique_name(),transform(),script_parameters_float(),script_parameters_string()
{
	//Read data from the XML

	{
		tinyxml2::XMLElement * xml_child_type = xml_root->FirstChildElement("Type");
		if (xml_child_type != 0) {
			my_type = xml_child_type->GetText();
		}
	}

	{
		tinyxml2::XMLElement * xml_child_type = xml_root->FirstChildElement("UniqueName");
		if (xml_child_type != 0) {
			unique_name = xml_child_type->GetText();
		}
	}


	{//Get transform info from the child
		tinyxml2::XMLElement * xml_child_transform = xml_root->FirstChildElement("Transform");
		if (xml_child_transform != 0) {
			transform = Transform(xml_child_transform);
		}
		else {
			Log("Error", "PseudoInstance data incomplete, missing Transform");
		}
	}



	{
		tinyxml2::XMLElement * xml_child = xml_root->FirstChildElement("ScriptParameterFloat");
		while (xml_child != 0) {
			script_parameters_float.push_back(stof(xml_child->GetText()));
			xml_child = xml_child->NextSiblingElement("ScriptParameterFloat");
		}
	}


	{
		tinyxml2::XMLElement * xml_child = xml_root->FirstChildElement("Float");
		while (xml_child != 0) {
			script_parameters_float.push_back(stof(xml_child->GetText()));
			xml_child = xml_child->NextSiblingElement("Float");
		}
	}


	{
		tinyxml2::XMLElement * xml_child = xml_root->FirstChildElement("ScriptParameterString");
		while (xml_child != 0) {
			script_parameters_string.push_back(xml_child->GetText());
			xml_child = xml_child->NextSiblingElement("ScriptParameterString");
		}
	}


	{
		tinyxml2::XMLElement * xml_child = xml_root->FirstChildElement("String");
		while (xml_child != 0) {
			script_parameters_string.push_back(xml_child->GetText());
			xml_child = xml_child->NextSiblingElement("String");
		}
	}


};



PseudoInstance::PseudoInstance()
	:my_type(), unique_name(), transform(), script_parameters_float(), script_parameters_string()
{
};



tinyxml2::XMLElement* PseudoInstance::MakeXML(tinyxml2::XMLDocument & doc) const {
	tinyxml2::XMLElement * return_element = doc.NewElement("Part");

	tinyxml2::XMLElement * type_element = doc.NewElement("Type");
	type_element->SetText(my_type.c_str());
	return_element->InsertEndChild(type_element);

	tinyxml2::XMLElement * transform_element = transform.MakeXML(doc);
	return_element->InsertEndChild(transform_element);

	if (unique_name != "") {
		tinyxml2::XMLElement * unique_name_element = doc.NewElement("UniqueName");
		unique_name_element->SetText(unique_name.c_str());
		return_element->InsertEndChild(unique_name_element);
	}

	for (auto it = script_parameters_string.cbegin(); it != script_parameters_string.cend(); ++it) {
		tinyxml2::XMLElement * element = doc.NewElement("String");
		element->SetText(it->c_str());
		return_element->InsertEndChild(element);
	}

	for (auto it = script_parameters_float.cbegin(); it != script_parameters_float.cend(); ++it) {
		tinyxml2::XMLElement * element = doc.NewElement("Float");
		element->SetText(*it);
		return_element->InsertEndChild(element);
	}

	return return_element;
};