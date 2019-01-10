#pragma once


#include <vector>

#include "tinyxml2.h"

#include "PseudoInstance.h"

class VariantCarrier;


class Variant {
public:
	Variant(tinyxml2::XMLElement * xml_root);

	tinyxml2::XMLElement* MakeXML(tinyxml2::XMLDocument&) const;

private:
	int min;
	int max;

	std::vector<PseudoInstance> parts;

	std::vector<VariantCarrier> sub_variants;
};

class VariantCarrier {
public:
	VariantCarrier(tinyxml2::XMLElement * xml_root);

	tinyxml2::XMLElement* MakeXML(tinyxml2::XMLDocument&) const;
private:

	std::vector<Variant> var;

};
