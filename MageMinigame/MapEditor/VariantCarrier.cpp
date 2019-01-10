#include "VariantCarrier.h"

#include "../Log.h"

using namespace std;


Variant::Variant(tinyxml2::XMLElement * xml_root) 
	:min(0),max(100)
{
	//Read attribute: Min, Max
	xml_root->QueryIntAttribute("Min", &min);
	xml_root->QueryIntAttribute("Max", &max);


	tinyxml2::XMLElement * xml_sub_variant = xml_root->FirstChildElement("Variant");
	while (xml_sub_variant != 0) {
		sub_variants.push_back(VariantCarrier(xml_sub_variant));
		xml_sub_variant = xml_sub_variant->NextSiblingElement("Variant");
	}

	{ //Look up parts
		tinyxml2::XMLElement * xml_child = xml_root->FirstChildElement("Part");
		while (xml_child != 0) {
			//Log("TO  DO", "Map Editor: Load parts in Variants in some reasonable way that allows saving them back"); //that way would be xml_stroage container object like in Explo. Not needed aymore though.
			parts.push_back(PseudoInstance(xml_child));

			xml_child = xml_child->NextSiblingElement("Part");
		}//while children are found
	}//look up parts

};

tinyxml2::XMLElement* Variant::MakeXML(tinyxml2::XMLDocument& doc) const {
	tinyxml2::XMLElement * return_element = doc.NewElement("Var");

	return_element->SetAttribute("Min", min);
	return_element->SetAttribute("Max", max);

	for (auto it = sub_variants.cbegin(); it != sub_variants.cend(); ++it) {
		tinyxml2::XMLElement * element = it->MakeXML(doc);
		return_element->InsertEndChild(element);
	}

	for (auto it = parts.cbegin(); it != parts.cend(); ++it) {
		tinyxml2::XMLElement * element = it->MakeXML(doc);
		return_element->InsertEndChild(element);
	}

	return return_element;

};

VariantCarrier::VariantCarrier(tinyxml2::XMLElement * xml_root) {

	tinyxml2::XMLElement * xml_variant = xml_root->FirstChildElement("Var");
	while (xml_variant != 0) {
		
		var.push_back(Variant(xml_variant));

		xml_variant = xml_variant->NextSiblingElement("Var");
	}

};


tinyxml2::XMLElement* VariantCarrier::MakeXML(tinyxml2::XMLDocument& doc) const {
	tinyxml2::XMLElement * return_element = doc.NewElement("Variant");

	for (auto it = var.cbegin(); it != var.cend(); ++it) {
		tinyxml2::XMLElement * element = it->MakeXML(doc);
		return_element->InsertEndChild(element);
	}

	return return_element;

};
