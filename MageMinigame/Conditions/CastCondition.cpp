#include "CastCondition.h"

#include "../Monster.h"

using namespace std;
using namespace tinyxml2;

CastCondition::CastCondition()
	:type(TypeEnum::nothing),target(),conditions_to_monster()
{
}

CastCondition::CastCondition(tinyxml2::XMLElement * xml_root)
	:type(TypeEnum::nothing),target(),conditions_to_monster()
{


	{
		XMLElement * xml_child = xml_root->FirstChildElement("Type");
		if (xml_child != nullptr) {
			string t = xml_child->GetText();
			if (t == "ConditionToMonster") {
				InitializeAsConditionToMonster(xml_child);
			}
			
		}
	}






}

bool CastCondition::Evaluate(std::vector<Monster> monsters, int self)
{
	bool needs_condition_on_all = false;
	vector<int> target_monster_ids;

	if (target == "Self") {
		target_monster_ids.push_back(self);
	}







	return false;
}

void CastCondition::InitializeAsConditionToMonster(tinyxml2::XMLElement * xml_root)
{
	type = TypeEnum::condition_to_monster;

	{
		XMLElement * xml_child = xml_root->FirstChildElement("Type");
		if (xml_child != nullptr) {
			target = xml_child->GetText();
		}
	}


	{
		XMLElement * xml_child = xml_root->FirstChildElement("ConditionToMonster");
		while (xml_child != nullptr) {
			conditions_to_monster.emplace_back(xml_child->GetText());
			xml_child = xml_child->NextSiblingElement("ConditionToMonster");
		}
	}


}
