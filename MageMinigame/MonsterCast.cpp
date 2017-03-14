
#include "Log.h"
#include "MonsterCast.h"

#include "Monster.h"

#include <algorithm>

using namespace std;
using namespace tinyxml2;


MonsterCastSegment::MonsterCastSegment(int damage)
	:type("Damage"),values(),string_values(),targets("Player"), caster_id(-1)
{
	values.push_back(damage);
}

MonsterCastSegment::MonsterCastSegment(string name)
	: type("FlipbookAnimation"), values(), string_values(), targets("Player"), caster_id(-1)
{
	string_values.push_back(name);
}



MonsterCastSegment::MonsterCastSegment(XMLElement* p_definition)
	:type("MissingType"), caster_id(-1)
{
	if (p_definition == 0) {
		Log("Error", "MonsterCastSegment: Invalid pointer passed");
		return; //If we somehow got a nullpointer we can't extract any information and need to return
	}


	{
		XMLElement * xml_child = p_definition->FirstChildElement("Type");
		if (xml_child != nullptr) {
			type = xml_child->GetText();
		}
	}

	{
		XMLElement * xml_child = p_definition->FirstChildElement("Targets");
		if (xml_child != nullptr) {
			targets = xml_child->GetText();
		}
	}

	{
		XMLElement * xml_child = p_definition->FirstChildElement("StringValue");
		while (xml_child != nullptr) {
			string_values.push_back(xml_child->GetText());
			xml_child = xml_child->NextSiblingElement("StringValue");
		}
	}

	{
		XMLElement * xml_child = p_definition->FirstChildElement("Value");
		while (xml_child != nullptr) {
			values.push_back(stoi(xml_child->GetText()));
			xml_child = xml_child->NextSiblingElement("Value");
		}
	}


};

//Can't select player!
std::vector<int> MonsterCastSegment::SelectTargetsFrom(const std::vector<Monster>& monsters){ 
	vector<int> target_monster_ids;

	if (targets == "Self") {
		if (caster_id == -1) {
			Log("Error", "Self is -1 for a casting monster");
		}
		target_monster_ids.push_back(caster_id);
	}

	if (targets == "AllMonsters") { //Living ones only!
		for (unsigned int i = 0; i < monsters.size(); i++) {
			if (monsters[i].Alive()) {
				target_monster_ids.push_back(i);
			}
		}
	}

	if (targets == "RandomMonster") { //Living ones only!
		vector<int> cand;
		for (unsigned int i = 0; i < monsters.size(); i++) {
			if (monsters[i].Alive()) {
				cand.push_back(i);
			}
		}
		if (cand.size() > 0) {
			int z = rand() % cand.size();
			target_monster_ids.push_back(cand[z]);
		}
	}

	if (targets == "LowestHpMonster") { //Living ones only!
		int cand = -1;
		int val = 1000000;
		for (unsigned int i = 0; i < monsters.size(); i++) {
			int a = monsters[i].ReadStats().Health();
			if (a > 0 && val > a) {
				cand = i;
				val = a;
			}
		}
		if (cand >= 0) {
			target_monster_ids.push_back(cand);
		}
	}

	if (targets == "HighestHpMonster") { //Living ones only!
		int cand = -1;
		int val = -1000000;
		for (unsigned int i = 0; i < monsters.size(); i++) {
			int a = monsters[i].ReadStats().Health();
			if (a > 0 && val < a) {
				cand = i;
				val = a;
			}
		}
		if (cand >= 0) {
			target_monster_ids.push_back(cand);
		}
	}


	if (targets == "LastLivingMonster") {
		int cand = -1;
		for (unsigned int i = 0; i < monsters.size(); i++) {
			if (monsters[i].Alive()) {
				cand = i;
			}	
		}
		if (cand >= 0) {
			target_monster_ids.push_back(cand);
		}
	}


	return target_monster_ids;
};

MonsterCastSegment::MonsterCastSegment()
	:type("Finish")
{};




MonsterCast::MonsterCast() //Empty cast
	:execution_position(0), internal_name("Empty"), segments(), cast_conditions(),caster_id(0)
{
};

MonsterCast::MonsterCast(int damage, std::string animation_name)
	:execution_position(0), internal_name("DefaultAttack"), segments(), cast_conditions(), caster_id(0)
{

	segments.emplace_back(animation_name);

	segments.emplace_back(damage);

	segments.push_back(MonsterCastSegment());

};


MonsterCast::MonsterCast(XMLElement* p_definition)
	:execution_position(0),internal_name(),segments(), cast_conditions(), caster_id(0)
{
	if (p_definition == 0) {
		return; //If we somehow got a nullpointer we can't extract any information and need to return
	}

	{ 
		XMLElement * xml_child = p_definition->FirstChildElement("InternalName");
		if (xml_child != nullptr) {
			internal_name = xml_child->GetText();
		}
	}

	{ 
		XMLElement * xml_child = p_definition->FirstChildElement("Segment");
		while (xml_child != nullptr) {
			segments.emplace_back(xml_child); 
			xml_child = xml_child->NextSiblingElement("Segment");
		}
	}

	{
		XMLElement * xml_child = p_definition->FirstChildElement("CastCondition");
		while (xml_child != nullptr) {
			cast_conditions.emplace_back(xml_child);
			xml_child = xml_child->NextSiblingElement("CastCondition");
		}
	}


	//Log("Data", "Done loading MonsterDefinition with internalname: " + internal_name);
};


MonsterCastSegment MonsterCast::Progress(){
	if (execution_position >= segments.size()) {
		return MonsterCastSegment();//The default segment means "finished"
	}

	execution_position++;
	return segments[execution_position - 1];
}
void MonsterCast::SetCasterID(int a)
{
	caster_id = a; 
	for_each(segments.begin(), segments.end(), [a](MonsterCastSegment& seg) {seg.SetCasterID(a); });
}
;