#pragma once

#include <vector>
#include <string>
#include "../tinyxml2.h"

class OffsetElement {
public:
	OffsetElement();
	OffsetElement(tinyxml2::XMLElement * xml_root);

	std::pair<float, float> Calc(std::pair<float,float> lower_left,std::pair<float,float> upper_right) const;


	enum Variants_H {center,left,right};
	enum Variants_V {center_v,top,bot};

private:
	Variants_H h;
	Variants_V v;
};