#pragma once

#include <vector>
#include <string>

#include "../tinyxml2.h"

class UiElementParameter {
public:
	UiElementParameter();
	UiElementParameter(std::vector<int> a, std::vector<float> b, std::vector<std::string> c);

	int Int_Value(int);
	float Float_Value(int);
	std::string String_Value(int);
	friend bool operator== (const UiElementParameter &c1, const UiElementParameter &c2); // Expensive if called often and on objects with large string vectors

	bool EvaluateCondition(tinyxml2::XMLElement * xml_root) const;

private:
	std::string type;

	std::vector<int> int_values;
	std::vector<float> float_values;
	std::vector<std::string> string_values;
};
