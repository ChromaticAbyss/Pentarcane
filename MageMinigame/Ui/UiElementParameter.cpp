#include "UiElementParameter.h"

#include <exception>

#include "../Log.h"

using namespace std;
using namespace tinyxml2;

UiElementParameter::UiElementParameter()
	:int_values(),float_values(),string_values()
{


};

UiElementParameter::UiElementParameter(std::vector<int> a, std::vector<float> b, std::vector<std::string> c) 
	:int_values(a),float_values(b),string_values(c)
{

};


int UiElementParameter::Int_Value(int id) {
	if (id < 0 || id >= (int)int_values.size()) {
		Log("Error", "UiElementParameter: Int_Value bad id. This type of error is most likely caused by using the wrong file!");
		throw std::out_of_range("UiElementParameter: Int_Value, id is invalid");
	}
	return int_values[id];
};
float UiElementParameter::Float_Value(int id) {
	if (id < 0 || id >= (int)float_values.size()) {
		Log("Error", "UiElementParameter: Float_Value bad id. This type of error is most likely caused by using the wrong file!");
		throw std::out_of_range("UiElementParameter: Float_Value, id is invalid");
	}
	return float_values[id];
};
std::string UiElementParameter::String_Value(int id) {
	if (id < 0 || id >= (int)string_values.size()) {
		Log("Error", "UiElementParameter: String_Value bad id. This type of error is most likely caused by using the wrong file!");
		throw std::out_of_range("UiElementParameter: String_Value, id is invalid");
	}
	return string_values[id];
};


bool operator== (const UiElementParameter &c1, const UiElementParameter &c2)
{
	if (c1.int_values.size() != c2.int_values.size()) { return false; }
	if (c1.float_values.size() != c2.float_values.size()) { return false; }
	if (c1.string_values.size() != c2.string_values.size()) { return false; }

	int n = c1.int_values.size();
	for (int ii = 0; ii < n; ii++) {
		if (c1.int_values[ii] != c2.int_values[ii]) { return false; }
	}

	int n2 = c1.float_values.size();
	for (int ii = 0; ii < n2; ii++) {
		if (c1.float_values[ii] != c2.float_values[ii]) { return false; }
	}

	//Expensive!
	int n3 = c1.string_values.size();
	for (int ii = 0; ii < n3; ii++) {
		if (c1.string_values[ii] != c2.string_values[ii]) { return false; }
	}

	return true;
}

bool UiElementParameter::EvaluateCondition(XMLElement * xml_root) const {

	{
		XMLElement * xml_child = xml_root->FirstChildElement("IntValueEqual");
		while (xml_child != 0) {
			int index = 0;
			xml_child->QueryIntAttribute("Index", &index);
			int value = 0;
			xml_child->QueryIntAttribute("Value", &value);

			if (int_values[index] != value) { return false; }

			xml_child = xml_child->NextSiblingElement("IntValueEqual");
		}
	}

	{
		XMLElement * xml_child = xml_root->FirstChildElement("IntValueInequal");
		while (xml_child != 0) {
			int index = 0;
			xml_child->QueryIntAttribute("Index", &index);
			int value = 0;
			xml_child->QueryIntAttribute("Value", &value);

			if (int_values[index] == value) { return false; }

			xml_child = xml_child->NextSiblingElement("IntValueInequal");
		}
	}


	{
		XMLElement * xml_child = xml_root->FirstChildElement("IntValueGreaterThan");
		while (xml_child != 0) {
			int index = 0;
			xml_child->QueryIntAttribute("Index", &index);
			int value = 0;
			xml_child->QueryIntAttribute("Value", &value);

			if (int_values[index] <= value) { 				
				return false; 
			}

			xml_child = xml_child->NextSiblingElement("IntValueGreaterThan");
		}
	}


	{
		XMLElement * xml_child = xml_root->FirstChildElement("IntValueSmallerThan");
		while (xml_child != 0) {
			int index = 0;
			xml_child->QueryIntAttribute("Index", &index);
			int value = 0;
			xml_child->QueryIntAttribute("Value", &value);

			if (int_values[index] >= value) { return false; }

			xml_child = xml_child->NextSiblingElement("IntValueSmallerThan");
		}
	}

	return true;
}