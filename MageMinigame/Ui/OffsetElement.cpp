#include "OffsetElement.h"

using namespace std;
using namespace tinyxml2;

OffsetElement::OffsetElement()
	:h(Variants_H::center),v(Variants_V::center_v)
{
}

OffsetElement::OffsetElement(tinyxml2::XMLElement * xml_root)
	:h(Variants_H::center), v(Variants_V::center_v)
{
	{ //Look up internal name
		XMLElement * xml_child = xml_root->FirstChildElement("Horizontal");
		if (xml_child != nullptr) {
			string text = xml_child->GetText();
			if (text == "Left" || text == "left") {
				h = Variants_H::left;
			}
			if (text == "Right" || text == "right") {
				h = Variants_H::right;
			}
		}
	}

	{ //Look up internal name
		XMLElement * xml_child = xml_root->FirstChildElement("Vertical");
		if (xml_child != nullptr) {
			string text = xml_child->GetText();
			if (text == "Top" || text == "top") {
				v = Variants_V::top;
			}
			if (text == "Bot" || text == "bot") {
				v = Variants_V::bot;
			}
		}
	}



}

std::pair<float, float> OffsetElement::Calc(std::pair<float, float> lower_left, std::pair<float, float> upper_right) const
{
	auto x_l = get<0>(lower_left);
	auto y_l = get<1>(lower_left);
	auto x_r = get<0>(upper_right);
	auto y_r = get<1>(upper_right);
	auto o_x = -(x_l + x_r) / 2;//0.0f; //Should no assume that the entity is nicely centered around 0|0 (text fields are not for example!)
	auto o_y = -(y_l + y_r) / 2;//0.0f;
	if (v == Variants_V::bot) {
		o_y = -y_l;
	}
	if (v == Variants_V::top) {
		o_y = -y_r;
	}
	if (h == Variants_H::left) {
		o_x = -x_l;
	}
	if (h == Variants_H::right) {
		o_x = -x_r;
	}

	return std::pair<float, float>(o_x,o_y);
}
