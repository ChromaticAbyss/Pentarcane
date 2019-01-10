#include "UiElementEffect.h"

using namespace std;
;

UiElementEffect::UiElementEffect() 
	:type(),for_mode(-1),value(),valueF(),stringValue(),trace()
{
};


UiElementEffect::UiElementEffect(tinyxml2::XMLElement * xml_root, UiElementParameter param)
	:type(), for_mode(-1), value(), valueF(), stringValue(), trace()
{

	{ 
		tinyxml2::XMLElement * xml_child = xml_root->FirstChildElement("Type");
		if (xml_child != 0) {
			type = xml_child->GetText();
		}
	}

	{ 
		tinyxml2::XMLElement * xml_child = xml_root->FirstChildElement("ForMode");
		if (xml_child != 0) {
			for_mode = stoi(xml_child->GetText());
		}
	}

	{
		tinyxml2::XMLElement * xml_child = xml_root->FirstChildElement("Value");
		while (xml_child != 0) {
			value.push_back(stoi(xml_child->GetText()));
			xml_child = xml_child->NextSiblingElement("Value");
		}
	}

	{
		tinyxml2::XMLElement * xml_child = xml_root->FirstChildElement("FloatValue");
		while (xml_child != 0) {
			valueF.push_back(stof(xml_child->GetText()));
			xml_child = xml_child->NextSiblingElement("FloatValue");
		}
	}

	{
		tinyxml2::XMLElement * xml_child = xml_root->FirstChildElement("StringValue");
		while (xml_child != 0) {
			stringValue.push_back(xml_child->GetText());
			xml_child = xml_child->NextSiblingElement("StringValue");
		}
	}

};