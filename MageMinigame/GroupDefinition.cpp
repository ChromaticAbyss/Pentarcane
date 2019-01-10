#include "GroupDefinition.h"
#include "Log.h"
#include "XMLUtil.h"
;

using namespace std;

#include <algorithm>


GroupDefinition::GroupDefinition(const tinyxml2::XMLElement * root)
	:internal_name("GroupDef internal_name not set"),monsters(),tags(),likelihood(1000),power_rating(100)
{
	if (root == 0) {
		return; //If we somehow got a nullpointer we can't extract any information and need to return
	}
	internal_name = LoadSingleValue_Required<std::string>(root,"InternalName");
	monsters = LoadVectorOf<std::string>(root, "Monster");
	tags = LoadVectorOf<std::string>(root, "Tag");
	likelihood = LoadSingleValue(root, "Likelyhood",1000);
	power_rating = LoadSingleValue(root, "PowerRating", 100);
};


bool GroupDefinition::HasTag(string look_for) const{
	return any_of(tags.cbegin(), tags.cend(), [&look_for](const string& tag) {return tag == look_for; });
}

bool GroupDefinition::FulfillsCondition(const std::vector<FightDataCondition>&  cond) const {
	return !any_of(cond.cbegin(), cond.cend(), [this](const FightDataCondition& d) {return !FulfillsCondition(d); });
}

bool GroupDefinition::FulfillsCondition(const FightDataCondition& cond) const {
	if (cond.c_type == cond.AND) {
		return FulfillsCondition(cond.sub_conditions);
	}
	if (cond.c_type == cond.OR) {
		for (auto it = cond.sub_conditions.begin(); it != cond.sub_conditions.end(); ++it) {
			if (FulfillsCondition(*it) == true) { return true; }
		}
		return false;
	}
	if (cond.c_type == cond.NOT) { //One sub condition true -> construct false!
		for (auto it = cond.sub_conditions.begin(); it != cond.sub_conditions.end(); ++it) {
			if (FulfillsCondition(*it) == true) { return false; }
		}
		return true;
	}


	//Value based
	if (cond.c_type == cond.Difficulty) {
		return ValueCondition(cond,power_rating);
	}

	if (cond.c_type == cond.ExactName) {
		//cout << internal_name << " vs " << cond.string_values[0] << endl;
		return (internal_name == cond.string_values[0]);
	}

	if (cond.c_type == cond.HasTag) { //Slow, but then again this is called only on monster spawn, so whatever
		string tag = cond.string_values[0];
		for (auto it = tags.begin(); it != tags.end(); ++it) {
			if (*it == tag) { 
				return true; 
			}
		}
		return false;
	}


	Log("TODO", intToString(cond.c_type) + "not a valid condition" );
	return false;
}


//If this ever gets used somewhere else consider making it a function of the Condition object itself
bool GroupDefinition::ValueCondition(const FightDataCondition&  cond,float value) const {
	//cout <<"v "<< value << endl;
	//cout << "c " << cond.values[0] << endl;
	//Value operation
	if (cond.c_mod == cond.Less) {
		return (value <	cond.values[0]);
	}
	if (cond.c_mod == cond.LessOrEqual) {
		return (value <= cond.values[0]);
	}
	if (cond.c_mod == cond.More) {
		return (value >	cond.values[0]);
	}
	if (cond.c_mod == cond.MoreOrEqual) {
		return (value >= cond.values[0]);
	}
	if (cond.c_mod == cond.Equal) { //TODO: Small tolerance!
		//Log("TODO", "Group Definition Cond.Equal: Tolerance!");
		return (value == cond.values[0]);
	}
	if (cond.c_mod == cond.Inequal) { //TODO: Small tolerance!
		//Log("TODO", "Group Definition Cond.Inequal: Tolerance!");
		return (value != cond.values[0]);
	}
	Log("Error", "Invalid value condition");
	return false;
}