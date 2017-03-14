#include "FightData.h"

#include "Log.h"

using namespace std;
using namespace tinyxml2;


FightDataCondition::FightDataCondition(XMLElement * xml_root)
	:c_type(AND), c_mod(UNINIT_MOD), values(), string_values()
{
	if (xml_root == 0) {
		Log("Error", "FightDataConstruction, nil pointer");
		return;
	}


	{ 
		XMLElement * xml_child = xml_root->FirstChildElement("Type");
		if (xml_child != 0) {
			string text = xml_child->GetText();
			if (text == "AND") {
				c_type = AND;
			}
			if (text == "OR") {
				c_type = OR;
			}
			if (text == "NOT") {
				c_type = NOT;
			}
			if (text == "Difficulty") {
				c_type = Difficulty;
			}
			if (text == "ExactName") {
				c_type = ExactName;
			}
			if (text == "HasTag") {
				c_type = HasTag;
			}
		}
	}


	{
		XMLElement * xml_child = xml_root->FirstChildElement("Modifier");
		if (xml_child != 0) {
			string text = xml_child->GetText();
			if (text == "Equal") {
				c_mod = Equal;
			}
			if (text == "Less") {
				c_mod = Less;
			}
			if (text == "LessOrEqual") {
				c_mod = LessOrEqual;
			}
			if (text == "More") {
				c_mod = More;
			}
			if (text == "MoreOrEqual") {
				c_mod = MoreOrEqual;
			}
			if (text == "Inequal") {
				c_mod = Inequal;
			}
		}
	}




	{
		XMLElement * xml_child = xml_root->FirstChildElement("Value");
		if (xml_child != 0) {
			values.push_back(stof(xml_child->GetText()));
			xml_child = xml_child->NextSiblingElement("Value");
		}
	}

	{
		XMLElement * xml_child = xml_root->FirstChildElement("StringValue");
		while (xml_child != 0) {
			string_values.push_back(xml_child->GetText());
			xml_child = xml_child->NextSiblingElement("StringValue");
		}
	}


	{
		XMLElement * xml_child = xml_root->FirstChildElement("Condition");
		while (xml_child != 0) {
			sub_conditions.emplace_back(xml_child);
			xml_child = xml_child->NextSiblingElement("Condition");
		}
	}

}

FightData::FightData()
	:stage_name(""),flavour_text(),encounter_conditions()
{
}


FightData::FightData(XMLElement * xml_root)
	: stage_name(""), flavour_text(), monster_stat_modifier(), encounter_conditions()
{


	{
		XMLElement * xml_child = xml_root->FirstChildElement("Stage");
		if (xml_child != 0) {
			stage_name = xml_child->GetText();
		}
	}


	{
		XMLElement * xml_child = xml_root->FirstChildElement("Flavour");
		while(xml_child != 0) {
			flavour_text.push_back(xml_child->GetText());
			xml_child = xml_child->NextSiblingElement("Flavour");
		}
	}


	{
		XMLElement * xml_child = xml_root->FirstChildElement("BonusStats");
		if (xml_child != 0) {
			monster_stat_modifier = Stats(xml_child);
		}
	}


	{
		XMLElement * xml_child = xml_root->FirstChildElement("Condition");
		while(xml_child != 0) {
			encounter_conditions.emplace_back(xml_child);
			xml_child = xml_child->NextSiblingElement("Condition");
		}
	}

}