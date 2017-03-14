#include "UiElementEffect.h"

using namespace std;
using namespace tinyxml2;

UiElementEffect::UiElementEffect() 
	:type(),for_mode(),value(),valueF(),stringValue(),trace()
{
};


UiElementEffect::UiElementEffect(tinyxml2::XMLElement * xml_root, UiElementParameter param)
	:type(), for_mode(), value(), valueF(), stringValue(), trace()
{

	{ 
		XMLElement * xml_child = xml_root->FirstChildElement("Type");
		if (xml_child != 0) {
			type = xml_child->GetText();
		}
	}

	{ 
		XMLElement * xml_child = xml_root->FirstChildElement("ForMode");
		if (xml_child != 0) {
			for_mode = stoi(xml_child->GetText());
		}
	}

	{
		XMLElement * xml_child = xml_root->FirstChildElement("Value");
		while (xml_child != 0) {
			value.push_back(stoi(xml_child->GetText()));
			xml_child = xml_child->NextSiblingElement("Value");
		}
	}

	{
		XMLElement * xml_child = xml_root->FirstChildElement("FloatValue");
		while (xml_child != 0) {
			valueF.push_back(stof(xml_child->GetText()));
			xml_child = xml_child->NextSiblingElement("FloatValue");
		}
	}

	{
		XMLElement * xml_child = xml_root->FirstChildElement("StringValue");
		while (xml_child != 0) {
			stringValue.push_back(xml_child->GetText());
			xml_child = xml_child->NextSiblingElement("StringValue");
		}
	}

};