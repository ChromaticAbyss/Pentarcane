#pragma once

#include <vector>
#include <string>

#include "../tinyxml2.h"
#include "uiElementParameter.h"

class UiElementEffect {
public:
	UiElementEffect();
	UiElementEffect(tinyxml2::XMLElement * xml_root, UiElementParameter param);

	std::string type;
	int for_mode;
	std::vector<int> value;
	std::vector<float> valueF;
	std::vector<std::string> stringValue;

	std::vector<int> trace; //Trace is filled when returning the object, in very return vector trace is only defined for the effect in [0]. Use it to find that specific UI element in the tree (i.e. for destruction)
};





