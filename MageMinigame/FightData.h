#pragma once

#include <string>
#include <vector>

#include "tinyxml2.h"

#include "Stats.h"

struct FightDataCondition {
	FightDataCondition(tinyxml2::XMLElement *);
	enum condition_type{AND,OR,NOT,Difficulty,ExactName,HasTag};
	condition_type c_type;
	enum condition_modifier {UNINIT_MOD,Equal,Less,LessOrEqual,More,MoreOrEqual,Inequal};
	condition_modifier c_mod;

	std::vector<float> values;
	std::vector<std::string> string_values;

	std::vector<FightDataCondition> sub_conditions;
};


struct FightData {
	FightData();
	FightData(tinyxml2::XMLElement *);

	std::string stage_name;
	std::vector<std::string> flavour_text;

	Stats monster_stat_modifier; //A bonus set of stats added to all monsters in this battle

	std::vector<FightDataCondition> encounter_conditions;//Conditions for the spawner to select by
};
